----------------------------------
-- Crazy House 1F (north)       --
----------------------------------

function event_hero_interaction(entity_name)
	if entity_name == "mario_message_2" then
		-- Tableau de mario qui parle
		sol.main.play_sound("sm64_heehee")
	end
end
