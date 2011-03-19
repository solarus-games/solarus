----------------------------------
-- Crazy House 1FB (north)       --
----------------------------------

-- Guichet 41 -------------------------------------------------
-- TODO: dialogues à finir
function guichet_41()
	if sol.game.savegame_get_integer(1410) == 3 then
		sol.map.dialog_start("crazy_house.guichet_41_ech_eq_3")
	else
		sol.map.dialog_start("crazy_house.guichet_41_ech_ne_3")
	end
end

-- Guichet 43 -------------------------------------------------
function guichet_43()
	sol.map.dialog_start("crazy_house.guichet_43")
end

-- Guichet 45 -------------------------------------------------
-- TODO: dialogues à faire, script à faire
function guichet_45()
	if sol.game.savegame_get_integer(1410) == 3 then
		sol.map.dialog_start("crazy_house.guichet_45_ech_eq_3")
	else
		
	end
end

-- Guichet 47 -------------------------------------------------
-- TODO: dialogues à faire, script à faire
function guichet_47()

end

-- Guichet 49 -------------------------------------------------
-- TODO: dialogues à finir, script à faire
function guichet_49()

end

function event_hero_interaction(entity_name)
	if entity_name == "mario_message_2" then
-- Tableau de mario qui parle ---------------------------------
		sol.main.play_sound("sm64_heehee")
	elseif entity_name == "GC41" then
		guichet_41()
	elseif entity_name == "GC43" then
		guichet_43()
	elseif entity_name == "GC45" then
		guichet_45()
	elseif entity_name == "GC47" then
		guichet_47()
	elseif entity_name == "GC49" then
		guichet_49()
	end
end

