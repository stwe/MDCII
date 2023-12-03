// This file is part of the MDCII project.
//
// Copyright (c) 2023. stwe <https://github.com/stwe/MDCII>
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

#include <string>
#include <string_view>

/**
 * @brief This macro is very commonly used in projects that use GNU gettext.
 *        It replaces the string with its translated version at runtime.
 */
#define _(String) gettext(String) // NOLINT(*-reserved-identifier)

/**
 * @brief Holds the gettext domain which is used for differentiating between different groups of messages.
 */
static constexpr std::string_view GETTEXT_DOMAIN{ "mdcii" };

/**
 * @brief The setup_i18n function is used to initialize gettext for internationalization.
 *        It sets up locale, binds text domain and sets the text domain for translation.
 *
 * @param t_locale This is the locale that will be set for translation.
 * @param t_resRelPath This is the relative path to the game resources.
 */
static void setup_i18n(const std::string& t_locale, const std::string& t_resRelPath)
{
    auto localesPath{ std::string(t_resRelPath).append("locales") };

    setlocale(LC_ALL, t_locale.c_str());
    bindtextdomain(GETTEXT_DOMAIN.data(), localesPath.c_str());
    textdomain(GETTEXT_DOMAIN.data());
}
