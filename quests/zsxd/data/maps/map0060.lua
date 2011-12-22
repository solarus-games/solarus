-- Temple of Stupidities 1F NW

local switches_good_order = {
  3,1,4,2,3,2,1,1,4,3,2,2,
  1,3,2,3,3,1,4,4,2,1,2,3,
  3,2,1,1,3,4,2,2,1,3,4,1
}
local next_switch_index = 1

function event_switch_activated(switch_name)

  local switch_index = string.match(switch_name, "^switch_([1-4])$")
  if switch_index ~= nil and next_switch_index <= #switches_good_order then
 
    switch_index = tonumber(switch_index)
    if switch_index == switches_good_order[next_switch_index] then
      next_switch_index = next_switch_index + 1
    else
      next_switch_index = 1
    end

    if next_switch_index == 5 and not sol.map.door_is_open("code_door") then
      sol.map.dialog_start("dungeon_1.big_code_ok")
    elseif next_switch_index > #switches_good_order then
      sol.map.dialog_start("dungeon_1.big_code_completed")
    end

    sol.map.switch_set_activated(switch_name, false)
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "dungeon_1.big_code_ok" then
    sol.map.camera_move(1072, 456, 250, function()
      sol.map.door_open("code_door")
      sol.main.play_sound("secret")
    end)
  end
end

