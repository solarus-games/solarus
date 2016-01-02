local map = ...

function map:on_started()

  local num_entities = 0
  for entity in map:get_entities() do
    num_entities = num_entities + 1
  end

  local num_entities_in_rectangle = 0
  for entity in map:get_entities_in_rectangle(0, 0, map:get_size()) do
    num_entities_in_rectangle = num_entities_in_rectangle + 1
  end

  assert(num_entities_in_rectangle == num_entities)

  sol.main.exit()
end
