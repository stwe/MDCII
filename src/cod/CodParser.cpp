#include <fstream>
#include <regex>
#include <google/protobuf/util/json_util.h>
#include "CodParser.h"
#include "CodHelper.h"
#include "Game.h"

mdcii::cod::CodParser::CodParser(std::string t_codFilePath, const bool t_decode)
    : m_path{ std::move(t_codFilePath) }
{
    ReadFile(t_decode);
    ParseFile();
}

bool mdcii::cod::CodParser::ReadFile(const bool t_decode)
{
    std::ifstream input(m_path, std::ios::binary);
    std::vector<unsigned char> buffer(std::istreambuf_iterator(input), {});

    if (t_decode)
    {
        for (auto& c : buffer)
        {
            c = -c;
        }
    }

    std::string line;
    for (size_t i{ 0 }; i < buffer.size() - 1; ++i)
    {
        if (buffer[i + 1] != '\n' && buffer[i] != '\r')
        {
            line.append(1, buffer[i]);
        }
        else
        {
            auto res{ CodHelper::TrimCommentFromLine(CodHelper::TabsToSpaces(line)) };
            if (res.empty())
            {
                line = "";
            }
            else
            {
                line = res[0];
            }

            if (!CodHelper::IsEmpty(line))
            {
                m_codTxt.push_back(line);
            }
            line = "";
            i++; // hop over '\n'
        }
    }

    return true;
}

