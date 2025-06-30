-- scripts/bird.lua
local gravity = 500.0 -- Pixels per second squared
local jump_force = 200.0 -- Upward velocity on jump

function update(dt)
    -- Apply gravity
    local vel = bird:GetVelocity()
    vel.y = vel.y + gravity * dt
    bird:SetVelocity(vel.x, vel.y)

    -- Update position
    local pos = bird:GetPosition()
    pos.y = pos.y + vel.y * dt
    bird:SetPosition(pos.x, pos.y)

    -- Jump on spacebar
    if space_pressed then
        bird:Jump(jump_force)
    end

    -- Keep bird within screen bounds (optional)
    if pos.y < 0 then
        pos.y = 0
        bird:SetPosition(pos.x, pos.y)
        bird:SetVelocity(vel.x, 0)
    elseif pos.y > 600 - bird:GetSize().y then
        pos.y = 600 - bird:GetSize().y
        bird:SetPosition(pos.x, pos.y)
        bird:SetVelocity(vel.x, 0)
    end
end