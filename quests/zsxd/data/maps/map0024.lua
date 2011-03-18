----------------------------------
-- Crazy House 3F               --
----------------------------------

function event_map_started(destination_point_name)
	-- Desactivation des sensors pour la porte du couloir sans fin	
	sol.map.sensor_set_enabled("bowser_leave", false)
	sol.map.sensor_set_enabled("bowser_exit", false)
end

function event_map_opening_transition_finished(destination_point_name)
	-- Affichage du nom du donjon quand on vient de l'escalier de dehors
  -- TODO: à tester
  if destination_point_name == "fromOutsideSO" then
    sol.map.dialog_start("crazy_house.title")
  end
end

function event_hero_interaction(entity_name)
	if entity_name == "GC31" then
-- Guichet 31 -------------------------------------------------
-- TODO: dialogues et script à faire

	elseif entity_name == "GC32" then
-- Guichet 32 -------------------------------------------------
-- TODO: dialogues à vérifier, script à finir
		if sol.main.savegame_get_integer(1410) <= 6 then
			sol.map.dialog_start("crazy_house.guichet_32_ech_le_6")
		end

	elseif entity_name == "GC33" then
-- Guichet 33 -------------------------------------------------
-- TODO: dialogues à finir, script à finir
		if sol.main.savegame_get_integer(1410) == 3 then	
			-- Concernant le guichet 41	
			-- TODO: dialogue ok, script à finir
			sol.map.dialog_start("crazy_house.guichet_33_ech_eq_3")
		elseif sol.main.savegame_get_integer(1410) >= 4 then
			-- Clé du dépôt (guichet 56)
			-- TODO: dialogue ok, script à finir
			sol.map.dialog_start("crazy_house.guichet_33_ech_ge_4")
		else
			-- De quoi il s'agit ? TODO: Dialogue à faire
			sol.map.dialog_start("crazy_house.guichet_33_ech_le_2")
		end

	elseif entity_name == "GC36" then
-- Guichet 36 -------------------------------------------------
-- TODO: dialogues et script à faire

	elseif entity_name == "Apothicaire" then
-- Apothicaire ------------------------------------------------
-- TODO: dialogues presque finis, script à finir (options)
		sol.map.dialog_start("crazy_house.apothicaire")
	end
end

function event_hero_on_sensor(sensor_name)
	-- Link approche de la porte qui annonce le couloir sans fin	
	if sensor_name == "bowser_message" then
		sol.main.play_sound("sm64_bowser_message")
		sol.map.dialog_start("crazy_house.infinite_greetings")
		sol.map.sensor_set_enabled("bowser_close", true)
	end
	-- Fermeture de la porte derrière Link après être entré
	if sensor_name == "bowser_close" then
		sol.map.door_set_open("bowser_door", false)
		sol.main.play_sound("door_closed")
		sol.map.sensor_set_enabled("bowser_exit", true)
		sol.map.sensor_set_enabled("bowser_close", false)
	end
	-- Ouverture de la porte si Link souhaite sortir
	if sensor_name == "bowser_exit" then
		sol.map.door_set_open("bowser_door", true)
		sol.main.play_sound("door_open")
		sol.map.sensor_set_enabled("bowser_message", false)
		sol.map.sensor_set_enabled("bowser_close", true)
		sol.map.sensor_set_enabled("bowser_exit", false)
	end
	-- Fermeture de la porte derrière Link après être sorti
	if sensor_name == "bowser_leave" then
		sol.map.door_set_open("bowser_door", false)
		sol.main.play_sound("door_closed")
		sol.map.sensor_set_enabled("bowser_message", true)
		sol.map.sensor_set_enabled("bowser_leave", false)
	end
	-- Mécanisme du couloir sans fin
	if sensor_name == "infinite_corridor" then
		sol.map.hero_set_position(1136, 797, 1)
	end
end

function event_dialog_finished(first_message_id, answer)
	if first_message_id == "crazy_house.infinite_greetings" then
		sol.map.door_set_open("bowser_door", true)
		sol.map.sensor_set_enabled("bowser_leave", true)
		sol.main.play_sound("door_open")
	end
end

-- Déplacement de la camera le long du couloir après appui sur
-- le giga bouton
function giga_bouton_camera_move()
	sol.map.camera_move(680, 792, 250)
	sol.main.timer_stop("giga_bouton_camera_move")
end

-- Retour de la camera suite à l'appui sur le giga bouton
function giga_bouton_camera_back()
	sol.map.camera_restore()
end

function event_switch_activated(switch_name)
	-- Appui sur le giga bouton
	if switch_name == "giga_bouton" then
		sol.map.hero_freeze()		
		sol.map.sensor_set_enabled("infinite_corridor", false)	
		sol.main.play_sound("secret")
		sol.main.timer_start(500, "giga_bouton_camera_move", false)
	end
end

function event_camera_reached_target()
	sol.main.timer_start(1500, "giga_bouton_camera_back", false)
end

function event_camera_back()
	sol.map.hero_unfreeze()
end
