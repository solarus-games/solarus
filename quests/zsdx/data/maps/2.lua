local map = ...

-- Rupee house.

-- Initializations made when the map has just been loaded.
local playing_game_1 = false
local playing_game_2 = false
local playing_game_3 = false
local already_played_game_1 = false
local game_1_rewards = {5, 20, 50}  -- Possible rupee rewards in game 1.
local game_2_bet = 0
local game_2_reward = 0
local game_2_man_sprite = nil
local game_2_timer
local game_2_slots = {
  [slot_machine_left] =   {initial_frame = 6, initial_delay = 70, current_delay = 0, symbol = -1},
  [slot_machine_middle] = {initial_frame = 15, initial_delay = 90, current_delay = 0, symbol = -1},
  [slot_machine_right] =  {initial_frame = 9, initial_delay = 60, current_delay = 0, symbol = -1}
}  -- The key is also the entity.
local game_3_timer
local game_1_question_dialog_finished
local game_2_question_dialog_finished
local game_2_choose_bet_dialog_finished
local game_3_question_dialog_finished
local open_game_1_chest
local game_2_timeout

-- Function called when the map starts.
function map:on_started(destination)

  chest_1.on_empty = open_game_1_chest
  chest_2.on_empty = open_game_1_chest
  chest_3.on_empty = open_game_1_chest
  for npc, v in pairs(game_2_slots) do
    v.sprite = npc:get_sprite()
    v.sprite:set_frame(v.initial_frame)
    function npc:on_interaction()
      map:activate_slot_machine(self)
    end
  end
  game_2_man_sprite = game_2_man:get_sprite()
end

-- Function called when the player wants to talk to this non-playing character.
function game_1_man:on_interaction()
  -- game 1 dialog

  if playing_game_1 then
    -- the player is already playing: tell him to choose a chest
    map:start_dialog("rupee_house.game_1.choose_chest")
  else

    -- see if the player can still play
    local unauthorized = map:get_game():get_value("b16")

    if unauthorized then
      -- the player already won much money
      map:start_dialog("rupee_house.game_1.not_allowed_to_play")
    else
      if not already_played_game_1 then
        -- first time: long dialog with the game rules
        map:start_dialog("rupee_house.game_1.intro", game_1_question_dialog_finished)
      else
        -- quick dialog to play again
        map:start_dialog("rupee_house.game_1.play_again_question", game_1_question_dialog_finished)
      end
    end
  end
end

function game_2_man:on_interaction()

  -- game 2 dialog

  if playing_game_2 then
    -- the player is already playing: tell him to stop the reels
    map:start_dialog("rupee_house.game_2.playing")
  else
    -- dialog with the game rules
    map:start_dialog("rupee_house.game_2.intro", game_2_question_dialog_finished)
  end
end

function game_3_man:on_interaction()
  -- game 3 dialog

  if playing_game_3 then
    -- the player is already playing: let him restart the game
    map:start_dialog("rupee_house.game_3.restart_question", game_3_question_dialog_finished)
  else
    -- see if the player can still play
    local unauthorized = map:get_game():get_value("b17")

    if unauthorized then
      -- the player already won this game
      map:start_dialog("rupee_house.game_3.not_allowed_to_play")
    else
      -- game rules
      map:start_dialog("rupee_house.game_3.intro", game_3_question_dialog_finished)
    end
  end
end

function map:activate_slot_machine(npc)

  if playing_game_2 then

    game_2_man_sprite:set_direction(0)
    local slot = game_2_slots[npc]
    if slot.symbol == -1 then
      -- stop this reel

      local sprite = slot.sprite
      local current_symbol = math.floor(sprite:get_frame() / 3)
      slot.symbol = (current_symbol + math.random(2)) % 7
      slot.current_delay = slot.current_delay + 100
      sprite:set_frame_delay(slot.current_delay)

      -- test code to win every game:
      -- for _, v in pairs(game_2_slots) do
      --    v.symbol = slot.symbol
      --    v.current_delay = slot.current_delay + 100
      --    v.sprite:set_frame_delay(v.current_delay)
      -- end

      sol.audio.play_sound("switch")
      hero:freeze()
    end
  else
    sol.audio.play_sound("wrong")
    map:start_dialog("rupee_house.pay_first")
  end
