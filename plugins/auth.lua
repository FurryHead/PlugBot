depends = {"userlist"}

local owners = {}
local admins = {}
local mods = {}

local authfile = server:config("confdir") .. "auth.cfg"

local owner_hosts = {}
local admin_hosts = {}
local mod_hosts = {}

function startup() 
    local cp = ConfigParser()
    local err = cp:read(authfile)
    if err == "Unable to open "..authfile then
        onick = server:config("owner_nick"):lower()
        owners[onick] = ""
    elseif err then
        error(err)
    end
    
    local network = server:config("network")
    if cp:has_section(network) then
        if cp:has_option(network, "owners") then
            for user,pass in pairs(cp:get(network,"owners"):split(",")) do
                if user ~= "" then
                    owners[user:lower()] = pass
                end
            end
        end
        if cp:has_option(network, "admins") then
            for user,pass in pairs(cp:get(network,"admins"):split(",")) do
                if user ~= "" then
                    admins[user:lower()] = pass
                end
            end
        end
        if cp:has_option(network, "mods") then
            for user,pass in pairs(cp:get(network,"mods"):split(",")) do
                if user ~= "" then
                    mods[user:lower()] = pass
                end
            end
        end
    else
        onick = server:config("owner_nick"):lower()
        owners[onick] = ""
    end
end

function cleanup()
    local cp = ConfigParser()
    local err = cp:read(authfile)
    if err and err ~= "Unable to open "..authfile then
        error(err)
    end
    local network = server:config("network")
    if not cp:has_section(network) then 
        cp:add_section(network)
    end
    
    local ostr = ""
    for k,v in owners do
        ostr = ostr..k..":"..v..","
    end
    cp:set(network, "owners", ostr)
    
    local astr = ""
    for k,v in admins do
        astr = astr..k..":"..v..","
    end
    cp:set(network, "admins", astr)
    
    local mstr = ""
    for k,v in mods do
        mstr = mstr..k..":"..v..","
    end
    cp:set(network, "mods", mstr)
end

function handle_part(channel, user, message)
    for k,v in pairs(owners) do
        if user[1]:lower() == k then
            return
        end
    end
    for k,v in pairs(admins) do
        if user[1]:lower() == k then
            return
        end
    end
    for k,v in pairs(mods) do
        if user[1]:lower() == k then
            return
        end
    end
    
    local new_owners = {}
    for k,v in pairs(owner_hosts) do
        if v ~= user[3] then 
            new_owners[k] = v
        end 
    end
    owner_hosts = new_owners
    
    local new_admins = {}
    for k,v in pairs(admin_hosts) do
        if v ~= user[3] then 
            new_admins[k] = v
        end 
    end
    admin_hosts = new_admins
    
    local new_mods = {}
    for k,v in pairs(mod_hosts) do
        if v ~= user[3] then 
            new_mods[k] = v
        end 
    end
    mod_hosts = new_mods
end

function handle_quit(channel, user, message)
    local new_owners = {}
    for k,v in pairs(owner_hosts) do
        if v ~= user[3] then 
            new_owners[k] = v
        end 
    end
    owner_hosts = new_owners
    
    local new_admins = {}
    for k,v in pairs(admin_hosts) do
        if v ~= user[3] then 
            new_admins[k] = v
        end 
    end
    admin_hosts = new_admins
    
    local new_mods = {}
    for k,v in pairs(mod_hosts) do
        if v ~= user[3] then 
            new_mods[k] = v
        end 
    end
    mod_hosts = new_mods
end

function handle_message(channel, user, message)
    if channel == user[1] then
        if message:split(" ")[1] == "identify" then
            args = {}
            for i,v in ipairs(message:split(" ")) do if i ~= 1 and v ~= "" then args[i] = v end end
            handle_command(channel, user, "identify", args)
        end
    end
end

function handle_command(channel, user, cmd, args) 
    if cmd == "identify" then
        if channel ~= user[1] then
            server:msg(channel, user[1]..": Just so you know, you can do that using private messaging too. (hint, hint)")
        end
        
        local pswd
        if #args < 1 then
            pswd = ""
        else
            pswd = args[1]
        end
        
        local luser = user[1]:lower()
        
        for usr,pass in pairs(owners) do
            if luser == usr then
                if pswd == pass then
                    owner_hosts[luser] = user[3]
                    server:msg(channel, user[1]..": Sucessfully identified. (You now have owner access)")
                else
                    server:msg(channel, user[1]..": Invalid password.")
                end
                return
            end
        end
        
        for usr,pass in pairs(admins) do
            if luser == usr then
                if pswd == pass then
                    admin_hosts[luser] = user[3]
                    server:msg(channel, user[1]..": Sucessfully identified. (You now have admin access)")
                else
                    server:msg(channel, user[1]..": Invalid password.")
                end
                return
            end
        end
        
        for usr,pass in pairs(mods) do
            if luser == usr then
                if pswd == pass then
                    mod_hosts[luser] = user[3]
                    server:msg(channel, user[1]..": Sucessfully identified. (You now have mod access)")
                else
                    server:msg(channel, user[1]..": Invalid password.")
                end
                return
            end
        end
        
        server:msg(channel, user[1]..": You are not an owner/admin/mod.")
    end
end

function isOwner(user)
    for k,v in pairs(owner_hosts) do
        if user[3] == v then
            return true
        end
    end
    return false
end

function isAdmin(user)
    for k,v in pairs(admin_hosts) do
        if user[3] == v then
            return true
        end
    end
    return isOwner(user)
end

function isMod(user)
    for k,v in pairs(mod_hosts) do
        if user[3] == v then
            return true
        end
    end
    if isAdmin(user) then
        return true
    else
        return isOwner(user)
    end
end

-- Copied from online.
function string:split(sep)
        local sep, fields = sep or " ", {}
        local pattern = string.format("([^%s]+)", sep)
        self:gsub(pattern, function(c) fields[#fields+1] = c end)
        return fields
end
