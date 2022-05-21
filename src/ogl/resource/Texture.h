#pragma once

#include <string>

//-------------------------------------------------
// Texture
//-------------------------------------------------

namespace mdcii::ogl::resource
{
    class Texture
    {
    public:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        uint32_t id{ 0 };
        int width{ 0 };
        int height{ 0 };

        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Texture() = delete;
        Texture(std::string t_path, bool t_loadVerticalFlipped);
        explicit Texture(std::string t_path);

        Texture(const Texture& t_other) = delete;
        Texture(Texture&& t_other) noexcept = delete;
        Texture& operator=(const Texture& t_other) = delete;
        Texture& operator=(Texture&& t_other) noexcept = delete;

        ~Texture() noexcept;

        //-------------------------------------------------
        // Bind / unbind
        //-------------------------------------------------

        void Bind() const;
        static void Unbind();
        void BindForReading(uint32_t t_textureUnit) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        std::string m_path;
        bool m_loadVerticalFlipped{ false };
        int m_format{ 0 };
        int m_channels{ 0 };

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void CreateId();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        void LoadFromFile();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        void CleanUp() const;
    };
}
