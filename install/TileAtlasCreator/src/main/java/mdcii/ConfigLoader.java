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

package mdcii;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.lang.reflect.Modifier;
import java.util.Objects;
import java.util.Properties;

/**
 * Loads a config file.
 */
public class ConfigLoader {

    //-------------------------------------------------
    // Load
    //-------------------------------------------------

    /**
     * Loads a config file.
     *
     * @param configClass The config class.
     * @param path The path to the config file.
     * @throws IOException If an I/O error is thrown.
     * @throws IllegalAccessException If an access error is thrown.
     */
    public static void load(Class<?> configClass, String path) throws IOException, IllegalAccessException {
        Objects.requireNonNull(configClass, "configClass must not be null");
        Objects.requireNonNull(path, "path must not be null");

        var in = ConfigLoader.class.getResourceAsStream(path);
        if (in == null) {
            throw new FileNotFoundException("Config file " + path + " not found.");
        }

        var properties = new Properties();
        properties.load(in);

        for (var field : configClass.getDeclaredFields()) {
            if (Modifier.isStatic(field.getModifiers())) {
                field.set(null, getValue(properties, field.getName(), field.getType()));
            }
        }
    }

    //-------------------------------------------------
    // Helper
    //-------------------------------------------------

    /**
     * Get configuration value.
     *
     * @param properties A {@link Properties} object.
     * @param name The name of the config value.
     * @param type The type of the config value.
     *
     * @return {@link Object}
     */
    private static Object getValue(Properties properties, String name, Class<?> type) {
        var value = Objects.requireNonNull(properties).getProperty(Objects.requireNonNull(name));

        if (value == null) {
            throw new RuntimeException("Missing configuration value: " + name);
        }

        if (type == String.class) {
            return value;
        }

        if (type == boolean.class) {
            return Boolean.parseBoolean(value);
        }

        if (type == int.class) {
            return Integer.parseInt(value);
        }

        if (type == float.class) {
            return Float.parseFloat(value);
        }

        if (type == double.class) {
            return Double.parseDouble(value);
        }

        throw new RuntimeException("Unknown configuration value type: " + type.getName());
    }
}
