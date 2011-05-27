-----------------------------
-- Dungeon 8 B2 script     --
-----------------------------

-- Legend
-- RC: Rupee Chest
-- KC: Key Chest
-- KP: Key Pot
-- LD: Locked Door
-- KD: Key Door
-- DB: Door Button
-- LB: Locked Barrier
-- BB: Barrier Button
-- DS: Door Sensor

function event_map_started(destination_point_name)
	sol.map.door_set_open("LD11", true)
end

function event_hero_on_sensor(sensor_name)
	if sensor_name == "DS11" then
		sol.map.door_close("LD11")
		sol.map.sensor_set_enabled("DS11", false)
	end
end
