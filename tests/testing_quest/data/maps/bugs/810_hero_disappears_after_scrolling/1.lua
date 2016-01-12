local map = ...

function map:on_opening_transition_finished()

  hero:teleport("bugs/810_hero_disappears_after_scrolling/2", "_side0", "immediate")
end
