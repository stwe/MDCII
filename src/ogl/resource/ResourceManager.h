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
     * The manager prevents the reloading of textures or shaders.
     */
    class ResourceManager
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        inline static std::map<std::string, std::unique_ptr<Texture>> textures;
        inline static std::map<std::string, std::unique_ptr<ShaderProgram>> shaderPrograms;

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
