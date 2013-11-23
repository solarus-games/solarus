/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor;

import org.luaj.vm2.*;
import org.luaj.vm2.lib.jse.*;

/**
 * This class provides utility features related to Lua.
 */
public class LuaTools {

    /**
     * Runs the specified script and saves its output.
     * @param fileName Script to run (without extension).
     * @param arg An argument to pass to the script, or null.
     * @param logFile A file to save the output to, or null.
     * @throws LuaError If the script failed.
     */
    public static void runScript(String fileName, String arg, String logFile)
        throws LuaError {

        LuaValue _G = JsePlatform.standardGlobals();
        _G.get("io").get("output").call(LuaValue.valueOf(logFile));
        LuaValue code = _G.get("loadfile").call(fileName + ".lua");
        if (code.isnil()) {
            throw new LuaError("Failed to load script '" + fileName + ".lua'");
        }
        if (arg != null) {
            code.call(arg);
        }
        else {
            code.call();
        }
    }
}

