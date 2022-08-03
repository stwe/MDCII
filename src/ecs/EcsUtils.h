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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#pragma once

#include "Game.h"
#include "map/MapLayer.h"

//-------------------------------------------------
// EcsUtils
//-------------------------------------------------

namespace mdcii::ecs
{
    /**
     * Static Ecs helper methods.
     */
    class EcsUtils
    {
    public:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        EcsUtils(EcsUtils&& t_other) noexcept = delete;
        EcsUtils(const EcsUtils& t_other) = delete;
        EcsUtils& operator=(const EcsUtils& t_other) = delete;
        EcsUtils& operator=(EcsUtils&& t_other) noexcept = delete;

        //-------------------------------------------------
        // Utils
        //-------------------------------------------------

        /**
         * Creates a new entity.
         *
         * @return The created EnTT entity.
         */
        static entt::entity CreatePlainEntity();

        /**
         * Counts the number of entities for the given list of types.
         *
         * @tparam Component List of component types.
         * @tparam Exclude List of exclude types.
         *
         * @return The number of entities.
         */
        template<typename... Component, typename... Exclude>
        static int CountEntities(entt::exclude_t<Exclude...> = {})
        {
            auto c{ 0 };
            (
                [&c](entt::view<entt::get_t<Component...>, entt::exclude_t<Exclude...>> t_view)
                {
                    for (auto e : t_view)
                    {
                        c++;
                    }
                }
            )(entt::as_view{ Game::ecs });

            return c;
        }

        /**
         * Counts the number of entities and creates a label for output.
         * 
         * @tparam Component List of component types.
         * @tparam Exclude List of exclude types.
         * @param t_label Used as a label for the entity counter.
         * @param t_exclude List of excludes.
         *
         * @return The created label.
         */
        template<typename... Component, typename... Exclude>
        static std::string EntityCounterLabel(const std::string& t_label, entt::exclude_t<Exclude...> t_exclude = {})
        {
            return t_label + " (" + std::to_string(CountEntities<Component...>(t_exclude)).append(")");
        }

        /**
         * Sort entities by a given component.
         *
         * @tparam Component The component type wich must have a mapTile member.
         *
         * @param t_rotation For this rotation is sorted.
         */
        template<typename Component>
        static void SortEntities(const map::Rotation t_rotation)
        {
            const auto i{ rotation_to_int(t_rotation) };
            Game::ecs.sort<Component>([i](const Component& t_lhs, const Component& t_rhs)
            {
                return t_lhs.mapTile.indices[i] < t_rhs.mapTile.indices[i];
            });
        }

    protected:

    private:
        //-------------------------------------------------
        // Ctors. / Dtor.
        //-------------------------------------------------

        EcsUtils() = default;
        ~EcsUtils() noexcept = default;
    };
}
