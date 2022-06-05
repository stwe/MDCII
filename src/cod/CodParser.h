#pragma once

#include <stack>
#include <optional>
#include "cod.pb.h"

namespace mdcii::cod
{
    class CodParser
    {
    public:
        CodParser() = delete;

        CodParser(std::string t_codFilePath, bool t_decode);

        CodParser(const CodParser& t_other) = delete;
        CodParser(CodParser&& t_other) noexcept = delete;
        CodParser& operator=(const CodParser& t_other) = delete;
        CodParser& operator=(CodParser&& t_other) noexcept = delete;

        ~CodParser() noexcept = default;

        cod_pb::Objects objects;

    private:
        // Input/Output related functions
        bool ReadFile(bool t_decode);
        void ParseFile();
        void Json() const;

        // Object related functions
        cod_pb::Object* CreateObject(bool t_numberObject, int t_spaces, bool t_addToStack);
        cod_pb::Object* CreateOrReuseObject(const std::string& t_name, bool t_numberObject, int t_spaces, bool t_addToStack);

        std::optional<cod_pb::Object*> GetObject(const std::string& t_name);
        std::optional<cod_pb::Object*> GetObject(int t_id);
        static std::optional<cod_pb::Object*> GetSubObject(cod_pb::Object* t_obj, const std::string& t_name);
        cod_pb::Object* ObjfillPrefill(cod_pb::Object* t_obj) const;
        void ResetObjfillPrefill();

        // Constants related functions
        int ConstantExists(const std::string& t_key) const;
        cod_pb::Variable GetValue(const std::string& t_key, const std::string& t_value, bool t_isMath, const cod_pb::Variables& t_variables) const;

        // Variables related functions
        int ExistsInCurrentObject(const std::string& t_variableName) const;
        cod_pb::Variable* CreateOrReuseVariable(const std::string& t_name) const;
        cod_pb::Variable GetVariable(const std::string& t_key) const;
        static std::optional<cod_pb::Variable*> GetVariable(cod_pb::Object* t_obj, const std::string& t_name);
        static int CalculateOperation(int t_oldValue, const std::string& t_operation, int t_op);

        // Object stack related functions
        bool TopIsNumberObject();
        void AddToStack(cod_pb::Object* t_o, bool t_numberObject, int t_spaces);
        void ObjectFinished();

        struct ObjectType
        {
            cod_pb::Object* object{ nullptr };
            bool numberObject{ false };
            int spaces{ -1 };
        };

        std::stack<ObjectType> m_objectStack;

        struct ObjFillRangeType
        {
            cod_pb::Object object;
            std::string start;
            std::string stop;
            unsigned int stacksize = 0;
            bool filling = false;
        };

        ObjFillRangeType m_objFillRange;

        enum class CodValueType
        {
            INT = 0,
            FLOAT,
            STRING
        };

        CodValueType CheckType(const std::string& t_s) const;

        std::string m_path;
        std::vector<std::string> m_codTxt;

        cod_pb::Variables m_constants;

        std::map<std::string, cod_pb::Object*> m_objectMap;
        std::map<std::string, int> m_variableMap;
        std::map<int, cod_pb::Object*> m_objectIdMap;
        cod_pb::Object* m_currentObject{ nullptr };
        std::vector<std::string> m_unparsedLines;
    };
}
