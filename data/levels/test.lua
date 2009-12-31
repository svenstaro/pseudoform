level = nil

function levelStart(lvl)
    level = lvl

    log("started test level :D")
end

function levelStop(lvl)
    log("stopped test level :(")
end

function winTrigger(trig, ent)
    log("ent " .. ent:name() .. " hit trigger " .. trig:name())
    if ent:name() == "player" then
        level.guiSheet:update({
            winWin = {
                type = "window",
                title = "winner!",
                position = vec2(256, 256),
                winMsg = {
                    type = "text",
                    text = "YOU WIN! :D"
                }
            }
        })
    end
end
