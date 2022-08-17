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

#include <gtest/gtest.h>
#include "map/Zoom.h"
#include "MdciiUtils.h"

TEST(TestSuite, TestOperators)
{
    auto zoomAdd{ mdcii::map::Zoom::SGFX };
    ASSERT_EQ(mdcii::map::Zoom::MGFX, ++zoomAdd);
    ASSERT_EQ(mdcii::map::Zoom::GFX, ++zoomAdd);
    ASSERT_EQ(mdcii::map::Zoom::GFX, ++zoomAdd);

    auto zoomMinus{ mdcii::map::Zoom::GFX };
    ASSERT_EQ(mdcii::map::Zoom::MGFX, --zoomMinus);
    ASSERT_EQ(mdcii::map::Zoom::SGFX, --zoomMinus);
    ASSERT_EQ(mdcii::map::Zoom::SGFX, --zoomMinus);
}

TEST(TestSuite, TestStringUtils)
{
    const std::string foo{ "DumMy" };
    ASSERT_EQ("DUMMY", mdcii::to_upper_case(foo));
    ASSERT_EQ("dummy", mdcii::to_lower_case(foo));
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