end

function game_1_question_dialog_finished(answer)

  if answer == 1 then
    -- the player does not want to play the game
    map:start_dialog("rupee_house.game_1.not_playing")
  else
    -- wants to play game 1

    if map:get_game():get_money() < 20 then
      -- not enough money
      sol.audio.play_sound("wrong")
      map:start_dialog("rupee_house.not_enough_money")

    else
      -- enough money: reset the 3 chests, pay and start the game
      chest_1:set_open(false)
      chest_2:set_open(false)
      chest_3:set_open(false)

      map:get_game():remove_money(20)
      map:start_dialog("rupee_house.game_1.good_luck")
      playing_game_1 = true
    end
  end
end

function game_2_question_dialog_finished(answer)

  if answer == 1 then
    -- don't want to play the game
    map:start_dialog("rupee_house.game_2.not_playing")
  else
    -- wants to play game 2
    map:start_dialog("rupee_house.game_2.choose_bet", game_2_choose_bet_dialog_finished)
  end
end

function game_2_choose_bet_dialog_finished(answer)

  if answer == 0 then
    -- bet 5 rupees
    game_2_bet = 5
  else
    -- bet 20 rupees
    game_2_bet = 20
  end

  if map:get_game():get_money() < game_2_bet then
    -- not enough money
    sol.audio.play_sound("wrong")
    map:start_dialog("rupee_house.not_enough_money")
  else
    -- enough money: pay and start the game
    map:get_game():remove_money(game_2_bet)
    map:start_dialog("rupee_house.game_2.just_paid")
    playing_game_2 = true

    -- start the slot machine animations
    for k, v in pairs(game_2_slots) do
      v.symbol = -1
      v.current_delay = v.initial_delay
      v.sprite:set_animation("started")
      v.sprite:set_frame_delay(v.current_delay)
      v.sprite:set_frame(v.initial_frame)
      v.sprite:set_paused(false)
    end
  end
end

-- Called after the game 3 dialog "do you want to play?" or "do you want to try again?".
function game_3_question_dialog_finished(answer)

  if answer == 1 then
    -- don't want to play the game
    map:start_dialog("rupee_house.game_3.not_playing")
  else
    -- wants to play game 3

    if map:get_game():get_money() < 10 then
      -- not enough money
      sol.audio.play_sound("wrong")
      map:start_dialog("rupee_house.not_enough_money")

    else
      -- enough money: reset the game, pay and start the game
      reset_blocks()
      game_3_barrier_1:set_enabled(false)
      game_3_barrier_2:set_enabled(false)
      game_3_barrier_3:set_enabled(false)
      game_3_middle_barrier:set_enabled(false)
      if game_3_timer ~= nil then
        game_3_timer:stop()
        game_3_timer = nil
      end

      map:get_game():remove_money(10)
      map:start_dialog("rupee_house.game_3.go", function()
        game_3_timer = sol.timer.start(8000, function()
          sol.audio.play_sound("door_closed")
          game_3_middle_barrier:set_enabled(true)
        end)
        game_3_timer:set_with_sound(true)
        game_3_sensor:set_enabled(true)
      end)
      playing_game_3 = true
    end
  end
end

function map:on_obtained_treasure(item, variant, savegame_variable)
  -- stop game 3 when the player finds the piece of heart
  if item:get_name() == "piece_of_heart" then
    game_3_final_barrier:set_enabled(false)
    sol.audio.play_sound("secret")
    playing_game_3 = false
  end
end

