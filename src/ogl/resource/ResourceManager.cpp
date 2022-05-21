#include "ResourceManager.h"

//-------------------------------------------------
// Shaders
//-------------------------------------------------

mdcii::ogl::resource::ShaderProgram& mdcii::ogl::resource::ResourceManager::LoadShaderProgram(const std::string& t_path)
{
    if (shaderPrograms.count(t_path) == 0)
    {
        shaderPrograms.emplace(t_path, std::make_unique<ShaderProgram>(t_path));
    }

    return *shaderPrograms.at(t_path);
}
