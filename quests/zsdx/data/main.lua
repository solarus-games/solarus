events = {
    display = nil,
    key_pressed = nil,
    key_released = nil,
    joyad_button_pressed = nil,
    joyad_button_released = nil,
    joyad_axis_moved = nil,
    joyad_hat_moved = nil,
    direction_pressed = nil,
}

-- special function: called every tick by the engine!
function sol.events.update()

end

function sol.events.start()
    -- Just need this here, no need to require globally
    local title_screen = require 'screens/title.oop'

    t = title_screen:new()

    -- Call an initial screen. We need to do this.
    sol.main.start_screen(t)

    -- Maybe this should be called from a start_screen callback?
    t:on_started()
end

function sol.events.display(dst)
    if events["display"] then
        events["display"]:on_display(dst)
    end
end

function sol.events.key_pressed(key, mods)
    if events["key_pressed"] then
        events["key_pressed"]:on_key_pressed(key, mods)
    end
end

function sol.events.key_released(key)
    if events["key_released"] then
        events["key_released"]:on_key_released(key)
    end
end

function sol.events.joypad_button_pressed(button)
    if events["joypad_button_pressed"] then
        events["joypad_button_pressed"]:on_joypad_button_presed(button)
    end
end

function sol.events.joypad_button_released(button)
    if events["joypad_button_released"] then
        events["joypad_button_released"]:on_joypad_button_released(button)
    end
end

function sol.events.joypad_axis_moved(axis, state)
    if events["joypad_axis_moved"] then
        events["joypad_axis_moved"]:on_joypad_axis_moved(axis, state)
    end
end

function sol.events.joypad_hat_moved(hat, direction8)
    if events["joypad_hat_moved"] then
        events["joypad_hat_moved"]:on_joypad_hat_moved(hat, direction8)
    end
end

function sol.events.direction_pressed(direction8)
    if events["direction_pressed"] then
        events["direction_pressed"]:on_direction_pressed(direction8)
    end
end

