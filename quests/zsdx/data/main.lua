-- Timer = require 'classes/Timer.lua'

-- ticker, executed every tick
tickers = { }

-- just a collection of static methods
Main = { }

-- called every tick by the engine!
function Main.update()

    print("hi")

    for t in tickers do
        t:update()
    end

end

print("hi")
