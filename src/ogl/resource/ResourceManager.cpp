// This file is part of the MDCII project.
//
// Copyright (c) 2022. stwe <https://github.com/stwe/MDCII>
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

#include "ResourceManager.h"

//-------------------------------------------------
// Shaders
//-------------------------------------------------

mdcii::ogl::resource::ShaderProgram& mdcii::ogl::resource::ResourceManager::LoadShaderProgram(const std::string& t_path)
{
    if (shader_programs.count(t_path) == 0)
    {
        shader_programs.emplace(t_path, std::make_unique<ShaderProgram>(t_path));
    }

    return *shader_programs.at(t_path);
}
