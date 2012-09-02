-- Main script of the quest.

local console = sol.main.load_file("console")()
sol.main.console = console

-- Event called when the program starts.
function sol.main:on_started()

  -- Just need this here, no need to require globally.
  local language_menu = require("screens/language")

  -- Show the language menu initially.
  sol.main:start_menu(language_menu:new())
end

function sol.main:debug_on_key_pressed(key, modifiers)

  local handled = false
  if console.enabled then
    handled = console:on_key_pressed(key, modifiers)
  end

  if not handled then
    handled = true
    if key == "f1" then
      self:start_savegame("save1.dat")
    elseif key == "f2" then
      self:start_savegame("save2.dat")
    elseif key == "f3" then
      self:start_savegame("save3.dat")
    elseif key == "f12" then
      console:start()
    elseif sol.main.game ~= nil then
      local game = sol.main.game
      local hero
      if game ~= nil then
	hero = game:get_map():get_entity("hero")
      end

      -- In-game cheat keys.
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
	game:set_max_magic(42)
      elseif key == "kp 1" then
	game:set_ability("tunic", math.max(1, game:get_ability("tunic") - 1))
      elseif key == "kp 3" then
	game:set_ability("tunic", math.min(3, game:get_ability("tunic") + 1))
      elseif key == "kp 2" then
	game:set_ability("sword", math.max(0, game:get_ability("sword") - 1))
      elseif key == "kp 5" then
	game:set_ability("sword", math.min(4, game:get_ability("sword") + 1))
      elseif key == "kp 3" then
	game:set_ability("shield", math.max(0, game:get_ability("shield") - 1))
      elseif key == "kp 6" then
	game:set_ability("shield", math.min(3, game:get_ability("shield") + 1))
      elseif key == "g" then
	local x, y, layer = hero:get_position()
	if layer ~= 0 then
	  hero:set_position(x, y, layer - 1)
	end
      elseif key == "t" then
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

function sol.main:on_character_pressed(character)

  local handled = false
  if console.enabled then
    handled = console:on_character_pressed(character)
  end
  return handled
end

function sol.main:on_post_draw(dst_surface)

  if console.enabled then
    console:on_draw(dst_surface)
  end
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

