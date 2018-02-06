local map = ...

-- Test for sol.surface.get_pixels().
local function test_get_pixels()
  local surface = sol.surface.create(16, 16)
  surface:fill_color({128, 64, 0, 255})

  local pixels = surface:get_pixels()
  assert_equal(#pixels, 16 * 16 * 4)
  local r, g, b, a = pixels:byte(1, 4)
  assert_equal(r, 128)
  assert_equal(g, 64)
  assert_equal(b, 0)
  assert_equal(a, 255)
end

-- Test for sol.surface.set_pixels().
local function test_set_pixels()

  local surface = sol.surface.create(16, 16)
  surface:fill_color({255, 64, 0, 255})

  local pixels = surface:get_pixels()
  surface:set_pixels(pixels:reverse())
  pixels = surface:get_pixels()
  assert_equal(#pixels, 16 * 16 * 4)
  local r, g, b, a = pixels:byte(1, 4)
  assert_equal(r, 255)
  assert_equal(g, 0)
  assert_equal(b, 64)
  assert_equal(a, 255)
end

test_get_pixels()
test_set_pixels()

sol.main.exit()
