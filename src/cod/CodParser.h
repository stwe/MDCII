// This file is part of the MDCII Game Engine.
//
// Copyright (C) 2019 Armin Schlegel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

#pragma once

#include <stack>
#include <optional>
#include "cod.pb.h"

//-------------------------------------------------
// CodParser
//-------------------------------------------------

namespace mdcii::cod
{
    class CodParser
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        cod_pb::Objects objects;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        CodParser() = delete;

        explicit CodParser(std::string t_codFilePath);

        CodParser(const CodParser& t_other) = delete;
        CodParser(CodParser&& t_other) noexcept = delete;
        CodParser& operator=(const CodParser& t_other) = delete;
        CodParser& operator=(CodParser&& t_other) noexcept = delete;

        ~CodParser() noexcept;

    private:
        //-------------------------------------------------
        // Input/Output
        //-------------------------------------------------

        bool ReadFile(bool t_decode);
        void ParseFile();
        void Json() const;
        void Deserialize();

        //-------------------------------------------------
        // Object related functions
        //-------------------------------------------------

        cod_pb::Object* CreateObject(bool t_numberObject, int t_spaces, bool t_addToStack);
        cod_pb::Object* CreateOrReuseObject(const std::string& t_name, bool t_numberObject, int t_spaces, bool t_addToStack);

        std::optional<cod_pb::Object*> GetObject(const std::string& t_name);
        static std::optional<cod_pb::Object*> GetSubObject(cod_pb::Object* t_obj, const std::string& t_name);
        cod_pb::Object* ObjfillPrefill(cod_pb::Object* t_obj) const;
        void ResetObjfillPrefill();

        //-------------------------------------------------
        // Constants related functions
        //-------------------------------------------------

        int ConstantExists(const std::string& t_key) const;
        cod_pb::Variable GetValue(const std::string& t_key, const std::string& t_value, bool t_isMath, const cod_pb::Variables& t_variables, bool t_relative) const;

        //-------------------------------------------------
        // Variables related functions
        //-------------------------------------------------

        int ExistsInCurrentObject(const std::string& t_variableName) const;
        cod_pb::Variable* CreateOrReuseVariable(const std::string& t_name) const;
        cod_pb::Variable GetVariable(const std::string& t_key) const;
        static std::optional<cod_pb::Variable*> GetVariable(cod_pb::Object* t_obj, const std::string& t_name);
        static int CalculateOperation(int t_oldValue, const std::string& t_operation, int t_op);

        //-------------------------------------------------
        // Object stack related functions
        //-------------------------------------------------

        bool TopIsNumberObject();
        void AddToStack(cod_pb::Object* t_o, bool t_numberObject, int t_spaces);
        void ObjectFinished();

        //-------------------------------------------------
        // Types
        //-------------------------------------------------

        struct ObjectType
        {
            cod_pb::Object* object{ nullptr };
            bool numberObject{ false };
            int spaces{ -1 };
        };

        struct ObjFillRangeType
        {
            cod_pb::Object object;
            std::string start;
            std::string stop;
            unsigned int stacksize{ 0 };
            bool filling{ false };
        };

        enum class CodValueType
        {
            INT = 0,
            FLOAT,
            STRING
        };

        CodValueType CheckType(const std::string& t_s) const;

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::string m_path;
        std::string m_jsonPath;

        std::stack<ObjectType> m_objectStack;
        ObjFillRangeType m_objFillRange;

        std::vector<std::string> m_codTxt;

        cod_pb::Variables m_constants;

        std::map<std::string, cod_pb::Object*> m_objectMap;
        std::map<std::string, int> m_variableMap;
        std::map<int, cod_pb::Object*> m_objectIdMap;
        cod_pb::Object* m_currentObject{ nullptr };
        std::vector<std::string> m_unparsedLines;
    };
}
