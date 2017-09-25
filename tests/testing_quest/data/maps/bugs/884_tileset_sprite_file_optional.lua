local map = ...

function map:on_started()

  sol.timer.start(10, sol.main.exit)
end
