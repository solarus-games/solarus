-- Solarus 1.0 to 1.1.
-- Sprite data files change to a more readable syntax.
-- The sprite features remain the same.

local converter = {}

-- Parses a sprite data file in 1.0 format and returns a table representing
-- the sprite information.
local function load_sprite(quest_path, sprite_id)

  local input_file_name = quest_path .. "/data/sprites/" .. sprite_id .. ".dat"
  local input_file, error_message = io.open(input_file_name)
  if input_file == nil then
    error("Cannot open old sprite file for reading: " .. error_message)
  end

  local sprite = {}
  local line_number = 0
  local animation = nil
  local num_directions = 0
  for line in input_file:lines() do

    line_number = line_number + 1

    if #line > 0 then  -- Skip empty lines.

      if animation == nil then

        -- The line is a new animation declaration.
        animation = {}
        animation.directions = {}
        animation.name, animation.src_image, num_directions, animation.frame_delay, animation.frame_to_loop_on =
            line:match("^(%S+)%s+(%S+)%s+(%S+)%s+(%S+)%s+(%S+)%s*$")

        num_directions = tonumber(num_directions)
        animation.frame_delay = tonumber(animation.frame_delay)
        animation.frame_to_loop_on = tonumber(animation.frame_to_loop_on)
        if animation.name == nil
          or num_directions == nil
          or animation.frame_delay == nil
          or animation.frame_to_loop_on == nil then
          error(input_file_name .. ": line " .. line_number .. ": Invalid animation declaration")
        end

        if animation.frame_delay == 0 then
          -- nil or 0 means no frame end in Solarus 1.1
          animation.frame_delay = nil
        end

        if animation.frame_to_loop_on == -1 then
          -- nil means no loop in Solarus 1.1.
          animation.frame_to_loop_on = nil
        end

      else
        -- The line is a direction.
        local direction = {}
        direction.x, direction.y, direction.frame_width, direction.frame_height,
        direction.origin_x, direction.origin_y, direction.num_frames, direction.num_columns =
        line:match("^(%S+)%s+(%S+)%s+(%S+)%s+(%S+)%s+(%S+)%s+(%S+)%s+(%S+)%s+(%S+)%s*$")

        direction.x = tonumber(direction.x)
        direction.y = tonumber(direction.y)
        direction.frame_width = tonumber(direction.frame_width)
        direction.frame_height = tonumber(direction.frame_height)
        direction.origin_x = tonumber(direction.origin_x)
        direction.origin_y = tonumber(direction.origin_y)
        direction.num_frames = tonumber(direction.num_frames)
        direction.num_columns = tonumber(direction.num_columns)

        if direction.x == nil
          or direction.y == nil
          or direction.frame_width == nil
          or direction.frame_height == nil
          or direction.origin_x == nil
          or direction.origin_y == nil
          or direction.num_frames == nil
          or direction.num_columns == nil then
          error(input_file_name .. ": line " .. line_number .. ": Invalid direction definition")
        end

        -- 0,0 is the default origin.
        if direction.origin_x == 0 then
          direction.origin_x = nil
        end
        if direction.origin_y == 0 then
          direction.origin_y = nil
        end

        if direction.num_columns == direction.num_frames then
          -- nil means num_frames columns.
          direction.num_columns = nil
        end

        if direction.num_frames == 1 then
          -- Default value.
          direction.num_frames = nil
        end

        animation.directions[#animation.directions + 1] = direction

        if #animation.directions == num_directions then
          -- All directions of this animations were parsed.
          sprite[#sprite + 1] = animation
          animation = nil
        end
      end
    end
  end

  input_file:close()

  return sprite
end

function converter.convert(quest_path, sprite_id)

  local sprite = load_sprite(quest_path, sprite_id)

  local output_file_name = quest_path .. "/data/sprites/" .. sprite_id .. ".dat"
  local output_file, error_message = io.open(output_file_name, "w")
  if output_file == nil then
    error("Cannot open new sprite file for writing: " .. error_message)
  end

  for _, animation in ipairs(sprite) do

    -- Write the animation properties.
    output_file:write("animation{\n");
    output_file:write("  name = \"", animation.name, "\",\n")
    output_file:write("  src_image = \"", animation.src_image, "\",\n")

    if animation.frame_delay ~= nil then
      output_file:write("  frame_delay = ", animation.frame_delay, ",\n")
    end

    if animation.frame_to_loop_on ~= nil then
      output_file:write("  frame_to_loop_on = ", animation.frame_to_loop_on, ",\n")
    end

    output_file:write("  directions = {\n")

    for _, direction in ipairs(animation.directions) do
      output_file:write("    { x = ", direction.x,
          ", y = ", direction.y,
          ", frame_width = ", direction.frame_width,
          ", frame_height = ", direction.frame_height)
      if direction.origin_x ~= nil then
        output_file:write(", origin_x = ", direction.origin_x)
      end
      if direction.origin_y ~= nil then
        output_file:write(", origin_y = ", direction.origin_y)
      end
      if direction.num_frames ~= nil then
        output_file:write(", num_frames = ", direction.num_frames)
      end
      if direction.num_columns ~= nil then
        output_file:write(", num_columns = ", direction.num_columns)
      end
      output_file:write(" },\n")
    end

    output_file:write("  },\n")
    output_file:write("}\n\n")
  end

  output_file:close()
end

return converter