-- Function called when the player opens an empty chest (i.e. a chest
-- whose feature is to call the script) of game 1.
function open_game_1_chest(chest)

  if not playing_game_1 then
    -- trying to open a chest but not playing yet
    map:start_dialog("rupee_house.pay_first") -- the game man is angry
    chest:set_open(false) -- close the chest again
    sol.audio.play_sound("wrong")
    hero:unfreeze() -- restore the control
  else
    -- give a random reward
    local index = math.random(#game_1_rewards)
    local amount = game_1_rewards[index]
    if amount == 50 and not already_played_game_1 then
      -- don't give 50 rupees at the first attempt
      amount = 5
    end

    -- give the rupees
    if amount == 5 then
      hero:start_treasure("rupee", 2)
    elseif amount == 20 then
      hero:start_treasure("rupee", 3)
    elseif amount == 50 then
      hero:start_treasure("rupee", 4)
    end

    if amount == 50 then
      -- the maximum reward was found: the game will now refuse to let the hero play again
      map:get_game():set_value("b16", true)
    end

    playing_game_1 = false
    already_played_game_1 = true
  end
end

-- Function called when a sensor is activated
function game_3_sensor:on_activated()

  -- stop the timer when the player reaches this point
  if game_3_timer ~= nil then
    game_3_timer:stop()
    game_3_timer = nil
  end
  sol.audio.play_sound("secret")
  self:set_enabled(false)
end

-- Updates the slot machine
function map:on_update()

  if playing_game_2 then

    -- stop the reels when necessary
    local nb_finished = 0
    for k, v in pairs(game_2_slots) do
      if v.sprite:is_paused() then
	nb_finished = nb_finished + 1
      end
    end

    for k, v in pairs(game_2_slots) do
      local frame = v.sprite:get_frame()

      if not v.sprite:is_paused() and frame == v.symbol * 3 then
	v.sprite:set_paused(true)
	v.initial_frame = frame
	nb_finished = nb_finished + 1

	if nb_finished < 3 then
	  hero:unfreeze()
	else
	  playing_game_2 = false
	  game_2_timer = sol.timer.start(500, game_2_timeout)
	end
      end
    end
  end
end

-- This function gives the reward to the player in the slot machine game
function game_2_timeout()

  -- see if the player has won
  local i = 1
  local green_found = false
  local blue_found = false
  local red_found = false
  local symbols = {-1, -1, -1}
  for k, v in pairs(game_2_slots) do
    symbols[i] = v.symbol

    if symbols[i] == 0 then
      green_found = true
    elseif symbols[i] == 2 then
      blue_found = true
    elseif symbols[i] == 4 then
      red_found = true
    end

    i = i + 1
  end

  local function game_2_give_reward()
    map:get_game():add_money(game_2_reward)
  end

  if symbols[1] == symbols[2] and symbols[2] == symbols[3] then
    -- three identical symbols

    if symbols[1] == 0 then -- 3 green rupees
      map:start_dialog("rupee_house.game_2.reward.green_rupees", game_2_give_reward)
      game_2_reward = 5 * game_2_bet
    elseif symbols[1] == 2 then -- 3 blue rupees
      map:start_dialog("rupee_house.game_2.reward.blue_rupees", game_2_give_reward)
      game_2_reward = 7 * game_2_bet
    elseif symbols[1] == 4 then -- 3 red rupees
      map:start_dialog("rupee_house.game_2.reward.red_rupees", game_2_give_reward)
      game_2_reward = 10 * game_2_bet
    elseif symbols[1] == 5 then -- 3 Yoshi
      map:start_dialog("rupee_house.game_2.reward.yoshi", game_2_give_reward)
      game_2_reward = 20 * game_2_bet
    else -- other symbol
      map:start_dialog("rupee_house.game_2.reward.same_any", game_2_give_reward)
      game_2_reward = 4 * game_2_bet
    end

  elseif green_found and blue_found and red_found then
    -- three rupees with different colors
    map:start_dialog("rupee_house.game_2.reward.different_rupees", game_2_give_reward)
    game_2_reward = 15 * game_2_bet
  else
    map:start_dialog("rupee_house.game_2.reward.none", game_2_question_dialog_finished)
    game_2_reward = 0
  end

  if game_2_reward ~= 0 then
    sol.audio.play_sound("secret")
  else
    sol.audio.play_sound("wrong")
  end

  hero:unfreeze()
end

function map:reset_blocks()

  for i = 3, 16 do
    self:get_entity("block_" .. i):reset()
  end
end

