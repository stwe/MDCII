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

#include <gtest/gtest.h>
#include "world/Zoom.h"
#include "world/Rotation.h"
#include "physics/Aabb.h"

TEST(TestSuite, TestZoomOperators)
{
    auto zoomAdd{ mdcii::world::Zoom::SGFX };
    ASSERT_EQ(mdcii::world::Zoom::MGFX, ++zoomAdd);
    ASSERT_EQ(mdcii::world::Zoom::GFX, ++zoomAdd);
    ASSERT_EQ(mdcii::world::Zoom::GFX, ++zoomAdd);

    auto zoomMinus{ mdcii::world::Zoom::GFX };
    ASSERT_EQ(mdcii::world::Zoom::MGFX, --zoomMinus);
    ASSERT_EQ(mdcii::world::Zoom::SGFX, --zoomMinus);
    ASSERT_EQ(mdcii::world::Zoom::SGFX, --zoomMinus);
}

TEST(TestSuite, TestStringUtils)
{
    const std::string foo{ "DumMy" };
    ASSERT_EQ("DUMMY", mdcii::to_upper_case(foo));
    ASSERT_EQ("dummy", mdcii::to_lower_case(foo));
}

TEST(TestSuite, TestRotateOperators)
{
    auto rotateAdd{ mdcii::world::Rotation::DEG0 };
    ASSERT_EQ(mdcii::world::Rotation::DEG90, ++rotateAdd);
    ASSERT_EQ(mdcii::world::Rotation::DEG180, ++rotateAdd);
    ASSERT_EQ(mdcii::world::Rotation::DEG270, ++rotateAdd);

    auto rotateMinus{ mdcii::world::Rotation::DEG270 };
    ASSERT_EQ(mdcii::world::Rotation::DEG180, --rotateMinus);
    ASSERT_EQ(mdcii::world::Rotation::DEG90, --rotateMinus);
    ASSERT_EQ(mdcii::world::Rotation::DEG0, --rotateMinus);
}

TEST(TestSuite, TestAabb0)
{
    auto aabb{ mdcii::physics::Aabb(glm::ivec2(0, 0), glm::ivec2(50, 35)) };

    ASSERT_FALSE(mdcii::physics::Aabb::PointVsAabb(glm::ivec2(-1, -1), aabb));
    ASSERT_FALSE(mdcii::physics::Aabb::PointVsAabb(glm::ivec2(50, 35), aabb));

    ASSERT_TRUE(mdcii::physics::Aabb::PointVsAabb(glm::ivec2(0, 0), aabb));
    ASSERT_TRUE(mdcii::physics::Aabb::PointVsAabb(glm::ivec2(1, 1), aabb));
    ASSERT_TRUE(mdcii::physics::Aabb::PointVsAabb(glm::ivec2(49, 34), aabb));
}

TEST(TestSuite, TestAabb1)
{
    auto aabb{ mdcii::physics::Aabb(glm::ivec2(8, 12), glm::ivec2(8, 12)) };

    ASSERT_FALSE(mdcii::physics::Aabb::PointVsAabb(glm::ivec2(7, 12), aabb));
    ASSERT_FALSE(mdcii::physics::Aabb::PointVsAabb(glm::ivec2(8, 11), aabb));

    ASSERT_TRUE(mdcii::physics::Aabb::PointVsAabb(glm::ivec2(15, 23), aabb));
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
