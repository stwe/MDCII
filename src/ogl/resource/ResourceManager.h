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

#pragma once

#include <map>
#include <memory>
#include <tuple>
#include "Texture.h"
#include "ShaderProgram.h"

//-------------------------------------------------
// ResourceManager
//-------------------------------------------------

namespace mdcii::ogl::resource
{
    /**
     * The static manager prevents the reloading of textures or shaders.
     */
    class ResourceManager
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        inline static std::map<std::string, std::unique_ptr<Texture>> textures;
        inline static std::map<std::string, std::unique_ptr<ShaderProgram>> shader_programs;

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        ResourceManager(const ResourceManager& t_other) = delete;
        ResourceManager(ResourceManager&& t_other) noexcept = delete;
        ResourceManager& operator=(const ResourceManager& t_other) = delete;
        ResourceManager& operator=(ResourceManager&& t_other) noexcept = delete;

        //-------------------------------------------------
        // Textures
        //-------------------------------------------------

        template<typename... Args>
        static const Texture& LoadTexture(Args&&... t_args)
        {
            auto t{ std::make_tuple(t_args...) };
            if (textures.count(std::get<0>(t)) == 0)
            {
                textures.emplace(std::get<0>(t), std::make_unique<Texture>(std::forward<Args>(t_args)...));
            }

            return *textures.at(std::get<0>(t));
        }

        //-------------------------------------------------
        // Shaders
        //-------------------------------------------------

        static ShaderProgram& LoadShaderProgram(const std::string& t_path);

    protected:

    private:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        ResourceManager() = default;
        ~ResourceManager() noexcept = default;
    };
}
