-- This module checks the items of a quest.
-- Only one function does the work: validator.check().

local validator = {}

local report = require("report")

local item_sprite_id = "entities/items"

-- Checks an individual item.
local function check_item(item_id, resources, dialogs, item_sprite)

  print("Checking item '" .. item_id .. "'")

  -- Check that sprite entities/items has the animation of this item.
  if item_sprite ~= nil then

    if item_sprite[item_id] == nil then
      report.error("Missing treasure animation '" .. item_id
          .. "' in sprite '" .. item_sprite_id .. "'")
    end
  end

  -- Check that a treasure dialog is defined for this item.
  local language_resource = resources.language
  local dialog_id = "_treasure." .. item_id .. ".1"
  for _, language_element in ipairs(language_resource) do
 
    local language_id = language_element.id
    local dialogs = dialogs[language_id]
    if dialogs[dialog_id] == nil then
      report.error("Missing treasure dialog '" .. dialog_id ..
          "' for item '" .. item_id .. "' in language '" .. language_id .. "'")
    end
  end
end

-- Checks all items.
function validator.check(quest_path, resources, dialogs, sprites)

  print("Checking items")

  local item_resource = resources.item

  local item_sprite = sprites[item_sprite_id]
  if item_sprite == nil then
    report.error("Missing items sprite '" .. item_sprite_id .. "'")
  end

  for _, item_element in ipairs(item_resource) do
    local item_id = item_element.id
    check_item(item_id, resources, dialogs, item_sprite)
  end
end

return validator