void mdcii::cod::CodParser::ParseFile()
{
    std::map<std::string, int> variableNumbers;
    std::map<std::string, std::vector<int>> variableNumbersArray;

    for (unsigned int lineIndex{ 0 }; lineIndex < m_codTxt.size(); ++lineIndex)
    {
        auto line{ m_codTxt[lineIndex] };
        auto spaces{ CodHelper::CountFrontSpaces(line) };

        if (CodHelper::IsSubstring(line, "Nahrung:") || CodHelper::IsSubstring(line, "Soldat:") || CodHelper::IsSubstring(line, "Turm:"))
        {
            // TODO : skipped for now
            continue;
        }

        {
            // constant assignment
            auto result{ CodHelper::RegexSearch("(@?)(\\w+)\\s*=\\s*((?:\\d+|\\+|\\w+)+)", line) };
            if (!result.empty())
            {
                auto isMath{ CodHelper::IsSubstring(result[3], "+") };
                auto key{ result[2] };
                auto value{ result[3] };

                // example: 'HAUSWACHS = Nummer'
                if (value == "Nummer")
                {
                    if (variableNumbers.count(value))
                    {
                        auto number{ variableNumbers[value] };
                        auto i{ ConstantExists(key) };
                        cod_pb::Variable* variable{ nullptr };
                        if (i != -1)
                        {
                            variable = m_constants.mutable_variable(i);
                        }
                        else
                        {
                            variable = m_constants.add_variable();
                        }
                        variable->set_name(key);
                        variable->set_value_string(std::to_string(number));
                    }
                }
                else
                    // example: 'IDHANDW =   20501'
                {
                    auto i{ ConstantExists(key) };
                    cod_pb::Variable* variable{ nullptr };
                    if (i != -1)
                    {
                        variable = m_constants.mutable_variable(i);
                    }
                    else
                    {
                        variable = m_constants.add_variable();
                    }
                    *variable = GetValue(key, value, isMath, m_constants);
                }
                continue;
            }
        }
        {
            // example: '@Pos:       +0, +42'
            auto result{ CodHelper::RegexSearch("@(\\w+):.*(,)", line) };
            if (!result.empty())
            {
                auto name{ result[1] };
                std::vector<int> offsets;
                result = CodHelper::RegexSearch(":\\s*(.*)", line);
                if (!result.empty())
                {
                    auto tokens{ CodHelper::SplitByDelimiter(result[1], ",") };
                    for (auto& e : tokens)
                    {
                        e = CodHelper::TrimSpacesLeadingTrailing(e);
                        auto number{ CodHelper::RegexSearch("([+|-]?)(\\d+)", e) };
                        auto offset{ std::stoi(number[2]) };
                        if (number[1] == "-")
                        {
                            offset *= -1;
                        }
                        offsets.push_back(offset);
                    }
                }
                auto index{ ExistsInCurrentObject(name) };
                std::vector<int> currentArrayValues;
                for (unsigned int i{ 0 }; i < offsets.size(); ++i)
                {
                    auto currentValue{ 0 };
                    if (index != -1)
                    {
                        currentValue = variableNumbersArray[m_currentObject->variables().variable(index).name()][i];
                        currentValue = CalculateOperation(currentValue, "+", offsets[i]);
                        m_currentObject->mutable_variables()->mutable_variable(index)->mutable_value_array()->mutable_value(i)->set_value_int(currentValue);
                    }
                    else
                    {
                        currentValue = CalculateOperation(variableNumbersArray[name][i], "+", offsets[i]);
                        auto var{ CreateOrReuseVariable(name) };
                        var->set_name(name);
                        var->mutable_value_array()->add_value()->set_value_int(currentValue);
                    }
                    currentArrayValues.push_back(currentValue);
                }
                variableNumbersArray[name] = currentArrayValues;
                continue;
            }
        }
        {
            if (CodHelper::IsSubstring(line, ",") && !CodHelper::IsSubstring(line, "ObjFill"))
            {
                // example:
                // Arr: 5, 6
                // Var: 10-Arr[0], 20+Arr[1]
                if (CodHelper::IsSubstring(line, "["))
                {
                    auto result{ CodHelper::RegexSearch("(\\w+)\\s*:\\s*(.+)", line) };
                    if (!result.empty())
                    {
                        auto name{ result[1] };
                        auto var{ CreateOrReuseVariable(name) };
                        if (ExistsInCurrentObject(name))
                        {
                            var->mutable_value_array()->Clear();
                        }
                        var->set_name(name);
                        auto values{ result[2] };
                        auto splitValues{ CodHelper::SplitByDelimiter(values, ",") };
                        for (const auto& v : splitValues)
                        {
                            auto tokens{ CodHelper::RegexSearch("((\\d+)([+|-])(\\w+)\\[(\\d+)\\])", v) };
                            if (!tokens.empty())
                            {
                                auto index{ ExistsInCurrentObject(tokens[4]) };
                                if (index != -1)
                                {
                                    int arrayValue = m_currentObject->variables().variable(index).value_array().value(std::stoi(tokens[5])).value_int();
                                    int newValue = CalculateOperation(std::stoi(tokens[2]), tokens[3], arrayValue);
                                    var->mutable_value_array()->add_value()->set_value_int(newValue);
                                }
                            }
                        }
                    }
                }
                else
                    // example: Var: 1, 2, 3
                {
                    auto result{ CodHelper::RegexSearch("(\\b(?!Objekt\\b)\\w+)\\s*:\\s*(.+)", line) };
                    if (!result.empty())
                    {
                        if (m_objectStack.empty())
                        {
                            m_unparsedLines.push_back(line);
                            continue;
                        }

                        std::vector<int> currentArrayValues;
                        auto values{ CodHelper::SplitByDelimiter(result[2], ",") };
                        for (auto& v : values)
                        {
                            v = CodHelper::TrimSpacesLeadingTrailing(v);
                        }
                        auto varExists{ ExistsInCurrentObject(result[1]) };
                        auto var{ CreateOrReuseVariable(result[1]) };
                        var->set_name(result[1]);
                        auto arr{ var->mutable_value_array() };

                        if (varExists)
                        {
                            arr->Clear();
                        }

                        for (auto v : values)
                        {
                            if (CheckType(v) == CodValueType::INT)
                            {
                                arr->add_value()->set_value_int(std::stoi(v));
                                currentArrayValues.push_back(std::stoi(v));
                            }
                            else if (CheckType(v) == CodValueType::FLOAT)
                            {
                                arr->add_value()->set_value_float(std::stof(v));
                            }
                            else
                            {
                                auto i{ ConstantExists(v) };
                                if (i != -1)
                                {
                                    auto var{ GetVariable(v) };
                                    if (var.Value_case() == cod_pb::Variable::ValueCase::kValueInt)
                                    {
                                        arr->add_value()->set_value_int(var.value_int());
                                        currentArrayValues.push_back(var.value_int());
                                    }
                                    else if (var.Value_case() == cod_pb::Variable::ValueCase::kValueFloat)
                                    {
                                        arr->add_value()->set_value_float(var.value_float());
                                    }
                                    else
                                    {
                                        arr->add_value()->set_value_string(var.value_string());
                                    }
                                }
                                else
                                {
                                    arr->add_value()->set_value_string(v);
                                }
                            }
                        }
                        variableNumbersArray[result[1]] = currentArrayValues;
                    }
                }
                continue;
            }
        }
        {
            // example: '@Gfx:       -36'
            auto result{ CodHelper::RegexSearch("((@)(\\b(?!Nummer\\b)\\w+))\\s*:\\s*([+|-]?)(\\d+)", line) };
            if (!result.empty())
            {
                auto index{ ExistsInCurrentObject(result[3]) };

                auto currentValue{ 0 };
                if (index != -1)
                {
                    auto var{ variableNumbers[m_currentObject->variables().variable(index).name()] };
                    currentValue = CalculateOperation(var, result[4], std::stoi(result[5]));
                    m_currentObject->mutable_variables()->mutable_variable(index)->set_value_int(currentValue);
                }
                else
                {
                    currentValue = CalculateOperation(variableNumbers[result[3]], result[4], std::stoi(result[5]));
                    auto var{ CreateOrReuseVariable(result[3]) };
                    var->set_name(result[3]);
                    var->set_value_int(currentValue);
                }
                variableNumbers[result[3]] = currentValue;
                continue;
            }
        }
        {
            // example: 'Gfx:        GFXBODEN+80'
            auto result{ CodHelper::RegexSearch("(\\b(?!Nummer\\b)\\w+)\\s*:\\s*(\\w+)\\s*([+|-])\\s*(\\d+)", line) };
            if (!result.empty())
            {
                auto index{ ExistsInCurrentObject(result[1]) };

                auto currentValue{ -1 };
                if (ConstantExists(result[2]) != -1)
                {
                    currentValue = GetVariable(result[2]).value_int();
                }
                if (currentValue != -1)
                {
                    currentValue = CalculateOperation(currentValue, result[3], std::stoi(result[4]));
                }
                else
                {
                    currentValue = 0;
                }
                variableNumbers[result[1]] = currentValue;

                if (index != -1)
                {
                    m_currentObject->mutable_variables()->mutable_variable(index)->set_value_int(currentValue);
                }
                else
                {
                    auto var{ m_currentObject->mutable_variables()->add_variable() };
                    var->set_name(result[1]);
                    var->set_value_int(currentValue);
                }
                continue;
            }
        }
        {
            // example: 'Rotate: 1' or  'Gfx:        GFXGALGEN'
            auto result{ CodHelper::RegexSearch("(\\b(?!Objekt|ObjFill|Nummer\\b)\\w+)\\s*:\\s*(\\w+)", line) };
            if (!result.empty())
            {
                if (m_objectStack.empty())
                {
                    m_unparsedLines.push_back(line);
                    continue;
                }

                auto index{ ExistsInCurrentObject(result[1]) };

                cod_pb::Variable* var{ nullptr };
                if (index != -1)
                {
                    var = m_currentObject->mutable_variables()->mutable_variable(index);
                }
                else
                {
                    var = m_currentObject->mutable_variables()->add_variable();
                    var->set_name(result[1]);
                }

                if (CheckType(result[2]) == CodValueType::INT)
                {
                    if (result[1] == "Id")
                    {
                        m_objectIdMap[std::stoi(result[2])] = m_currentObject;
                    }

                    var->set_value_int(std::stoi(result[2]));
                    variableNumbers[result[1]] = std::stoi(result[2]);
                }
                else
                {
                    if (ConstantExists(result[2]) != -1)
                    {
                        auto v = GetVariable(result[2]);
                        var->set_value_int(v.value_int());
                    }
                    else
                    {
                        var->set_value_string(result[2]);
                    }
                }
                continue;
            }
        }
        {
            // example: Objekt: NAME
            auto result{ CodHelper::RegexSearch("Objekt:\\s*([\\w,]+)", line) };
            if (!result.empty())
            {
                m_currentObject = CreateOrReuseObject(result[1], false, spaces, true);
                m_currentObject->set_name(result[1]);
                m_objectMap[result[1]] = m_currentObject;
                continue;
            }
        }
        {
            // example: @Nummer: +1
            auto result{ CodHelper::RegexSearch("(Nummer):\\s*([+|-]?)(\\d+)", line) };
            if (!result.empty())
            {
                if (TopIsNumberObject())
                {
                    ObjectFinished();
                }

                auto currentNumber{ variableNumbers[result[1]] };
                currentNumber = CalculateOperation(currentNumber, result[2], std::stoi(result[3]));
                variableNumbers[result[1]] = currentNumber;
                auto name{ std::to_string(currentNumber) };
                m_currentObject = CreateObject(true, spaces, true);
                m_currentObject->set_name(name);
                m_objectMap[name] = m_currentObject;
                if ((name == m_objFillRange.stop) || (m_objectStack.size() < m_objFillRange.stacksize))
                {
                    ResetObjfillPrefill();
                }
                continue;
            }
        }
        {
            // example: EndObj
            auto result{ CodHelper::RegexSearch("\\s*EndObj", line) };
            if (!result.empty())
            {
                if (m_objectStack.size() <= m_objFillRange.stacksize)
                {
                    ResetObjfillPrefill();
                }

                if (!m_objectStack.empty())
                {
                    if (m_objectStack.top().spaces > spaces)
                    {
                        // finish previous number object
                        ObjectFinished();
                    }
                    ObjectFinished();
                }
                continue;
            }
        }

        {
            auto result{ CodHelper::RegexSearch("ObjFill:\\s*(\\w+)[,]?\\s*(\\w+)*", line) };
            if (!result.empty())
            {
                // Check if range object fill to insert to objects from start to stop
                // example: ObjFill: 0, MAX
                if (!result[2].empty())
                {
                    m_objFillRange.start = result[1];
                    m_objFillRange.stop = result[2];
                    m_objFillRange.object = *m_objectStack.top().object;
                    m_objFillRange.filling = true;

                    m_objFillRange.stacksize = static_cast<unsigned>(m_objectStack.size());
                    ObjectFinished();
                    m_currentObject = m_objectStack.top().object;
                    auto p{ m_currentObject->mutable_objects()->ReleaseLast() }; // todo p -> nodiscard
                }
                else
                {
                    // example: ObjFill: OBJ
                    auto realName{ GetVariable(result[1]) };
                    auto obj{ GetObject(realName.value_string()) };
                    if (obj)
                    {
                        if (obj.value()->has_variables())
                        {
                            for (auto i{ 0 }; i < obj.value()->variables().variable_size(); ++i)
                            {
                                auto variable{ CreateOrReuseVariable(obj.value()->variables().variable(i).name()) };
                                *variable = obj.value()->variables().variable(i);
                            }
                        }
                        if (obj.value()->objects_size() > 0)
                        {
                            for (auto i{ 0 }; i < obj.value()->objects_size(); ++i)
                            {
                                auto object{ CreateOrReuseObject(obj.value()->objects(i).name(), false, spaces, false) };
                                *object = obj.value()->objects(i);
                            }
                        }
                    }
                }
                continue;
            }
            {
                // example: Nummer: 0
                auto result{ CodHelper::RegexSearch("Nummer:\\s*(\\w+)", line) };
                if (!result.empty())
                {
                    if (TopIsNumberObject())
                    {
                        ObjectFinished();
                    }
                    m_currentObject = CreateObject(true, spaces, true);
                    m_currentObject->set_name(result[1]);
                    m_objectMap[result[1]] = m_currentObject;
                    continue;
                }
            }
        }
    }

    //std::cout << objects.DebugString() << std::endl;
    Json();
}

