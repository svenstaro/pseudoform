level = nil

function levelStart(lvl)
    level = lvl

    log("test_elevator1 level loaded ;)")
end

function playerEnterBottom(trigger, ent)
    if ent:name() == "player" then
        local elevator = level.gameWorld:get("ent_3")
        if not elevator.__ok then
            return
        end
        elevator = game.toGenericProp(elevator)
        elevator:vel(vec3(0, 1, 0))
    end
    collectgarbage("collect")
end

function playerEnterTop(trigger, ent)
    if ent:name() == "player" then
        local elevator = level.gameWorld:get("ent_3")
        if not elevator.__ok then
            return
        end
        elevator = game.toGenericProp(elevator)
        elevator:vel(vec3(0, 0, 0))
    end
    collectgarbage("collect")
end
