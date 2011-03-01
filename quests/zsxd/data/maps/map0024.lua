----------------------------------
-- Crazy House 3F               --
----------------------------------

function event_hero_interaction(entity_name)
	if entity_name == "infinite_greetings" then
		sol.main.play_sound("sm64_bowser_message")
	end
end