void mdcii::cod::CodParser::Json() const
{
    std::string jsonString;
    google::protobuf::util::JsonPrintOptions options;
    options.add_whitespace = true;
    options.always_print_primitive_fields = true;
    MessageToJsonString(objects, &jsonString, options);

    // ---------------------
    // todo
    std::ofstream outFile(Game::RESOURCES_PATH + "data/houses.json");
    outFile << jsonString << std::endl;
    outFile.close();
    // ---------------------
}

cod_pb::Object* mdcii::cod::CodParser::CreateObject(const bool t_numberObject, const int t_spaces, const bool t_addToStack)
{
    cod_pb::Object* ret;
    if (m_objectStack.empty())
    {
        ret = objects.add_object();
    }
    else
    {
        ret = m_objectStack.top().object->add_objects();
    }
    ObjectType obj;
    obj.object = ret;
    obj.numberObject = t_numberObject;
    obj.spaces = t_spaces;
    if (t_addToStack)
    {
        m_objectStack.push(obj);
    }
    if (m_objFillRange.filling && t_numberObject)
    {
        ret = ObjfillPrefill(obj.object);
    }

    return ret;
}

cod_pb::Object* mdcii::cod::CodParser::CreateOrReuseObject(const std::string& t_name, const bool t_numberObject, const int t_spaces, const bool t_addToStack)
{
    if (m_currentObject)
    {
        auto optionalObj{ GetSubObject(m_currentObject, t_name) };
        if (optionalObj)
        {
            auto o{ optionalObj.value() };
            if (t_addToStack)
            {
                AddToStack(o, t_numberObject, t_spaces);
            }
            return o;
        }
    }

    return CreateObject(t_numberObject, t_spaces, t_addToStack);
}

