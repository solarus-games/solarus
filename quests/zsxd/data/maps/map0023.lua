----------------------------------
-- Crazy House 2FB (north)      --
----------------------------------

function event_hero_interaction(entity_name)
-- Guichet 82 -------------------------------------------------
-- TODO: dialogues à vérifier, script à faire
	if entity_name == "GC82" then
		sol.main.dialog_start("crazy_house.guichet_82")

-- Guichet 84 -------------------------------------------------
-- TODO: dialogues à finir, script à finir
	elseif entity_name == "GC84" then
		if sol.main.savegame_get_integer(1410) == 3 then
			sol.main.dialog_start("crazy_house.guichet_84_ech_eq_3")
		elseif sol.main.savegame_get_integer(1410) >= 8 then
			sol.main.dialog_start("crazy_house.guichet_84_ech_ge_8")
		else
			-- TODO: dialogue à faire
			sol.main.dialog_start("crazy_house.guichet_84_ech_le_2")
		end
	end
end
