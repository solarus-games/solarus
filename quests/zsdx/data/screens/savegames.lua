-- Savegame selection screen, displayed after the title screen

function on_menu_started()

  -- music
  sol.audio.play_music("game_over")
end

function on_key_pressed(key)

  if key == "escape" then
    -- stop the program
    sol.main.exit()
  end
end

function on_display(dst_surface)

  dst_surface:fill_color({255, 255, 0})
end

