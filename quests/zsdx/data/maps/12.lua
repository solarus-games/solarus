local map = ...
-- Sahasrahla house

local function has_obtained_world_map()
  return map:get_game():get_value("b33")
end

local function has_seen_frozen_door()
  return map:get_game():get_value("b34")
end

local function has_open_frozen_door()
  return map:get_game():get_value("b35")
end

local function has_obtained_clay_key()
  return map:get_game():get_value("b28")
end

local function has_obtained_bow()
  return map:get_game():get_value("b26")
end

local function give_world_map()
  hero:start_treasure("world_map", 1, "b33")
end

function map:on_started(destination)

  if map:get_game():get_value("b37") then -- if the Lyriann cave is finished
    map:set_doors_open("door", map:get_game():is_dungeon_finished(1)) -- don't allow the player to obtain the bow until the first dungeon is finished
  end

  if map:get_game():is_dungeon_finished(4)
      and not map:get_game():is_dungeon_finished(7) then
    -- Sahasrahla has been kidnapped
    sahasrahla:remove()
  end
end

-- The player talks to Sahasrahla
function sahasrahla:on_interaction()

  if not has_obtained_clay_key() then

    if not has_obtained_world_map() then
      -- first visit
      map:start_dialog("sahasrahla_house.beginning", give_world_map)
      map:set_dialog_variable("sahasrahla_house.beginning", map:get_game():get_player_name())

    elseif has_seen_frozen_door() and not has_open_frozen_door() then
      -- the player has seen the frozen door but was not able to unfreeze it
      map:start_dialog("sahasrahla_house.frozen_door_advice")

    else
      -- the player has not found the clay key yet
      map:start_dialog("sahasrahla_house.quest_accepted", function()
        if not door:is_open() then
          map:open_doors("door")
        end
      end)
    end

  elseif not has_obtained_world_map() then
    -- the player has obtained the clay key: give him the world map now if he didn't talk the first time
    map:start_dialog("sahasrahla_house.give_world_map", give_world_map)
  elseif map:get_game():is_dungeon_finished(1) and not has_obtained_bow() then -- glove
    -- the player should now go downstairs to obtain the bow
    map:start_dialog("sahasrahla_house.dungeon_1_finished")
  else
    -- Sahsrahla has nothing special to say
    map:start_dialog("sahasrahla_house.default")
  end
end

-- Function called when the player has just obtained a treasure
function hero:on_obtained_treasure(item_name, variant, savegame_variable)
  if item_name == "world_map" and variant == 1 then
    map:start_dialog("sahasrahla_house.quest_accepted", function()
      if not door:is_open() then
        map:open_doors("door")
      end
      end)
  end
end

