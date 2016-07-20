local map = ...

function map:on_started()

  witch:get_sprite():set_animation("witch_mixing")
  sol.main.exit()
end
