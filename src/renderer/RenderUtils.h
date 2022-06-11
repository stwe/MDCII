#pragma once

#include <glm/ext/matrix_transform.hpp>

//-------------------------------------------------
// RenderUtils
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Static helper methods for rendering.
     */
    class RenderUtils
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        RenderUtils(RenderUtils&& t_other) noexcept = delete;
        RenderUtils(const RenderUtils& t_other) = delete;
        RenderUtils& operator=(const RenderUtils& t_other) = delete;
        RenderUtils& operator=(RenderUtils&& t_other) noexcept = delete;

        //-------------------------------------------------
        // Transform
        //-------------------------------------------------

        /**
         * Calculates a model matrix to transform an isometric position
         * from object space into world space.
         *
         * @param t_isoPosition The isometric coordinates.
         * @param t_size The scale.
         *
         * @return The model matrix.
         */
        static auto GetModelMatrix(const glm::vec2& t_isoPosition, const glm::vec2& t_size) -> glm::mat4
        {
            auto modelMatrix{ glm::mat4(1.0f) };

            modelMatrix = translate(modelMatrix, glm::vec3(t_isoPosition.x, t_isoPosition.y, 0.0f));
            modelMatrix = scale(modelMatrix, glm::vec3(t_size.x, t_size.y, 1.0f));

            return modelMatrix;
        }

    protected:

    private:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        RenderUtils() = default;
        ~RenderUtils() noexcept = default;
    };
}
