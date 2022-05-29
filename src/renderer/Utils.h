#pragma once

#include <glm/ext/matrix_transform.hpp>

//-------------------------------------------------
// Utils
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Represents the map rotation.
     */
    enum class Rotation
    {
        DEG0, DEG90, DEG180, DEG270
    };

    /**
     * Static helper methods.
     */
    class Utils
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Utils(Utils&& t_other) noexcept = delete;
        Utils(const Utils& t_other) = delete;
        Utils& operator=(const Utils& t_other) = delete;
        Utils& operator=(Utils&& t_other) noexcept = delete;

        //-------------------------------------------------
        // Tiles
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

        /**
         * Projects map coordinates in isometric view.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         *
         * @return The isometric coordinates.
         */
        static auto MapToIso(const int t_mapX, const int t_mapY) -> glm::vec2
        {
            return {
                (t_mapX - t_mapY) << 5, // 32
                (t_mapX + t_mapY) << 4  // 16
            };
        }

        /**
         * Projects map coordinates in isometric view.
         *
         * @param t_mapX The map x position.
         * @param t_mapY The map y position.
         * @param t_rotation The current map rotation.
         *
         * @return The isometric coordinates.
         */
        static auto MapToIso(const int t_mapX, const int t_mapY, const Rotation t_rotation)
        {
            auto xr{ t_mapX };
            auto yr{ t_mapY };

            if (t_rotation == Rotation::DEG90)
            {
                xr = 4 - t_mapY - 1;
                yr = t_mapX;
            }

            if (t_rotation == Rotation::DEG180)
            {
                xr = 4 - t_mapX - 1;
                yr = 8 - t_mapY - 1;
            }

            if (t_rotation == Rotation::DEG270)
            {
                xr = t_mapY;
                yr = 8 - t_mapX - 1;
            }

            return MapToIso(xr, yr);
        }

    protected:

    private:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        Utils() = default;
        ~Utils() noexcept = default;
    };
}
