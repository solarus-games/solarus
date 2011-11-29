



function event_map_started(destination_point_name)
	sol.map.door_set_open("LD12", true)
end

function event_hero_on_sensor(sensor_name)
	if sensor_name == "DS12" then
		sol.map.door_close("LD12")
		sol.map.sensor_set_enabled("DS12", false)
	end
end

DB1_status = false
DB2_status = false

function event_switch_activated(switch_name)
	if switch_name == "DB1" then
		DB1_status = true
	elseif switch_name == "DB2" then
		DB2_status = true
	elseif switch_name == "DB3" then
		sol.map.door_open("LD12")
		sol.map.door_open("LD13")
		sol.main.play_sound("secret")
	end
	if DB1_status == true and DB2_status == true then
		sol.map.door_open("LD14")
		sol.main.play_sound("secret")
	end
end
