local start_location_checker = {}

function start_location_checker.check(
    game,
    expected_map_id_relative,
    expected_destination_name
)
  local actual_map_id, actual_destination_name = game:get_starting_location()
  assert(actual_map_id == "bugs/819_destination_starting_location_mode/" .. expected_map_id_relative)
  assert(actual_destination_name == expected_destination_name)
end

return start_location_checker
