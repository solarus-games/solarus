-- This module checks the items of a quest.
-- Only one function does the work: validator.check().

local validator = {}

local report = require("report")

function validator.check(quest_path, resources, dialogs, sprites)

  print("Checking items")

  local item_resource = resources.item
  local language_resource = resources.language

  for _, item_element in ipairs(item_resource) do

    local item_id = item_element.id

    print("Checking item '" .. item_id .. "'")

    -- Check that a treasure dialog is defined for this item.
    local dialog_id = "_treasure." .. item_id .. ".1"
    for _, language_element in ipairs(language_resource) do
      
      local language_id = language_element.id
      local dialogs = dialogs[language_id]
      if dialogs[dialog_id] == nil then
        report.warning("Missing treasure dialog '" .. dialog_id ..
            "' for item '" .. item_id .. "' in language '" .. language_id .. "'")
      end
    end

    -- TODO check that sprite entities/items has the animation.
  end

  return dialogs_by_language
end

return validator

