-- Very basic ini parser used to load old data files from solarus 0.9.0.

-- Reads an arbitrary ini file and returns a table with all groups and
-- an array with all their ids.
function ini_parse(file)

  file = file or io.stdin
  local all_ids = {}      -- array of all group ids
  local all_groups = {}   -- table of all groups
  local group = {}        -- current group
  local line_number = 0

  for line in file:lines() do

    line_number = line_number + 1

    -- skip empty lines and comments
    if not line:find("^[ \t]*$") and not line:find("^#") then

      local id = line:match("^ *%[(.*)%]")
      local key, value = line:match("^ *([-_a-zA-Z0-9]*) *= *(.*)")

      if id then
        -- new group
        if group.id ~= nil then
          if all_groups[group.id] then
            error("Duplicate group '" .. group.id .. "'")
          end
          -- finish the previous group
          all_groups[group.id] = group 
          all_ids[#all_ids + 1] = group.id
        end
        -- create a new one
        group = {}
        group.id = id
      elseif key and value then
        -- line with a value
        group[key] = value
      else
        print("-- Warning: ignoring invalid line " .. line_number .. ": '"
            .. line .. "'")
      end
    end
  end

  if #all_ids == 0 then
    error("No groups were found. Is this an ini file?")
  end

  all_groups[group.id] = group
  all_ids[#all_ids + 1] = group.id

  return all_groups, all_ids
end

