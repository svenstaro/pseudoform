--function playStart(physWorld)
--    physWorld:threads(4)
--    physWorld:solver(2)
--end

--function playStop()
--    collectgarbage(collect)
--end
--
--function tick(dt)
--end

dofile(DATA_DIR .. "scripts/bind.lua")

play = {
    state = nil,
    level = nil
}

function playMain(level, state)
    play.state = state
    play.level = level

    bindPlayInput(play.state)

    --state:bind("+KC_F11", function() state:exit() end)
    state:bind("+KC_G", function() level.physWorld.gravity.v.y = level.physWorld.gravity.v.y * -1 end)

    play.level.physWorld:threads(4)
    -- if we use 1 thread, the simulation is 100% deterministic
    -- ... can be quite useful for some time-based levels

end

function createPlayGUI()

    local sheetSize = play.level.guiSheet.size

    local crystal = gui.skin("crystal")
    play.level.guiSheet:setSkin(crystal)

    play.level.guiSheet:update({

        console = {
            type = "console",
            size = vec2(500, 500),
            position = vec2(100, 200),
            title = "console",
            listenToLog = true,
            -- whenever something is typed into the console
            -- just parse it as Lua
            onEnter = function(caller, msg)
                local func, err = loadstring(msg)
                if func ~= nil then
                    func()
                else
                    log(err)
                end
            end,
            visible = false
        }

    })

    local function addToggleKey(k, wn)
        play.state:bind(k, function()
            local w = play.level.guiSheet:child(wn)
            if w ~= nil then w.visible = not w.visible end
        end)
    end

    addToggleKey("+KC_F5", "console")

end