std::optional<cod_pb::Object*> mdcii::cod::CodParser::GetObject(const std::string& t_name)
{
    if (m_objectMap.count(t_name))
    {
        return m_objectMap[t_name];
    }

    return {};
}

std::optional<cod_pb::Object*> mdcii::cod::CodParser::GetObject(const int t_id)
{
    if (m_objectIdMap.count(t_id))
    {
        return m_objectIdMap[t_id];
    }

    return {};
}

std::optional<cod_pb::Object*> mdcii::cod::CodParser::GetSubObject(cod_pb::Object* t_obj, const std::string& t_name)
{
    for (auto i{ 0 }; i < t_obj->objects_size(); ++i)
    {
        if (t_obj->objects(i).name() == t_name)
        {
            return t_obj->mutable_objects(i);
        }
    }

    return {};
}

cod_pb::Object* mdcii::cod::CodParser::ObjfillPrefill(cod_pb::Object* t_obj) const
{
    auto name{ t_obj->name() };
    *t_obj = m_objFillRange.object;
    t_obj->set_name(name);

    return t_obj;
}

void mdcii::cod::CodParser::ResetObjfillPrefill()
{
    m_objFillRange.start = "";
    m_objFillRange.stop = "";
    m_objFillRange.filling = false;
    m_objFillRange.stacksize = 0;
}

