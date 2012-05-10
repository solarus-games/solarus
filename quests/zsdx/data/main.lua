-- Timer = require 'classes/Timer.lua'

-- ticker, executed every tick
tickers = { }

-- just a collection of static methods
Main = { }

-- special function: called every tick by the engine!
function sol.events.update()
    for k, t in ipairs(Main.tickers) do
        t:update()
    end

end

