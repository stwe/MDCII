#pragma once

#include <cstdint>

namespace mdcii::ogl::input
{
    class PickingTexture
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        PickingTexture() = delete;
        PickingTexture(int t_width, int t_height);

        PickingTexture(const PickingTexture& t_other) = delete;
        PickingTexture(PickingTexture&& t_other) noexcept = delete;
        PickingTexture& operator=(const PickingTexture& t_other) = delete;
        PickingTexture& operator=(PickingTexture&& t_other) noexcept = delete;

        ~PickingTexture() noexcept;

        //-------------------------------------------------
        // Write
        //-------------------------------------------------

        void EnableWriting() const;
        static void DisableWriting();

        //-------------------------------------------------
        // Read
        //-------------------------------------------------

        void EnableReading() const;
        static void DisableReading();
        [[nodiscard]] int ReadMapIndex(int t_x, int t_y) const;

    protected:

    private:
        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The width of the window.
         */
        int m_width{ 0 };

        /**
         * The height of the window.
         */
        int m_height{ 0 };

        /**
         * The handle of the Fbo.
         */
        uint32_t m_fboId{ 0 };

        /**
         * The picking texture handle.
         */
        uint32_t m_pickingTextureId{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        void Init();

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void CreateId();

        //-------------------------------------------------
        // Bind / unbind
        //-------------------------------------------------

        void Bind() const;
        static void Unbind();

        //-------------------------------------------------
        // Clean up
        //-------------------------------------------------

        void CleanUp() const;
    };
}