int mdcii::cod::CodParser::ConstantExists(const std::string& t_key) const
{
    for (auto i{ 0 }; i < m_constants.variable_size(); ++i)
    {
        if (m_constants.variable(i).name() == t_key)
        {
            return i;
        }
    }

    return -1;
}

cod_pb::Variable mdcii::cod::CodParser::GetValue(const std::string& t_key, const std::string& t_value, bool t_isMath, const cod_pb::Variables& t_variables) const
{
    cod_pb::Variable ret;
    ret.set_name(t_key);
    if (t_isMath)
    {
        // Searching for some characters followed by a + or - sign and some digits.
        // example: VALUE+20
        auto result{ CodHelper::RegexSearch("(\\w+)(\\+|-)(\\d+)", t_value) };
        if (!result.empty())
        {
            cod_pb::Variable oldValue;
            auto i{ ConstantExists(result[1]) };
            if (i != -1)
            {
                oldValue = t_variables.variable(i);
            }
            else
            {
                oldValue.set_value_int(0);
            }

            if (oldValue.Value_case() == cod_pb::Variable::ValueCase::kValueString)
            {
                if (oldValue.value_string() == "RUINE_KONTOR_1")
                {
                    // TODO
                    oldValue.set_value_int(424242);
                }
            }
            if (result[2] == "+")
            {
                if (oldValue.Value_case() == cod_pb::Variable::ValueCase::kValueInt)
                {
                    ret.set_value_int(oldValue.value_int() + std::stoi(result[3]));
                    return ret;
                }
                else if (oldValue.Value_case() == cod_pb::Variable::ValueCase::kValueFloat)
                {
                    ret.set_value_int(static_cast<int>(oldValue.value_float() + std::stof(result[3])));
                    return ret;
                }
                else
                {
                    std::string o = oldValue.value_string();
                    ret.set_value_int(std::stoi(o) + std::stoi(result[3]));
                    return ret;
                }
            }

            if (result[2] == "-")
            {
                if (oldValue.Value_case() == cod_pb::Variable::ValueCase::kValueInt)
                {
                    ret.set_value_int(oldValue.value_int() - std::stoi(result[3]));
                    return ret;
                }
                else if (oldValue.Value_case() == cod_pb::Variable::ValueCase::kValueFloat)
                {
                    ret.set_value_int(static_cast<int>(oldValue.value_float() - std::stof(result[3])));
                    return ret;
                }
                else
                {
                    ret.set_value_int(std::stoi(oldValue.value_string()) - std::stoi(result[3]));
                    return ret;
                }
            }
        }
    }

    {
        // Check if value has no preceding characters, a possible + or - sign
        // and one ore more digits -> its an int
        auto result{ CodHelper::RegexMatch("^\\w*[\\-+]?\\d+", t_value) };
        if (!result.empty())
        {
            ret.set_value_int(std::stoi(t_value));
            return ret;
        }
    }

    {
        // Check if value has no preceding characters, a possible + or - sign and one ore more digits
        // followed by a dot and another one or more digits -> its a float
        auto result{ CodHelper::RegexMatch("^\\w+[\\-+]?\\d+\\.\\d+", t_value) };
        if (!result.empty())
        {
            ret.set_value_int(static_cast<int>(std::stof(t_value)));
            return ret;
        }
    }

    {
        // Check if value contains any other characters besides 0-9, + and -
        // -> it is a pure string
        auto result{ CodHelper::RegexMatch("([A-Za-z0-9_]+)", t_value) };
        if (!result.empty())
        {
            // TODO : When is value not in variables
            if (ConstantExists(t_key))
            {
                auto v{ GetVariable(result[1]) };
                ret = v;
                ret.set_name(t_key);
                return ret;
            }
            else
            {
                ret.set_value_string(t_value);
                return ret;
            }
        }
    }

    ret.set_value_int(0);

    return ret;
}

