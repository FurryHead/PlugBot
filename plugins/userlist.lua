local users = {}

function handle_quit(user, message)
    for name,user_list in pairs(users) do
        new_users = {}
        for _, usr in ipairs(user_list) do
            if usr ~= user[1] then
                table.insert(new_users, #new_users+1, usr)
            end
        end
        users[channel] = new_users
    end
end

function handle_join(channel, user)
    if users[channel] == nil then users[channel] = {} end
    for _, usr in ipairs(users[channel]) do
        if usr == user[1] then
            return
        end
    end
    table.insert(users[channel], #users[channel]+1, user[1])
end

function handle_part(channel, user, message)
    if users[channel] == nil then users[channel] = {} return end
    new_users = {}
    for _, usr in ipairs(users[channel]) do
        if usr ~= user[1] then
            table.insert(new_users, #new_users+1, usr)
        end
    end
    users[channel] = new_users
end

function handle_kick(channel, user, message)
    if users[channel] == nil then users[channel] = {} return end
    new_users = {}
    for _, usr in ipairs(users[channel]) do
        if usr ~= user[1] then
            table.insert(new_users, #new_users+1, usr)
        end
    end
    users[channel] = new_users
end

function handle_nick(oldnick, newnick)
    for name,user_list in pairs(users) do
        for _, user in ipairs(user_list) do
            if oldnick[1] == user then
                users[channel][oldnick[1]] = newnick
            end
        end
    end
end

function handle_353(channel, user_list) 
    if users[channel] == nil then users[channel] = {} end
    for _,v in ipairs(user_list) do
        table.insert(users[channel], #users[channel]+1, v)
    end
end
