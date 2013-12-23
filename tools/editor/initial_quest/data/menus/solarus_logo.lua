-- Animated Solarus logo by Maxs.

-- You may include this logo in your quest to show that you use Solarus,
-- but this is not mandatory.

-- Example of use:
-- local logo = require("menus/solarus_logo")
-- sol.menu.start(logo)
-- logo.on_finished = function()
--   -- Do whatever you want next (show a title screen, start a game...)
-- end
local solarus_logo_menu = {}

-- Main surface of the menu.
local surface = sol.surface.create(201, 48)

-- Solarus title sprite.
local title = sol.sprite.create("menus/solarus_logo")
title:set_animation("title")

-- Solarus subtitle sprite.
local subtitle = sol.sprite.create("menus/solarus_logo")
subtitle:set_animation("subtitle")

-- Sun sprite.
local sun = sol.sprite.create("menus/solarus_logo")
sun:set_animation("sun")

-- Sword sprite.
local sword = sol.sprite.create("menus/solarus_logo")
sword:set_animation("sword")

-- Black square below the sun.
local black_square = sol.surface.create(48, 15)
black_square:fill_color{0, 0, 0}

-- Step of the animation.
local animation_step = 0

-- Time handling.
local timer = nil

-------------------------------------------------------------------------------

-- Rebuilds the whole surface of the menu.
local function rebuild_surface()

  surface:clear()

  -- Draw the title (after step 1).
  if animation_step >= 1 then
    title:draw(surface)
  end

  -- Draw the sun.
  sun:draw(surface, 0, 33)

  -- Draw the black square to partially hide the sun.
  black_square:draw(surface, 24, 33)

  -- Draw the sword.
  sword:draw(surface, 48, -48)

  -- Draw the subtitle (after step 2).
  if animation_step >= 2 then
    subtitle:draw(surface)
  end
end

-------------------------------------------------------------------------------

-- Starting the menu.
function solarus_logo_menu:on_started()

  -- Initialize or reinitialize the animation.
  animation_step = 0
  timer = nil
  surface:set_opacity(255)
  sun:set_direction(0)
  sun:set_xy(0, 0)
  sword:set_xy(0, 0)
  -- Start the animation.
  solarus_logo_menu:start_animation()
  -- Update the surface.
  rebuild_surface()
end

-- Animation step 1.
function solarus_logo_menu:step1()

  animation_step = 1
  -- Change the sun color.
  sun:set_direction(1)
  -- Stop movements and replace elements.
  sun:stop_movement()
  sun:set_xy(0, -33)
  sword:stop_movement()
  sword:set_xy(-48, 48)
  -- Update the surface.
  rebuild_surface()
end

-- Animation step 2.
function solarus_logo_menu:step2()

  animation_step = 2
  -- Update the surface.
  rebuild_surface()
  -- Start the final timer.
  sol.timer.start(solarus_logo_menu, 500, function()
    surface:fade_out()
    sol.timer.start(solarus_logo_menu, 700, function()
      sol.menu.stop(solarus_logo_menu)
    end)
  end)
end

-- Run the logo animation.
function solarus_logo_menu:start_animation()

  -- Move the sun.
  local sun_movement = sol.movement.create("target")
  sun_movement:set_speed(64)
  sun_movement:set_target(0, -33)
  -- Update the surface whenever the sun moves.
  function sun_movement:on_position_changed()
    rebuild_surface()
  end

  -- Move the sword.
  local sword_movement = sol.movement.create("target")
  sword_movement:set_speed(96)
  sword_movement:set_target(-48, 48)

  -- Update the surface whenever the sword moves.
  function sword_movement:on_position_changed()
    rebuild_surface()
  end

  -- Start the movements.
  sun_movement:start(sun, function()
    sword_movement:start(sword, function()

      if not sol.menu.is_started(solarus_logo_menu) then
        -- The menu may have been stopped, but the movement continued.
        return
      end

      -- If the animation step is not greater than 0
      -- (if no key was pressed).
      if animation_step <= 0 then
        -- Start step 1.
        solarus_logo_menu:step1()
        -- Create the timer for step 2.
        timer = sol.timer.start(solarus_logo_menu, 250, function()
          -- If the animation step is not greater than 1
          -- (if no key was pressed).
          if animation_step <= 1 then
            -- Start step 2.
            solarus_logo_menu:step2()
          end
        end)
      end
    end)
  end)
end

-- Draws this menu on the quest screen.
function solarus_logo_menu:on_draw(screen)

  -- Get the screen size.
  local width, height = screen:get_size()

  -- Center the surface in the screen.
  surface:draw(screen, width / 2 - 100, height / 2 - 24)
end

-- Called when a keyboard key is pressed.
function solarus_logo_menu:on_key_pressed(key)

  if key == "escape" then
    -- Escape: quit Solarus.
    sol.main.exit()
  else
    -- If the timer exists (after step 1).
    if timer ~= nil then
      -- Stop the timer.
      timer:stop()
      timer = nil
      -- If the animation step is not greater than 1
      -- (if the timer has not expired in the meantime).
      if animation_step <= 1 then
        -- Start step 2.
        solarus_logo_menu:step2()
      end

    -- If the animation step is not greater than 0.
    elseif animation_step <= 0 then
      -- Start step 1.
      solarus_logo_menu:step1()
      -- Start step 2.
      solarus_logo_menu:step2()
    end

    -- Return true to indicate that the keyboard event was handled.
    return true
  end
end

-- Return the menu to the caller.
return solarus_logo_menu

