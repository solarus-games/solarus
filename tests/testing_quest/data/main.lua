function assert_equal(actual, expected)

  if actual ~= expected then
    error("equality assertion failed: expected '" ..
        tostring(expected) .. "', got '" ..
        tostring(actual) .. "'")
  end

end

local game_meta = sol.main.get_metatable("game")
function game_meta:on_started()

  local hero = self:get_hero()
  hero:set_tunic_sprite_id("main_heroes/edgar")
end
