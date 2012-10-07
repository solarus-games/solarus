#!/usr/bin/lua

-- This script reads a tileset file with the format of solarus 0.9.2
-- and converts it into the format of solarus 1.0.0

-- The old format (solarus 0.9.2) is a text file with a specific syntax.
-- The new format (solarus 1.0.0) is a Lua data file.

local ground_names = {
  [0] = "traversable",
  [1] = "wall",
  [2] = "wall_top_right",
  [3] = "wall_top_left",
  [4] = "wall_bottom_left",
  [5] = "wall_bottom_right",
  [6] = "empty",
  [7] = "water_top_right",
  [8] = "water_top_left",
  [9] = "water_bottom_left",
  [10] = "water_bottom_right",
  [-1] = "deep_water",
  [-2] = "shallow_water",
  [-3] = "hole",
  [-4] = "ladder",
  [-5] = "prickles",
  [-6] = "lava",
}

local file = io.stdin
local lines = file:lines()

-- The first line is the background color.
local first_line = lines()
if first_line == nil then
  error("Empty tileset file")
end

local r, g, b = first_line:match("^([0-9]+)%s+([0-9]+)%s+([0-9]+)")

if r == nil or g == nil or b == nil then
  error("Line 1: Invalid background color")
end
io.write("background_color{ " .. r .. ", " .. g .. ", " .. b .. " }\n")

local line_number = 1
for line in file:lines() do

  -- Parse this tile pattern.
  line_number = line_number + 1
  local id, animation_type, ground, default_layer, rest =
      line:match("^([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+(.*)$")
  local x, y, width, height
  if id == nil then
    error("Line " .. line_number .. ": Syntax error")
  end

  -- Is it a multi-frame pattern? If so, the syntax of the line is different.
  local multi_frame = animation_type == "1" or animation_type == "5"
  if multi_frame then
    -- Several frames.
    local animation_sequence, x1, y1, x2, y2, x3, y3
    animation_sequence, width, height, x1, y1, x2, y2, x3, y3 =
        rest:match("^([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s*$")
    if animation_sequence == nil then
      error("Line " .. line_number .. ": Syntax error")
    elseif animation_sequence == "1" then
      x = "{ " .. x1 .. ", " .. x2 .. ", " .. x3 .. " }"
      y = "{ " .. y1 .. ", " .. y2 .. ", " .. y3 .. " }"
    elseif animation_sequence == "2" then
      x = "{ " .. x1 .. ", " .. x2 .. ", " .. x3 .. ", " .. x2 .. " }"
      y = "{ " .. y1 .. ", " .. y2 .. ", " .. y3 .. ", " .. y2 .. " }"
    end

  else
    -- Single frame.
    x, y, width, height = 
        rest:match("^([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s*$")
    if x == nil then
      error("Line " .. line_number .. ": Syntax error")
    end
  end

  local scrolling = nil
  if animation_type == "2" then
    scrolling = "self"
  elseif animation_type == "4" or animation_type == "5" then
    scrolling = "parallax"
  end

  -- Print the tile pattern.
  io.write("tile_pattern{\n")
  io.write("  id = " .. id .. ",\n")
  io.write("  ground = \"" .. ground_names[tonumber(ground)] .. "\",\n")
  io.write("  default_layer = " .. default_layer .. ",\n")
  io.write("  x = " .. x .. ",\n")
  io.write("  y = " .. y .. ",\n")
  io.write("  width = " .. width .. ",\n")
  io.write("  height = " .. height .. ",\n")
  if scrolling ~= nil then
    io.write("  scrolling = " .. scrolling .. ",\n")
  end
  io.write("}\n\n")
end

