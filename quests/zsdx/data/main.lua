-- Main script of the quest.

local console = sol.main.load_file("console")()

-- Event called when the program starts.
function sol.main:on_started()

  -- Load built-in settings (audio volume, video mode, etc.).
  sol.main.load_settings()

  -- Just need this here, no need to require globally.
  local language_menu = require("menus/language")

  -- Show the language menu initially.
  sol.main:start_menu(language_menu:new())
end

function sol.main:on_finished()

  sol.main.save_settings()
end

function sol.main:debug_on_key_pressed(key, modifiers)

  local handled = true
  if key == "f1" then
    self:start_savegame("save1.dat")
  elseif key == "f2" then
    self:start_savegame("save2.dat")
  elseif key == "f3" then
    self:start_savegame("save3.dat")
  elseif key == "f12" and not console.enabled then
    console:start()
  elseif sol.main.game ~= nil and not console.enabled then
    local game = sol.main.game
    local hero = nil
    if game ~= nil and game:get_map() ~= nil then
      hero = game:get_map():get_entity("hero")
    end

    -- In-game cheating keys.
    if key == "p" then
      game:add_life(12)
    elseif key == "m" then
      game:remove_life(2)
    elseif key == "o" then
      game:add_money(50)
    elseif key == "l" then
      game:remove_money(15)
    elseif key == "i" then
      game:add_magic(10)
    elseif key == "k" then
      game:remove_magic(4)
    elseif key == "kp 7" then
      game:set_max_magic(0)
    elseif key == "kp 8" then
      game:set_max_magic(42)
    elseif key == "kp 9" then
      game:set_max_magic(84)
    elseif key == "kp 1" then
      local tunic = game:get_item("tunic")
      local variant = math.max(1, tunic:get_variant() - 1)
      tunic:set_variant(variant)
      game:set_ability("tunic", variant)
    elseif key == "kp 4" then
      local tunic = game:get_item("tunic")
      local variant = math.min(3, tunic:get_variant() + 1)
      tunic:set_variant(variant)
      game:set_ability("tunic", variant)
    elseif key == "kp 2" then
      local sword = game:get_item("sword")
      local variant = math.max(1, sword:get_variant() - 1)
      sword:set_variant(variant)
    elseif key == "kp 5" then
      local sword = game:get_item("sword")
      local variant = math.min(4, sword:get_variant() + 1)
      sword:set_variant(variant)
    elseif key == "kp 3" then
      local shield = game:get_item("shield")
      local variant = math.max(1, shield:get_variant() - 1)
      shield:set_variant(variant)
    elseif key == "kp 6" then
      local shield = game:get_item("shield")
      local variant = math.min(3, shield:get_variant() + 1)
      shield:set_variant(variant)
    elseif key == "g" and hero ~= nil then
      local x, y, layer = hero:get_position()
      if layer ~= 0 then
	hero:set_position(x, y, layer - 1)
      end
    elseif key == "t" and hero ~= nil then
      local x, y, layer = hero:get_position()
      if layer ~= 2 then
	hero:set_position(x, y, layer + 1)
      end
    else
      -- Not a known in-game debug key.
      handled = false
    end
  else
    -- Not a known debug key.
    handled = false
  end

  return handled
end

-- Event called when the player pressed a keyboard key.
function sol.main:on_key_pressed(key, modifiers)

  local handled = false

  -- Debugging features.
  if sol.main.is_debug_enabled() then
    handled = sol.main:debug_on_key_pressed(key, modifiers)
  end

  -- Normal features.
  if not handled then

    if key == "f5" then
      -- F5: change the video mode.
      sol.video.switch_mode()
    elseif key == "return" and (modifiers.alt or modifiers.control) then
      -- Alt + Return or Ctrl + Return: switch fullscreen.
      sol.video.set_fullscreen(not sol.video.is_fullscreen())
    elseif key == "f4" and modifiers.alt then
      -- Alt + F4: stop the program.
      sol.main.exit()
    end
  end

  return handled
end

-- Stops the current menu and start another one.
function sol.main:start_menu(menu)

  if sol.main.menu ~= nil then
    sol.menu.stop(sol.main.menu)
  end

  sol.main.menu = menu

  if menu ~= nil then
    sol.menu.start(sol.main, menu)
  end
end

-- Stops the current menu if any and starts a game.
function sol.main:start_savegame(file_name)

  if sol.main.menu ~= nil then
    sol.menu.stop(sol.main.menu)
  end

  sol.main.menu = nil

  local game = sol.game.load(file_name)
  local play_game = sol.main.load_file("play_game")
  play_game(game)
end

