local map = ...
-- Dungeon 10 2F

local function are_group1_torches_on()

  return torch1_1:exists()
    and torch1_1:get_sprite():get_animation() == "lit"
    and torch1_2:get_sprite():get_animation() == "lit"
    and torch1_3:get_sprite():get_animation() == "lit"
end

local function are_group2_torches_on()

  return torch2_1:exists()
    and torch2_1:get_sprite():get_animation() == "lit"
    and torch2_2:get_sprite():get_animation() == "lit"
    and torch2_3:get_sprite():get_animation() == "lit"
end

local function lock_torches_group1()

  torch1_1:remove()
  torch1_2:remove()
  torch1_3:remove()
end

local function lock_torches_group2()

  torch2_1:remove()
  torch2_2:remove()
  torch2_3:remove()
end

local function explode_block1()

  map:create_explosion(536, 256, 1)
  sol.audio.play_sound("explosion")
  sol.audio.play_sound("secret")
  block1_1:set_enabled(false)
end

local function explode_block2()

  map:create_explosion(552, 256, 1)
  sol.audio.play_sound("explosion")
  sol.audio.play_sound("secret")
  block2_1:set_enabled(false)
end

function map:on_started(destination)

  if map:get_game():get_boolean(205) then
    block1_1:set_enabled(false)
    lock_torches_group1()
  end

  if map:get_game():get_boolean(206) then
    block2_1:set_enabled(false)
    lock_torches_group2()
  end

  if map:get_game():get_boolean(227) then
    switch1_1:set_activated(true)
    switch1_2:set_activated(true)
    switch1_3:set_activated(true)
    switch1_4:set_activated(true)
  end
end

local function switch1_error()

  sol.audio.play_sound("wrong")
  switch1_1:set_activated(false)
  switch1_2:set_activated(false)
  switch1_3:set_activated(false)
  switch1_4:set_activated(false)
end

function switch1_1:on_activated()

  if switch1_2:is_activated()
      or switch1_3:is_activated()
      or switch1_4:is_activated() then
    switch1_error()
  end
end

function switch1_2:on_activated()

  if not switch1_1:is_activated() then
    switch1_error()
  end
end

function switch1_3:on_activated()

  if not switch1_2:is_activated() then
    switch1_error()
  end
end

function switch1_4:on_activated()

  if not switch1_3:is_activated() then
    switch1_error()
  else
    sol.audio.play_sound("secret")
    map:create_pickable("small_key", 1, 227, 144, 504, 0)
  end
end

local function enemy_in_group2_dead(enemy)

  if not map:has_entities("enemy_group2")
      and not map:get_game():get_boolean(221) then

    map:move_camera(232, 728, 250, function()
      map:create_pickable("small_key", 1, 221, 232, 733, 1)
      sol.audio.play_sound("secret")
    end)
  end
end
for _, enemy in ipairs(map:get_entities("enemy_group2")) do
  enemy.on_dead = enemy_in_group2_dead
end

function map:on_update()

  if not map:get_game():get_boolean(205)
      and are_group1_torches_on() then
    map:get_game():set_boolean(205, true)
    lock_torches_group1()
    map:move_camera(536, 256, 250, explode_block1)
  end

  if not map:get_game():get_boolean(206)
      and are_group2_torches_on() then
    map:get_game():set_boolean(206, true)
    lock_torches_group2()
    map:move_camera(552, 256, 250, explode_block2)
  end
end

