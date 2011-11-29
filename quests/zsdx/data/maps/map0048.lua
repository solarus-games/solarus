-----------------------------
-- Dungeon 8 B4 script     --
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

function event_hero_on_sensor(sensor_name)
	if sensor_name == "DS12" then
		sol.main.play_sound("secret")
		sol.map.door_open("LD12")
		sol.map.sensor_set_enabled("DS12", false)
	end
end
