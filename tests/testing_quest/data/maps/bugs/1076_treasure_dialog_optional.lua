local map = ...
local game = map:get_game()

local test_treasure_dialog_sprite
local test_treasure_dialog_no_sprite
local test_treasure_no_dialog_sprite
local test_treasure_no_dialog_no_sprite

-- 1. Treasure with both a dialog and a sprite.
function test_treasure_dialog_sprite()

  local treasure_callback_done = false

  assert(hero:get_state() == "free")
  hero:start_treasure("item_dialog_sprite", 1, nil, function()
    treasure_callback_done = true
  end)

  -- The treasure dialog should be active.
  assert(not treasure_callback_done)
  assert(game:is_dialog_enabled())
  assert(hero:get_state() == "treasure")
  assert(hero:get_sprite():get_animation() == "brandish")
  
  local timer = sol.timer.start(map, 100, function()
    assert(not treasure_callback_done)
    assert(game:is_dialog_enabled())
    assert(hero:get_state() == "treasure")
    assert(hero:get_sprite():get_animation() == "brandish")
  end)
  timer:set_suspended_with_map(false)

  local timer = sol.timer.start(map, 3000, function()
    assert(not treasure_callback_done)
    -- Pass the dialog.
    game:simulate_command_pressed("action")
    assert(treasure_callback_done)
  end)
  timer:set_suspended_with_map(false)
end

-- 2. Treasure with a dialog but no a sprite.
function test_treasure_dialog_no_sprite()

  local treasure_callback_done = false

  assert(hero:get_state() == "free")
  hero:start_treasure("item_dialog_no_sprite", 1, nil, function()
    treasure_callback_done = true
  end)

  assert(game:is_dialog_enabled())
  assert(hero:get_state() == "treasure")
  assert(hero:get_sprite():get_animation() == "brandish")

  local timer = sol.timer.start(map, 100, function()
    assert(not treasure_callback_done)
    assert(game:is_dialog_enabled())
    assert(hero:get_state() == "treasure")
    assert(hero:get_sprite():get_animation() == "brandish")
  end)
  timer:set_suspended_with_map(false)
  
  local timer = sol.timer.start(map, 3000, function()
    assert(not treasure_callback_done)
    game:simulate_command_pressed("action")
    assert(treasure_callback_done)
  end)
  timer:set_suspended_with_map(false)
end

-- 3. Treasure with no dialog but a sprite.
function test_treasure_no_dialog_sprite()

  local treasure_callback_done = false

  assert(hero:get_state() == "free")
  hero:start_treasure("item_no_dialog_sprite", 1, nil, function()
    treasure_callback_done = true
  end)
  assert(not treasure_callback_done)
  assert(hero:get_state() == "treasure")
  assert(hero:get_sprite():get_animation() == "brandish")

  sol.timer.start(map, 100, function()
    assert(not treasure_callback_done)
    assert(hero:get_state() == "treasure")
    assert(hero:get_sprite():get_animation() == "brandish")
  end)

  sol.timer.start(map, 4000, function()
    assert(treasure_callback_done)
    assert(hero:get_state() == "free")
  end)
end

-- 4. Treasure with no a dialog and no sprite.
function test_treasure_no_dialog_no_sprite()

  local treasure_callback_done = false

  assert(hero:get_state() == "free")
  hero:start_treasure("item_no_dialog_no_sprite", 1, nil, function()
    treasure_callback_done = true
  end)
  assert(not treasure_callback_done)
  assert(hero:get_state() == "treasure")
  assert(hero:get_sprite():get_animation() == "brandish")

  sol.timer.start(map, 1000, function()
    assert(not treasure_callback_done)
    assert(hero:get_state() == "treasure")
    assert(hero:get_sprite():get_animation() == "brandish")
  end)

  sol.timer.start(map, 4000, function()
    assert(treasure_callback_done)
  end)
end

function map:on_opening_transition_finished()

  test_treasure_dialog_sprite()
  sol.timer.start(map, 10000, test_treasure_dialog_no_sprite)
  sol.timer.start(map, 20000, test_treasure_no_dialog_sprite)
  sol.timer.start(map, 30000, test_treasure_no_dialog_no_sprite)
  sol.timer.start(map, 40000, sol.main.exit)
end
