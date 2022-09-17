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

#include <array>
#include <memory>
#include "ogl/Window.h"
#include "camera/Camera.h"
#include "map/Zoom.h"

//-------------------------------------------------
// Forward declarations
//-------------------------------------------------

namespace mdcii::map
{
    class Map;
    enum class LayerType;
    enum class Rotation;
}

namespace mdcii::ogl::buffer
{
    class Vao;
}

//-------------------------------------------------
// TerrainRenderer
//-------------------------------------------------

namespace mdcii::renderer
{
    /**
     * Renders the map with instancing.
     */
    class TerrainRenderer
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        /**
         * Constructs a new TerrainRenderer object.
         *
         * @param t_map The parent Map object.
         */
        explicit TerrainRenderer(map::Map* t_map);

        TerrainRenderer(const TerrainRenderer& t_other) = delete;
        TerrainRenderer(TerrainRenderer&& t_other) noexcept = delete;
        TerrainRenderer& operator=(const TerrainRenderer& t_other) = delete;
        TerrainRenderer& operator=(TerrainRenderer&& t_other) noexcept = delete;

        ~TerrainRenderer() noexcept;

        //-------------------------------------------------
        // Render
        //-------------------------------------------------

        /**
         * Renders a layer with the specified zoom and rotation.
         *
         * @param t_layerType The layer type to render.
         * @param t_zoom The zoom to render for.
         * @param t_rotation The rotation to render for.
         * @param t_window The Window object to get the orthographic projection matrix.
         * @param t_camera The Camera object to get the view matrix.
         */
        void Render(
            map::LayerType t_layerType,
            map::Zoom t_zoom,
            map::Rotation t_rotation,
            const ogl::Window& t_window,
            const camera::Camera& t_camera
        ) const;

    protected:

    private:
        //-------------------------------------------------
        // Constants
        //-------------------------------------------------

        /**
         * The number of the model matrices0 shader attribute.
         */
        static constexpr auto MODEL_MATRICES0_LOCATION{ 1 };

        /**
         * The number of the texture atlas shader attribute.
         */
        static constexpr auto TEXTURE_ATLAS_LOCATION{ 17 };

        /**
         * The number of the offsets0 shader attribute.
         */
        static constexpr auto OFFSETS0_LOCATION{ 18 };

        /**
         * The number of the heights shader attribute.
         */
        static constexpr auto HEIGHTS_LOCATION{ 22 };

        //-------------------------------------------------
        // Member
        //-------------------------------------------------

        /**
         * The parent Map object.
         */
        map::Map* m_map{ nullptr };

        using Vaos = std::array<std::unique_ptr<ogl::buffer::Vao>, map::NR_OF_ZOOMS>;
        using Layer_Vaos = std::array<Vaos, 2>;

        // todo: einen weiteren Vao als mixed hinzufügen

        Layer_Vaos m_vaos{};

        /**
         * Number of instances to render.
         */
        int32_t m_instances{ 0 };

        //-------------------------------------------------
        // Init
        //-------------------------------------------------

        /**
         * Initializes the class.
         */
        void Init();

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        /**
         * Add model matrices to the Gpu.
         *
         * @param t_zoom The zoom level.
         * @param t_layerType The type of the layer (TERRAIN, BUILDINGS).
         */
        void AddModelMatrices(map::Zoom t_zoom, map::LayerType t_layerType);

        /**
         * Add texture info to the Gpu.
         * 
         * @param t_zoom The zoom level.
         * @param t_layerType The type of the layer (TERRAIN, BUILDINGS).
         */
        void AddTextureInfo(map::Zoom t_zoom, map::LayerType t_layerType) const;
    };
}
