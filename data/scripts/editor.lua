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

editor = {}
editor.state = nil
editor.level = nil
editor.bgMusic = nil

function editorMain(lvl, state)
    bindEditorInput(state)

    editor.state = state
    editor.level = lvl

    lvl.physWorld:threads(4)
    -- if we use 1 thread, the simulation is 100% deterministic
    -- ... can be quite useful for some time-based levels

    --editor.bgMusic = level:soundSys():play("media/music/THA - Faculty (writers block).mp3", false)
    --editor.bgMusic:loop(-1) -- loop forever

end

function createEditorGUI()

    local sheetSize = editor.level.guiSheet.size

    local crystal = gui.skin("crystal")
    editor.level.guiSheet:setSkin(crystal)

    local function button(tex, desc, callback)
        return {
            type = "panel",
            size = vec2(32, 32),
            draggable = false,
            decorate = false,
            tip = desc,
            texture = "button_" .. tex .. "_small.png",
            onClick = callback
        }
    end

    local function separator()
        return {
            type = "panel",
            size = vec2(16, 32),
            draggable = false,
            decorate = false,
            clickable = false,
            texture = "gui/separator_vertical_small.png"
        }
    end

    local function buttons(tbl)
        local pos = vec2(0, 0)
        for key, val in pairs(tbl) do
            for key2, val2 in pairs(val) do
                val2.position = vec2(pos.x, pos.y)
                pos.x = pos.x + val2.size.x
            end
        end
        return tbl
    end

    local function toggleVisible(wn)
        local w = editor.level.guiSheet:child(wn)
        if w ~= nil then w.visible = not w.visible end
    end

    editor.level.guiSheet:update({

        fileDialog = {
            type = "fileDialog",
            size = vec2(400, 500),
            position = vec2(200, 200),
            visible = false
        },

        buttonsBar0 = {
            type = "panel",
            size = vec2(32 * 16, 32),
            draggable = false,
--            texture = "gui/back_light.png",
            buttons({
                { fileNew = button("file_new", "new", function()
                    editor.state:clear()
                end) },
                { fileOpen = button("file_open", "open", function()
                    -- retrieve the file dialog
                    local fd = editor.level.guiSheet:child("fileDialog")
                    -- remove any previous onConfirm callback
                    fd:removeCallback("onConfirm")
                    fd:update({
                        -- add a new callback that loads the level
                        onConfirm = function(caller, final)
                            editor.state:loadLevel(final)
                            -- hide when done
                            caller.visible = false
                        end,
                        -- initial directory should be the levels directory
                        directory = DATA_DIR .. "levels/",
                        -- make the file dialog visible
                        visible = true,
                        -- start with no input
                        input = {
                            text = ""
                        }
                    })
                end) },
                { fileSave = button("file_save", "save", function()
                    -- retrieve the file dialog
                    local fd = editor.level.guiSheet:child("fileDialog")
                    -- remove any previous onConfirm callback
                    fd:removeCallback("onConfirm")
                    fd:update({
                        -- add a new callback that saves the level
                        onConfirm = function(caller, final)
                            editor.state:saveLevel(final)
                            -- hide when done
                            caller.visible = false
                        end,
                        -- initial directory should be the levels directory
                        directory = DATA_DIR .. "levels/",
                        -- make the file dialog visible
                        visible = true,
                        -- start with no input
                        input = {
                            text = ""
                        }
                    })
                end) },
                { props = button("window_props", "props", function()
                    -- retrieve the file dialog
                    local fd = editor.level.guiSheet:child("fileDialog")
                    -- remove any previous onConfirm callback
                    fd:removeCallback("onConfirm")
                    fd:update({
                        -- add a new callback that loads the level
                        onConfirm = function(caller, final)
                            editor.state:createSpawnEnt(final)
                            -- hide when done
                            caller.visible = false
                        end,
                        -- initial directory should be the levels directory
                        directory = DATA_DIR,
                        -- make the file dialog visible
                        visible = true,
                        -- start with no input
                        input = {
                            text = ""
                        }
                    })
                end) },
                { separator1 = separator() },
                { undo = button("undo", "undo", function() editor.state:undo() end) },
                { redo = button("redo", "redo", function() editor.state:redo() end) },
                { separator2 = separator() },
                --{ select = button("tool_select", "select") },
                --{ selectFromList = button("selectfromlist", "select from list") },
                { move = button("tool_move", "move", function() editor.state:gizmoType("translate") end) },
                { rotate = button("tool_rotate", "rotate", function() editor.state:gizmoType("rotate") end) },
                { scale = button("tool_scale", "scale", function() editor.state:gizmoType("scale") end) },
                { separator3 = separator() },
                { referencePivotCenter = button("reference_pivotcenter", "pivot individually", function()
                    editor.state:rotateAsGroup(false)
                end) },
                { referenceSelectionCenter = button("reference_selectioncenter", "pivot as group", function()
                    editor.state:rotateAsGroup(true)
                end) },
                { separator4 = separator() },
--                { run = button("simulation_run", function() editor.actions:go("startSimulation") end) },
                { startstop = button("simulation_startstop", "toggle simulation", function()
                    editor.state:toggleSimulation()
--                    if editor.bgMusic:paused() then
--                        editor.bgMusic:resume()
--                    else
--                        editor.bgMusic:pause()
--                    end
                end) },
                { launch = button("launchgame", "launch", function()
                    editor.state:play()
                    --editor.bgMusic:pause()
                end) },
                { console = button("window_console", "console", function() toggleVisible("console") end) }
            })
        },

--        buttonsBar1 = {
--            type = "panel",
--            size = vec2(32 * 7, 32),
--            position = vec2(0, 64),
--            draggable = false,
----            texture = "gui/back_light.png",
--            buttons({
--                { props = button("window_props", "props", function() end) },
--                { levels = button("window_levels", "levels", function() end) },
--                { properties = button("window_properties", "properties", function() end) },
--                { lights = button("window_lights", "lights", function() end) },
--                { triggers = button("window_triggers", "triggers", function() end) },
--                { debug = button("window_debug", "debug", function() end) },
--                { console = button("window_console", "console", function() toggleVisible("console") end) }
--            })
--        },

        timeSpeedScrollbarWin = {
            type = "window",
            position = vec2(0, sheetSize.y * 0.75),
            size = vec2(256, 48),
            resizable = false,
            closable = false,
            title = "time speed",
            timeSpeedScrollbar = {
                type = "scrollbar",
                percent = 1
            }
        },

--        mouseSpeedScrollbarWin = {
--            type = "window",
--            position = vec2(sheetSize.x - 128, sheetSize.y * 0.75),
--            size = vec2(128, 16),
--            resizable = false,
--            closable = false,
--            title = "mouse speed",
--            mouseSpeedScrollbar = {
--                type = "scrollbar",
--                points = vec4(0, 0, 112, 0),
--                draggable = false
----                { percent = 100 }
--            }
--        },

        spawnTypeDropDownMenu = {
            type = "dropDownMenu",
            draggable = false,
            adjustedSize = vec2(200, 1),
            position = vec2(sheetSize.x - 250 , 16),
            { addChoice = "generic" },
            { addChoice = "portal" },
            { addChoice = "trigger" },
            { addChoice = "marker" },
            onSelect = function(caller, str)
                --editor.state:selectSpawnType(str)
                local type = game.entity.GENERIC_PROP
                if str == "portal" then type = game.entity.PORTAL
                elseif str == "trigger" then type = game.entity.TRIGGER
                elseif str == "marker" then type = game.entity.MARKER
                end
                editor.state:selectSpawnType(type)
            end
        },

        propertiesWin = {
            type = "window",
            position = vec2(300, 200),
            size = vec2(300, 500),
            resizable = false,
            title = "object properties",
            visible = false
        },

        levelPropertiesWin = {
            type = "window",
            position = vec2(300, 200),
            size = vec2(300, 500),
            resizable = false,
            title = "level properties",
            visible = false
        },

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

    local function createLevelPropertiesWin()
        local win = editor.level.guiSheet:child("levelPropertiesWin")
        win:resetLayout()
        win:clearChildren(false)

        local lay = gui.verticalLayout()
        win:setLayout(lay)

        win:update({
            scriptLabel = {
                type = "text",
                text = "script:"
            },
            scriptValue = {
                type = "textBox",
                text = editor.level.script,
                onEnter = function(caller, val)
                    editor.level.script = val
                end
            }
        })

        lay:addWidget(win:child("scriptLabel"))
        lay:addWidget(win:child("scriptValue"))
    end
    createLevelPropertiesWin()

    local function addToggleKey(k, wn)
        editor.state:bind(k, function()
            local w = editor.level.guiSheet:child(wn)
            if w ~= nil then w.visible = not w.visible end
        end)
    end

    addToggleKey("+KC_F1", "console")
    addToggleKey("+KC_F2", "propertiesWin")
    addToggleKey("+KC_F3", "levelPropertiesWin")

end

function limitDecimals(x, numDecimals)
    local coef = math.pow(10, numDecimals)
    return math.floor(x * coef) / coef
end

--updateFPSAccum = 0
---- deltaTime can go backwards (negative), realDeltaTime is the "real" time
--function tick(deltaTime, realDeltaTime)
--    updateFPSAccum = updateFPSAccum + realDeltaTime
--    if updateFPSAccum > 1 then
--        local fps = 1.0 / realDeltaTime
--
--        local fpsLabel = editor.guiSheet:child("fpsLabel")
--
--        if fpsLabel.__ok then -- __ok is bound by luabind to tell if a shared_ptr is not NULL
--            fpsLabel:update({ text = "fps: " .. limitDecimals(fps, 1) })
--        end
--
--        updateFPSAccum = 0
--    end
--end

function updatePropertiesWinGenericProp(win, pickedEnt)
    if pickedEnt == nil then
        return
    end

    local ent = game.toGenericProp(pickedEnt)

    local lay = win:getLayout()

    win:update({
        posXText = {
            type = "text",
            text = "x: " .. limitDecimals(ent:pos().x, 3)
        },
        posYText = {
            type = "text",
            text = "y: " .. limitDecimals(ent:pos().y, 3)
        },
        posZText = {
            type = "text",
            text = "z: " .. limitDecimals(ent:pos().z, 3)
        },

        massLabel = {
            type = "text",
            text = "mass:"
        },
        massValue = {
            type = "textBox",
            text = tostring(limitDecimals(ent:mass(), 3)),
            onEnter = function(caller, val)
                ent:mass(tonumber(val))
                ent:setCurrentStateAsInitial()
            end
        },

        scaleLabel = {
            type = "text",
            text = "scale:"
        },
        scaleXValue = {
            type = "textBox",
            text = tostring(limitDecimals(ent:scale().x, 3)),
            onEnter = function(caller, val)
                local s = ent:scale()
                s.x = tonumber(val)
                ent:scale(s)
                ent:setCurrentStateAsInitial()
            end
        },
        scaleYValue = {
            type = "textBox",
            text = tostring(limitDecimals(ent:scale().y, 3)),
            onEnter = function(caller, val)
                local s = ent:scale()
                s.y = tonumber(val)
                ent:scale(s)
                ent:setCurrentStateAsInitial()
            end
        },
        scaleZValue = {
            type = "textBox",
            text = tostring(limitDecimals(ent:scale().z, 3)),
            onEnter = function(caller, val)
                local s = ent:scale()
                s.z = tonumber(val)
                ent:scale(s)
                ent:setCurrentStateAsInitial()
            end
        },

        collidableLabel = {
            type = "text",
            text = "collidable:"
        },
        collidableValue = {
            type = "checkBox",
            check = ent:collidable(),
            onCheck = function(caller, val)
                ent:collidable(val)
                ent:setCurrentStateAsInitial()
            end
        }

--        activeLabel = {
--            type = "text",
--            text = "active:"
--        },
--        activeValue = {
--            type = "checkBox",
--            check = ent:active(),
--            onCheck = function(caller, val)
--                ent:active(val)
--                ent:setCurrentStateAsInitial()
--            end
--        }
    })

    lay:addWidget(win:child("posXText"))
    lay:addWidget(win:child("posYText"))
    lay:addWidget(win:child("posZText"))

    lay:addWidget(win:child("massLabel"))
    lay:addWidget(win:child("massValue"))

    lay:addWidget(win:child("scaleLabel"))
    lay:addWidget(win:child("scaleXValue"))
    lay:addWidget(win:child("scaleYValue"))
    lay:addWidget(win:child("scaleZValue"))

    lay:addWidget(win:child("collidableLabel"))
    lay:addWidget(win:child("collidableValue"))

--    lay:addWidget(win:child("activeLabel"))
--    lay:addWidget(win:child("activeValue"))
end

function updatePropertiesWinPortal(win, pickedEnt)
    if pickedEnt == nil then
        return
    end

    updatePropertiesWinGenericProp(win, pickedEnt)

    local ent = game.toPortal(pickedEnt)

    local lay = win:getLayout()

    win:update({
        connectionLabel = {
            type = "text",
            text = "connection:"
        },
        connectionValue = {
            type = "textBox",
            text = ent:connectionName(),
            onEnter = function(caller, val)
                local p = editor.level.gameWorld:get(val)
                ent:connection(game.toPortal(p))
                ent:setCurrentStateAsInitial()
            end
        }
    })

    lay:addWidget(win:child("connectionLabel"))
    lay:addWidget(win:child("connectionValue"))
end


function updatePropertiesWinTrigger(win, pickedEnt)
    if pickedEnt == nil then
        return
    end

    updatePropertiesWinGenericProp(win, pickedEnt)

    local ent = game.toTrigger(pickedEnt)

    local lay = win:getLayout()

    win:update({
        onEnterLabel = {
            type = "text",
            text = "onEnter:"
        },
        onEnterValue = {
            type = "textBox",
            text = ent.onEnter,
            onEnter = function(caller, val)
                ent.onEnter = val
                ent:setCurrentStateAsInitial()
            end
        },
        onExitLabel = {
            type = "text",
            text = "onExit:"
        },
        onExitValue = {
            type = "textBox",
            text = ent.onExit,
            onEnter = function(caller, val)
                ent.onExit = val
                ent:setCurrentStateAsInitial()
            end
        }
    })

    lay:addWidget(win:child("onEnterLabel"))
    lay:addWidget(win:child("onEnterValue"))
    lay:addWidget(win:child("onExitLabel"))
    lay:addWidget(win:child("onExitValue"))
end

function updatePropertiesWinMarker(win, pickedEnt)
    if pickedEnt == nil then
        return
    end

    updatePropertiesWinGenericProp(win, pickedEnt)

    local ent = game.toMarker(pickedEnt)

    local lay = win:getLayout()

end

updatePropertiesWinFuncs = {}
updatePropertiesWinFuncs[game.entity.GENERIC_PROP] = updatePropertiesWinGenericProp
updatePropertiesWinFuncs[game.entity.PORTAL] = updatePropertiesWinPortal
updatePropertiesWinFuncs[game.entity.TRIGGER] = updatePropertiesWinTrigger
updatePropertiesWinFuncs[game.entity.MARKER] = updatePropertiesWinMarker

stringTypes = {}
stringTypes[game.entity.GENERIC_PROP] = "genericProp"
stringTypes[game.entity.PORTAL] = "portal"
stringTypes[game.entity.TRIGGER] = "trigger"
stringTypes[game.entity.MARKER] = "marker"

function updatePropertiesWin(ent)

    local win = editor.level.guiSheet:child("propertiesWin")
    win:resetLayout()
    win:clearChildren(false)

    local lay = gui.verticalLayout()
    win:setLayout(lay)

    win:update({
        nameLabel = {
            type = "text",
            text = "name:"
        },
        nameValue = {
            type = "textBox",
            text = ent:name(),
            onEnter = function(caller, val)
                ent:name(val)
                ent:setCurrentStateAsInitial()
            end
        },
        typeLabel = {
            type = "text",
            text = "type: " .. stringTypes[ent:type()]
        }
    })

    lay:addWidget(win:child("nameLabel"))
    lay:addWidget(win:child("nameValue"))
    lay:addWidget(win:child("typeLabel"))

    updatePropertiesWinFuncs[ent:type()](win, ent)
end

function updateLevelPropertiesWin(script)
    editor.level.guiSheet:child("levelPropertiesWin"):child("scriptValue"):update({
        text = script
    })
end

--function testOnEnter(trigger, ent)
--    trigger:setMaterial("red")
--    collectgarbage("collect")
--end
--
--function testOnExit(trigger, ent)
--    trigger:setMaterial("green")
--    collectgarbage("collect")
--end





