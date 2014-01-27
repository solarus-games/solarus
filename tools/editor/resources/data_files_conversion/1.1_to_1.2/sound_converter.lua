-- This module reads creates the new necessary sound files for Solarus 1.2.
-- In Solarus 1.2 there is a new required sound: "running_obstacle" that is
-- played instead of "explosion" when hitting a wall while running.

local converter = {}

function converter.convert(quest_path)

  -- Copy explosion.ogg to running_obstacle.ogg.
  -- There is no copy function in Lua but I don't want to add a dependency just for that.

  local output_file_name = quest_path .. "/data/sounds/running_obstacle.ogg"
  local output_file = io.open(output_file_name, "r")
  if output_file ~= nil then
    -- Already exists: do nothing.
    output_file:close()
    return
  end

  local input_file_name = quest_path .. "/data/sounds/explosion.ogg"
  local input_file, error_message = io.open(input_file_name, "rb")
  if input_file == nil then
    -- The explosion sound does not exist.
    -- The quest is probably in an early development step.
    -- Doing nothing will not harm, it will keep the situation unchanged.
    return
  end

  local output_file, error_message = io.open(output_file_name, "w")
  if output_file == nil then
    error("Cannot open sound file for writing: " .. error_message)
  end

  local bytes = input_file:read("*a")
  output_file:write(bytes)

  input_file:close()
  output_file:close()
end

return converter

