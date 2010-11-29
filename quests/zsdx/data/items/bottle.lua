-- This script handles all bottles (each bottle script includes it)

function event_use()
  print("I am a bottle!")
  sol.item.set_finished()
end

