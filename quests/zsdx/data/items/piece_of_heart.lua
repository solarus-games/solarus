message_id = {
  "found_piece_of_heart.first",
  "found_piece_of_heart.second",
  "found_piece_of_heart.third",
  "found_piece_of_heart.fourth"
}

function event_obtained(variant)

  local nb_pieces_of_heart = sol.game.savegame_get_integer(1030)
  sol.map.dialog_start(message_id[nb_pieces_of_heart + 1])
end

function event_dialog_finished(first_message_id, answer)

  local nb_pieces_of_heart = sol.game.savegame_get_integer(1030)

  if first_message_id == message_id[nb_pieces_of_heart + 1] then

    sol.game.savegame_set_integer(1030, (nb_pieces_of_heart + 1) % 4)
    if (nb_pieces_of_heart == 3) then
      sol.game.add_max_life(4)
    end
    sol.game.add_life(sol.game.get_max_life())
  end
end

