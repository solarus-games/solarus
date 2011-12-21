local message_id = {
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

-- this function is not used in releases :)
function print_pieces_of_heart()

  local pieces = {
    {savegame_variable =  12, description = "Link's house secret room"},
    {savegame_variable =  17, description = "rupee house: game 3"},
    {savegame_variable = 922, description = "outside world A3: above dungeon 3"},
    {savegame_variable =  18, description = "outside world B3: north of the village, after a hole"},
    {savegame_variable =  21, description = "outside world B4: balcony from the cave under the waterfall"},
    {savegame_variable =  20, description = "outside world B4: behind the lake shop"},
    {savegame_variable = 935, description = "outside world B4: south of the lake "},
    {savegame_variable =  14, description = "outside world A4: near the entrance of dungeon 1 2F"},
    {savegame_variable = 936, description = "twin caves: chest reachable with the cane of somaria"},
    {savegame_variable = 107, description = "outside world A2: island near the castle"},
    {savegame_variable = 920, description = "outside world B1: bottom of the vine"},
    {savegame_variable =  27, description = "Sahsarahla cave: chest reachable with the golden glove"},
    {savegame_variable =  39, description = "Lyriann cave B1: walk on the invisible sensors"},
    {savegame_variable = 101, description = "fall in the hole near a flower in outside world A3"},
    {savegame_variable = 105, description = "true hero maze"},
    {savegame_variable = 191, description = "fairy cave 2F"},
    {savegame_variable = 900, description = "junk shop"},
    {savegame_variable = 917, description = "west mountains cave 1F: rechable with the cane of somaria"},
    {savegame_variable = 918, description = "west mountains cave 3F: come from the waterfall using the mysic mirror"},
    {savegame_variable = 181, description = "chests game"},
    {savegame_variable = 197, description = "waterfall cave 1F: open all chests"},
    {savegame_variable = 937, description = "laser cave"},
    {savegame_variable = 924, description = "dungeon 1 1F: weak wall"},
    {savegame_variable =  71, description = "dungeon 2 1F: under a platform"},
    {savegame_variable = 132, description = "dungeon 3 3F: open a barrier in the maze"},
    {savegame_variable = 413, description = "dungeon 4 1F: under a platform"},
    {savegame_variable = 506, description = "dungeon 5 2F: in the teletransporters maze"},
    {savegame_variable = 309, description = "dungeon 6 4F: under a platform"},
    {savegame_variable = 601, description = "dungeon 7 1F: in a pipe"},
    {savegame_variable = 724, description = "dungeon 8 1F: weak wall"},
    {savegame_variable =  22, description = "dungeon 9 2F: get out to outside world A4"},
    {savegame_variable = 241, description = "secret dungeon 4F: use the hookshot"},
  }

  local nb_found = 0
  for i, v in ipairs(pieces) do
    if sol.game.savegame_get_boolean(v.savegame_variable) then
      nb_found = nb_found + 1
    else
      print("You don't have piece of heart #" .. i .. ": " .. v.description)
    end
  end
  print("Total pieces of heart found: " .. nb_found .. "/" .. #pieces)
end

-- If you want to know your missing pieces of heart, uncomment the line below
-- print_pieces_of_heart()

