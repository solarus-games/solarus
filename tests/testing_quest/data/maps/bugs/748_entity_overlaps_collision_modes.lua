local map = ...

local all_tests = {
  "overlapping",
  "containing",
  "origin",
  "facing",
  "touching",
  "center",
  "sprite",
}

local function check_collisions(entity, detector, expected_tests)

  for _, test in ipairs(all_tests) do
    local expects_collision = expected_tests[test] == true
    print("Testing collision " .. test .. " of " .. entity:get_name() ..
      " on detector " .. detector:get_name() ..
      ", expects " .. (expects_collision and "true" or "false"))
    local collision = detector:overlaps(entity, test)
    if expects_collision then
      assert(collision)
    else
      assert(not collision)
    end
  end
end

function map:on_started()

  -- Hero.
  check_collisions(hero, npc, {
    facing = true,
    touching = true,
  })

  check_collisions(hero, sensor, {
    overlapping = true,
    origin = true,
    center = true,
    touching = true,
  })

  check_collisions(hero, block, {
    overlapping = true,
    origin = true,
    center = true,
    sprite = true,
    touching = true,
  })

  -- NPC.
  check_collisions(npc, hero, {
    touching = true,
  })

  check_collisions(npc, sensor, {
  })

  check_collisions(npc, block, {
  })

  -- Block.
  check_collisions(block, hero, {
    overlapping = true,
    sprite = true,
  })

  check_collisions(block, npc, {
  })

  check_collisions(block, sensor, {
    overlapping = true,
    containing = true,
    origin = true,
    center = true,
    touching = true,
  })

  -- Sensor.
  check_collisions(sensor, hero, {
    overlapping = true,
  })

  check_collisions(sensor, npc, {
  })

  check_collisions(sensor, block, {
    overlapping = true,
    origin = true,
  })

  sol.main.exit()
end
