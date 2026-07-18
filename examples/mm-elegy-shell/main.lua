local ship = require("ship")

local active_statue = nil

local function remove_active_statue()
    if not active_statue then
        return
    end

    local alive = ship.actor.exists(active_statue)
    if alive then
        ship.actor.destroy(active_statue)
    end
    active_statue = nil
end

local function spawn_elegy_statue()
    remove_active_statue()

    local statue, err = ship.actor.spawn("compat.mm.elegy_shell.human", {
        position = { x = 0, y = 0, z = 0 },
        rotation = { x = 0, y = 180, z = 0 },
    })

    if not statue then
        ship.log.warn("Elegy shell spawn failed [" .. err.code .. "]: " .. err.message)
        return
    end

    active_statue = statue
    ship.log.info("Majora's Mask Elegy shell spawned through the OoT actor engine")
end

ship.hotkeys.register("spawn_mm_elegy_shell", {
    default = "K",
    label = "Spawn MM Elegy shell",
}, spawn_elegy_statue)
