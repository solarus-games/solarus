local submenu = require("menus/pause_submenu")
local options_submenu = submenu:new()

function options_submenu:on_started()

  submenu.on_started(self)

  self.video_mode_text = sol.text_surface.create{
    horizontal_alignment = "right",
    vertical_alignment = "top",
    font = "fixed",
    text_key = "options.video_mode." .. sol.video.get_mode(),
  }

  self.command_column_text = sol.text_surface.create{
    horizontal_alignment = "center",
    vertical_alignment = "top",
    font = "fixed",
    text_key = "options.commands_column",
  }

  self.keyboard_column_text = sol.text_surface.create{
    horizontal_alignment = "center",
    vertical_alignment = "top",
    font = "fixed",
    text_key = "options.keyboard_column",
  }

  self.joypad_column_text = sol.text_surface.create{
    horizontal_alignment = "center",
    vertical_alignment = "top",
    font = "fixed",
    text_key = "options.joypad_column",
  }

  self.commands_surface = sol.surface.create(215, 160)
  self.commands_surface:set_transparency_color{0, 0, 0}
  self.commands_highest_visible = 1
  self.commands_visible_y = 0

  self.command_texts = {}
  self.keyboard_texts = {}
  self.joypad_texts = {}
  self.command_names = { "action", "attack", "item_1", "item_2", "pause", "left", "right", "up", "down" }
  for i = 1, #self.command_names do

    self.command_texts[i] = sol.text_surface.create{
      horizontal_alignment = "left",
      vertical_alignment = "top",
      font = "fixed",
      text_key = "options.command." .. self.command_names[i],
    }

    self.keyboard_texts[i] = sol.text_surface.create{
      horizontal_alignment = "left",
      vertical_alignment = "top",
      font = "fixed",
    }

    self.joypad_texts[i] = sol.text_surface.create{
      horizontal_alignment = "left",
      vertical_alignment = "top",
      font = "fixed",
    }
  end

  self:load_command_texts()

  self.up_arrow_sprite = sol.sprite.create("menus/arrow")
  self.up_arrow_sprite:set_direction(1)
  self.down_arrow_sprite = sol.sprite.create("menus/arrow")
  self.down_arrow_sprite:set_direction(3)
  self.cursor_sprite = sol.sprite.create("menus/options_cursor")
  self.cursor_position = nil
  self:set_cursor_position(1)

  self.game:set_custom_command_effect("action", "change")
end

-- Loads the text displayed for each game command, for the
-- keyboard and the joypad.
function options_submenu:load_command_texts()

  self.commands_surface:fill_color{0, 0, 0}
  for i = 1, #self.command_names do
    local keyboard_binding = self.game:get_command_keyboard_binding(self.command_names[i])
    local joypad_binding = self.game:get_command_joypad_binding(self.command_names[i])
    self.keyboard_texts[i]:set_text(keyboard_binding:sub(1, 9))
    self.joypad_texts[i]:set_text(joypad_binding:sub(1, 9))

    local y = 16 * i - 14
    self.command_texts[i]:draw(self.commands_surface, 4, y)
    self.keyboard_texts[i]:draw(self.commands_surface, 74, y)
    self.joypad_texts[i]:draw(self.commands_surface, 143, y)
  end
end

function options_submenu:set_cursor_position(position)

  if position ~= self.cursor_position then

    self.cursor_position = position
    if position == 1 then  -- Video mode.
      self:set_caption("options.caption.press_action_change_mode")
      self.cursor_sprite.x = 102
      self.cursor_sprite.y = 61
      self.cursor_sprite:set_animation("big")
    else  -- Customization of a command.
      self:set_caption("options.caption.press_action_customize_key")

      -- Make sure the selected command is visible.
      while position <= self.commands_highest_visible do
        self.commands_highest_visible = self.commands_highest_visible - 1
        self.commands_visible_y = self.commands_visible_y - 16
      end

      while position > self.commands_highest_visible + 5 do
        self.commands_highest_visible = self.commands_highest_visible + 1
        self.commands_visible_y = self.commands_visible_y + 16
      end

      self.cursor_sprite.x = 55
      self.cursor_sprite.y = 88 + 16 * (position - self.commands_highest_visible)
      self.cursor_sprite:set_animation("small")
    end
  end
end

function options_submenu:on_draw(dst_surface)

  self:draw_background(dst_surface)
  self:draw_caption(dst_surface)

  -- Cursor.
  self.cursor_sprite:draw(dst_surface, self.cursor_sprite.x, self.cursor_sprite.y)

  -- Text.
  self.video_mode_text:draw(dst_surface, 264, 62)
  self.command_column_text:draw(dst_surface, 84, 83)
  self.keyboard_column_text:draw(dst_surface, 153, 83)
  self.joypad_column_text:draw(dst_surface, 229, 83)
  self.commands_surface:draw_region(0, self.commands_visible_y, 215, 84,
      dst_surface, 53, 102)

  -- Arrows.
  if self.commands_visible_y > 0 then
    self.up_arrow_sprite:draw(dst_surface, 96, 96)
    self.up_arrow_sprite:draw(dst_surface, 211, 96)
  end

  if self.commands_visible_y < 60 then
    self.down_arrow_sprite:draw(dst_surface, 96, 182)
    self.down_arrow_sprite:draw(dst_surface, 211, 182)
  end

  self:draw_save_dialog_if_any(dst_surface)
end

function options_submenu:on_command_pressed(command)
 
  if self.command_customizing ~= nil then
    -- We are customizing a command: any key pressed should have been handled before.
    error("options_submenu:on_command_pressed() should not called in this state")
  end

  local handled = submenu.on_command_pressed(self, command)

  if not handled then
    if command == "left" then
      self:previous_submenu()
      handled = true
    elseif command == "right" then
      self:next_submenu()
      handled = true
    elseif command == "up" then
      sol.audio.play_sound("cursor")
      self:set_cursor_position((self.cursor_position + 8) % 10 + 1)
      handled = true
    elseif command == "down" then
      sol.audio.play_sound("cursor")
      self:set_cursor_position(self.cursor_position % 10 + 1)
      handled = true
    elseif command == "action" then
      sol.audio.play_sound("danger")
      if self.cursor_position == 1 then
        -- Change the video mode.
        sol.video.switch_mode()
        self.video_mode_text:set_text_key("options.video_mode." .. sol.video.get_mode())
      else
        -- Customize a game command.
        self:set_caption("options.caption.press_key")
        self.cursor_sprite:set_animation("small_blink")
        local command_to_customize = self.command_names[self.cursor_position - 1]
        self.game:capture_command_binding(command_to_customize, function()
          sol.audio.play_sound("danger")
          self:set_caption("options.caption.press_action_customize_key")
          self.cursor_sprite:set_animation("small")
          self:load_command_texts()
          -- TODO restore HUD icons.
        end)

        -- TODO grey over HUD icons, make the icon of the command blink.
      end
      handled = true
    end
  end

  return handled
end

return options_submenu

