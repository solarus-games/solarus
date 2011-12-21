-- Cannonball or random sprite
-- made for the ALTTP room in Temple of Stupidities 1F NE

local random_balls = {
  {
    sprite = "enemies/cannonball",
    sound = "cannonball",
    width = 16,
    height = 16,
    origin_x = 8,
    origin_y = 13,
    dx = -8,
    dy = 0
  },
  {
    sprite = "enemies/cannonball",
    sound = "cannonball",
    width = 16,
    height = 16,
    origin_x = 8,
    origin_y = 13,
    dx = 8,
    dy = 0
  },
  {
    sprite = "enemies/cannonball_big",
    sound = "cannonball",
    width = 32,
    height = 32,
    origin_x = 16,
    origin_y = 29,
    dx = 0,
    dy = 0
  },
  {
    sprite = "enemies/dkc_bananas",
    sound = "cannonball",
    width = 32,
    height = 32,
    origin_x = 16,
    origin_y = 29,
    dx = 0,
    dy = 0
  },
  {
    sprite = "enemies/dkc_barrel",
    sound = "cannonball",
    width = 32,
    height = 40,
    origin_x = 16,
    origin_y = 29,
    dx = 0,
    dy = 8
  },
  {
    sprite = "enemies/mc_pickaxe",
    sound = "cannonball",
    width = 32,
    height = 32,
    origin_x = 16,
    origin_y = 29,
    dx = 0,
    dy = 0
  },
  {
    sprite = "enemies/smb_block",
    sound = "smb_coin",
    width = 16,
    height = 16,
    origin_x = 8,
    origin_y = 13,
    dx = -8,
    dy = 0
  },
  {
    sprite = "enemies/smb_block",
    sound = "smb_coin",
    width = 16,
    height = 16,
    origin_x = 8,
    origin_y = 13,
    dx = 8,
    dy = 0
  },
  {
    sprite = "enemies/smk_mario",
    sound = "mk64_mario_yeah",
    width = 32,
    height = 32,
    origin_x = 16,
    origin_y = 29,
    dx = 0,
    dy = 0
  },
  {
    sprite = "enemies/smk_yoshi",
    sound = "mk64_yoshi",
    width = 32,
    height = 32,
    origin_x = 16,
    origin_y = 29,
    dx = 0,
    dy = 0
  },
  {
    sprite = "enemies/smk_toad",
    sound = "mk64_toad",
    width = 32,
    height = 32,
    origin_x = 16,
    origin_y = 29,
    dx = 0,
    dy = 0
  }
}

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(0)
  sol.enemy.set_invincible()

  local index
  if math.random(4) == 4 then
    -- choose between all stupid stuff
    index = math.random(#random_balls)
  else
    -- choose between the 3 real cannonballs
    index = math.random(3)
  end
  local props = random_balls[index]

  sol.enemy.create_sprite(props.sprite)
  sol.enemy.set_size(props.width, props.height)
  sol.enemy.set_origin(props.origin_x, props.origin_y)
  local x, y = sol.enemy.get_position()
  sol.enemy.set_position(x + props.dx, y + props.dy)
  sol.main.play_sound(props.sound)
end

function event_restart()

  local m = sol.main.straight_movement_create(48, math.pi * 3 / 2)
  sol.enemy.start_movement(m)
end

function event_obstacle_reached()

  sol.map.enemy_remove(sol.enemy.get_name())
end

