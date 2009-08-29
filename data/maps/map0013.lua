function event_open_empty_chest(chest_name)
  start_message("chest.not_in_demo")
  play_sound("wrong")
  unfreeze() -- restore the control
end