int mdcii::cod::CodParser::ExistsInCurrentObject(const std::string& t_variableName) const
{
    if (m_currentObject)
    {
        // Check if variable already exists in currentObject (e.g. copied from ObjFill)
        for (auto index{ 0 }; index < m_currentObject->variables().variable_size(); ++index)
        {
            if (m_currentObject->variables().variable(index).name() == t_variableName)
            {
                return index;
            }
        }
    }

    return -1;
}

cod_pb::Variable* mdcii::cod::CodParser::CreateOrReuseVariable(const std::string& t_name) const
{
    if (m_currentObject)
    {
        auto optionalVar{ GetVariable(m_currentObject, t_name) };
        if (optionalVar)
        {
            return optionalVar.value();
        }
    }

    return m_currentObject->mutable_variables()->add_variable();
}

cod_pb::Variable mdcii::cod::CodParser::GetVariable(const std::string& t_key) const
{
    for (auto i{ 0 }; i < m_constants.variable_size(); ++i)
    {
        if (m_constants.variable(i).name() == t_key)
        {
            return m_constants.variable(i);
        }
    }
    cod_pb::Variable ret;

    return ret;
}

std::optional<cod_pb::Variable*> mdcii::cod::CodParser::GetVariable(cod_pb::Object* t_obj, const std::string& t_name)
{
    for (auto i{ 0 }; i < t_obj->variables().variable_size(); ++i)
    {
        if (t_obj->variables().variable(i).name() == t_name)
        {
            return t_obj->mutable_variables()->mutable_variable(i);
        }
    }

    return {};
}

int mdcii::cod::CodParser::CalculateOperation(const int t_oldValue, const std::string& t_operation, const int t_op)
{
    auto currentValue{ t_oldValue };
    if (t_operation == "+")
    {
        currentValue += t_op;
    }
    else if (t_operation == "-")
    {
        currentValue -= t_op;
    }
    else
    {
        currentValue = t_op;
    }

    return currentValue;
}

bool mdcii::cod::CodParser::TopIsNumberObject()
{
    if (!m_objectStack.empty())
    {
        return m_objectStack.top().numberObject;
    }

    return false;
}

void mdcii::cod::CodParser::AddToStack(cod_pb::Object* t_o, const bool t_numberObject, const int t_spaces)
{
    ObjectType obj;
    obj.object = t_o;
    obj.numberObject = t_numberObject;
    obj.spaces = t_spaces;
    m_objectStack.push(obj);
}

void mdcii::cod::CodParser::ObjectFinished()
{
    if (!m_objectStack.empty())
    {
        m_objectStack.pop();
    }
}

mdcii::cod::CodParser::CodValueType mdcii::cod::CodParser::CheckType(const std::string& t_s) const
{
    if (std::regex_match(t_s, std::regex("[-|+]?[0-9]+")))
    {
        return CodValueType::INT;
    }
    else if (std::regex_match(t_s, std::regex("[-|+]?[0-9]+.[0-9]+")))
    {
        return CodValueType::FLOAT;
    }
    else
    {
        return CodValueType::STRING;
    }
}
