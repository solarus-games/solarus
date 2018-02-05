local function format_value(value)

  if type(value) == "string" then
    return "'" .. value .. "'"
  end
  return tostring(value)
end

function assert_equal(actual, expected)

  if actual ~= expected then
    error("equality assertion failed: expected " ..
        format_value(expected) .. ", got " ..
        format_value(actual) .. "")
  end
end

function sol.main.on_started()

  sol.language.set_language("en")
end

local game_meta = sol.main.get_metatable("game")
function game_meta:on_started()

  local hero = self:get_hero()
  hero:set_tunic_sprite_id("main_heroes/eldran")
  self:set_max_life(40)
  self:set_life(self:get_max_life())
end
