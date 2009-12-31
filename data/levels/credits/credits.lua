level = nil

bgMusic = nil

function levelStart(lvl)
    level = lvl

    local function createCredits()
        level.guiSheet:update({
            creditsWin = {
                type = "window",
                title = "credits",
				visible = true,
				size = vec2(400, 400),
                position = vec2(64, 64),
				texture = "gui/credits.png"
            }
        })
    end
    createCredits()

    bgMusic = level:soundSys():play("media/music/THA - Rock This.mp3", false)
    bgMusic:loop(-1)
end

function levelStop(lvl)

    bgMusic:stop()

end
