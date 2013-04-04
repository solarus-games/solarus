local item = ...

local message_id = {
  "found_piece_of_heart.first",
  "found_piece_of_heart.second",
  "found_piece_of_heart.third",
  "found_piece_of_heart.fourth"
}

function item:on_obtained(variant)

  local game = self:get_game()
  local nb_pieces_of_heart = game:get_value("i1030") or 0
  self:get_map():start_dialog(message_id[nb_pieces_of_heart + 1], function()

    game:set_value("i1030", (nb_pieces_of_heart + 1) % 4)
    if nb_pieces_of_heart == 3 then
      game:add_max_life(4)
    end
    game:add_life(game:get_max_life())
  end)
end

-- This function is not used in releases :)
function item:print_pieces_of_heart()

  local pieces = {
    {savegame_variable =  "b12", description = "Link's house secret room"},
    {savegame_variable =  "b17", description = "rupee house: game 3"},
    {savegame_variable = "b922", description = "outside world A3: above dungeon 3"},
    {savegame_variable =  "b18", description = "outside world B3: north of the village, after a hole"},
    {savegame_variable =  "b21", description = "outside world B4: balcony from the cave under the waterfall"},
    {savegame_variable =  "b20", description = "outside world B4: behind the lake shop"},
    {savegame_variable = "b935", description = "outside world B4: south of the lake "},
    {savegame_variable =  "b14", description = "outside world A4: near the entrance of dungeon 1 2F"},
    {savegame_variable = "b936", description = "twin caves: chest reachable with the cane of somaria"},
    {savegame_variable = "b107", description = "outside world A2: island near the castle"},
    {savegame_variable = "b920", description = "outside world B1: bottom of the vine"},
    {savegame_variable =  "b27", description = "Sahsarahla cave: chest reachable with the golden glove"},
    {savegame_variable =  "b39", description = "Lyriann cave B1: walk on the invisible sensors"},
    {savegame_variable = "b101", description = "fall in the hole near a flower in outside world A3"},
    {savegame_variable = "b105", description = "true hero maze"},
    {savegame_variable = "b191", description = "fairy cave 2F"},
    {savegame_variable = "b900", description = "junk shop"},
    {savegame_variable = "b917", description = "west mountains cave 1F: rechable with the cane of somaria"},
    {savegame_variable = "b918", description = "west mountains cave 3F: come from the waterfall using the mysic mirror"},
    {savegame_variable = "b181", description = "chests game"},
    {savegame_variable = "b197", description = "waterfall cave 1F: open all chests"},
    {savegame_variable = "b937", description = "laser cave"},
    {savegame_variable = "b924", description = "dungeon 1 1F: weak wall"},
    {savegame_variable =  "b71", description = "dungeon 2 1F: under a platform"},
    {savegame_variable = "b132", description = "dungeon 3 3F: open a barrier in the maze"},
    {savegame_variable = "b413", description = "dungeon 4 1F: under a platform"},
    {savegame_variable = "b506", description = "dungeon 5 2F: in the teletransporters maze"},
    {savegame_variable = "b309", description = "dungeon 6 4F: under a platform"},
    {savegame_variable = "b601", description = "dungeon 7 1F: in a pipe"},
    {savegame_variable = "b724", description = "dungeon 8 1F: weak wall"},
    {savegame_variable =  "b22", description = "dungeon 9 2F: get out to outside world A4"},
    {savegame_variable = "b241", description = "secret dungeon 4F: use the hookshot"},
  }

  local nb_found = 0
  for i, v in ipairs(pieces) do
    if self:get_game():get_value(v.savegame_variable) then
      nb_found = nb_found + 1
    else
      print("You don't have piece of heart #" .. i .. ": " .. v.description)
    end
  end
  print("Total pieces of heart found: " .. nb_found .. "/" .. #pieces)
end

-- If you want to know your missing pieces of heart, uncomment the line below
-- item:print_pieces_of_heart()

