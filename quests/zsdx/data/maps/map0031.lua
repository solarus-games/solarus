-------------------------
-- Dungeon 2 B1 script --
-------------------------

switches_puzzle_order = {
  switch_a = 1,
  switch_b = 2,
  switch_c = 3,
  switch_d = 4,
  switch_e = 5,
  switch_f = 6
}

switches_puzzle_nb_enabled = 0
switches_puzzle_correct = true

function event_switch_enabled(switch_name)

  order = switches_puzzle_order[switch_name]
  if order ~= nil then 

    switches_puzzle_nb_enabled = switches_puzzle_nb_enabled + 1
    if switches_puzzle_nb_enabled ~= order then
      switches_puzzle_correct = false
    end

    if switches_puzzle_nb_enabled == 6 then

      if switches_puzzle_correct then
	play_sound("secret")
      else
	play_sound("wrong")
      end
    end
  end
end

