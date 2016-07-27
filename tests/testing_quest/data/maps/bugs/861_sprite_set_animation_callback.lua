local map = ...

local animation_finished_count = 0

function map:on_opening_transition_finished()

  local sprite = custom_entity:get_sprite()

  function sprite:on_animation_finished()

    animation_finished_count = animation_finished_count + 1

    if animation_finished_count == 1 then
      sprite:set_animation("explosion", function()
        sprite:set_animation("explosion", "explosion")
      end)
    end
  end

  sol.timer.start(5000, function()
    assert_equal(animation_finished_count, 4)
    sprite:set_animation("explosion")
    sol.main.exit()
  end)
end
