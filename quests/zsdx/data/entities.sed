s/map:[a-z]*_set_activated("\([a-zA-Z0-9_]*\)", */\1:set_activated(/g
s/map:[a-z]*_set_activated(\([a-zA-Z0-9_\." ]*\), */map:get_entity(\1):set_activated(/g
s/map:[a-z]*_is_activated("\([a-zA-Z0-9_]*\)")/\1:is_activated()/g
s/map:[a-z]*_is_activated(\([a-zA-Z0-9_\." ]*\))/map:get_entity(\1):is_activated()/g
s/map:[a-z]*_set_enabled("\([a-zA-Z0-9_]*\)", */\1:set_enabled(/g
s/map:[a-z]*_set_enabled(\([a-zA-Z0-9_\." ]*\), */map:get_entity(\1):set_enabled(/g
s/map:[a-z]*_is_enabled("\([a-zA-Z0-9_]*\)")/\1:is_enabled()/g
s/map:[a-z]*_is_enabled(\([a-zA-Z0-9_\." ]*\))/map:get_entity(\1):is_enabled()/g
s/map:[a-z]*_set_open("\([a-zA-Z0-9_]*\)", */\1:set_open(/g
s/map:[a-z]*_set_open(\([a-zA-Z0-9_\." ]*\), */map:get_entity(\1):set_open(/g
s/map:[a-z]*_is_open("\([a-zA-Z0-9_]*\)")/\1:is_open()/g
s/map:[a-z]*_is_open(\([a-zA-Z0-9_\." ]*\))/map:get_entity(\1):is_open()/g
s/map:[a-z]*_get_sprite("\([a-zA-Z0-9_]*\)")/\1:get_sprite()/g
s/map:[a-z]*_remove("\([a-zA-Z0-9_]*\)")/\1:remove()/g
s/map:[a-z]*_start_movement("\([a-zA-Z0-9_]*\)", */\1:start_movement(/g
s/map:[a-z]*_set_group_enabled/map:set_entities_enabled/g
s/map:[a-z]*_exists("\([a-zA-Z0-9_]*\)")/\1:exists()/g
s/map:[a-z]*_set_position("\([a-zA-Z0-9_]*\)", */\1:set_position(/g
s/map:[a-z]*_set_position(\([a-zA-Z0-9_\." ]*\), */map:get_entity(\1):set_position(/g
s/map:[a-z]*_get_position("\([a-zA-Z0-9_]*\)")/\1:get_position()/g
s/map:[a-z]*_get_position(\([a-zA-Z0-9_\." ]*\))/map:get_entity(\1):get_position()/g
s/map:[a-z]*_set_locked("\([a-zA-Z0-9_]*\)", */\1:set_locked(/g
s/map:[a-z]*_set_locked(\([a-zA-Z0-9_\." ]*\), */map:get_entity(\1):set_locked(/g
s/map:[a-z]*_is_locked("\([a-zA-Z0-9_]*\)")/\1:is_locked()/g
s/map:[a-z]*_is_locked(\([a-zA-Z0-9_\." ]*\))/map:get_entity(\1):is_locked()/g
s/map:[a-z]*_reset("\([a-zA-Z0-9_]*\)")/\1:reset()/g
s/map:[a-z]*_reset(\([a-zA-Z0-9_\." ]*\))/map:get_entity(\1):reset()/g
s/if destination_point_name /if destination_point:get_name() /g
s/destination_point_name/destination_point/g

