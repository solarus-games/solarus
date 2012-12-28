local submenu = require("menus/pause_submenu")
local options_submenu = submenu:new()

function options_submenu:on_started()

  submenu.on_started(self)

  self.captions = {
    "options.caption.press_action_change_mode",
    "options.caption.press_action_customize_key",
    "options.caption.press_key",
  }

  self.video_mode_text = sol.text_surface.create{
    horizontal_alignment = "right",
    vertical_alignment = "top",
    font = "fixed",
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
  local command_names = { "action", "attack", "item_1", "item_2", "pause", "left", "right", "up", "down" }
  for i = 1, #command_names do

    self.command_texts[i] = sol.text_surface.create{
      horizontal_alignment = "left",
      vertical_alignment = "top",
      font = "fixed",
      text_key = "options.command." .. command_names[i],
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
  self:set_cursor_position(1)

  self.game:set_custom_command_effect("action", "change")
end

function options_submenu:load_command_texts()
  -- TODO
end

function options_submenu:set_cursor_position()
  -- TODO
end

function options_submenu:on_draw(dst_surface)

  self:draw_background(dst_surface)
  self:draw_save_dialog_if_any(dst_surface)
end

function options_submenu:on_command_pressed(command)
 
  local handled = submenu.on_command_pressed(self, command)

  if not handled then
    if command == "left" then
      self:previous_submenu()
      handled = true
    elseif command == "right" then
      self:next_submenu()
      handled = true
    end
  end

  return handled
end

return options_submenu

