local logfh = nil
local logfmt = "%H:%M:%S"
local start_or_stop_fmt = "%m/%d/%y %I:%M:%S %p"
local logfile = server:config("confdir").."logs/"..server:config("network")..".log"
local backlog = { }
local nick = server:config("nick")
local comchar = server:config("comchar")
local MAX_RESULTS = 4


function startup()
    local err
    logfh,err = io.open(logfile, "a")
    if err == (logfile..": No such file or directory") then
        error("Missing logfile directory. Please make a new folder named \"logs\" in the conf directory and try again.")
    elseif err then error(err) end
    logfh:write("====== Start of Log --- "..os.date(start_or_stop_fmt).." ======\n")
    logfh:write("====== Start of Log --- "..os.date(start_or_stop_fmt).." ======\n")
end

function cleanup()
    logfh:write("====== End of Log --- "..os.date(start_or_stop_fmt).." ======\n")
    logfh:close()
end

function log(msg)
    if logfmt then
        msg = "["..os.date(logfmt).."] "..msg
    end
    logfh:write(msg.."\n")
    print(msg)
end

local function add_to_backlog(channel, msg)
    if not backlog[channel] then backlog[channel] = {} end
    if logfmt then
        msg = "["..os.date(logfmt).."] "..msg
    end
    table.insert(backlog[channel], #(backlog[channel])+1, msg)
    if #(backlog[channel]) > 100 then
        table.remove(backlog[channel], 1)
    end
end

function handle_message(channel, user, message) 
    local msg = string.format("<%s> %s: %s", channel, user[1], message)
    
    add_to_backlog(channel, msg)
    log(msg)
end

function handle_mode(channel, user, mode, otheruser)
    local msg
    if otheruser then -- The mode was set onto a user
        msg = string.format("<%s> %s sets mode %s on %s", channel, user[1], mode, otheruser)
    else -- Then the mode was set on the channel
        msg = string.format("<%s> %s sets mode %s", channel, user[1], mode)
    end
    add_to_backlog(channel, msg)
    log(msg)
end

function handle_action(channel, user, action)
    local msg = string.format("<%s> * %s %s", channel, user[1], action)
    add_to_backlog(channel, msg)
    log(msg)
end

function handle_quit(user, message)
    local person
    if user[1] == nick then
        person = "I have"
    else
        person = user[1] .. " has"
    end
    for k,v in pairs(backlog) do
        add_to_backlog(k, string.format("%s quit (%s)", person, message))
    end
    log(string.format("%s quit (%s)", person, message))
end

function handle_join(channel, user)
    local person
    if user[1] == nick then 
        person = "I have"
    else
        person = user[1] .. " has"
    end
    add_to_backlog(channel, string.format("%s joined %s", person, channel))
    log(string.format("%s joined %s", person, channel))
end

function handle_part(channel, user, message)
    local person
    if user[1] == nick then 
        person = "I have"
    else
        person = user[1] .. " has"
    end
    add_to_backlog(channel, string.format("%s left %s (%s)", person, channel, message))
    log(string.format("%s left %s (%s)", person, channel, message))
end

function handle_nick(oldnick, newnick)
    local person
    if oldnick[0] == nick then 
        person = "I am"
    else
        person = oldnick[0] .. " is"
    end
    add_to_backlog(channel, string.format("%s now known as %s", person, newnick))
    log(string.format("%s now known as %s", person, newnick))
end

function string:join(args)
    if #args < 1 then return self end
    if #args == 1 then return args[1] end
    i = 2
    result = args[1]
    for _, v in ipairs(args) do
        if _ > i then
            result = result .. self .. v
        end
    end
    return result
end

function handle_command(channel, user, cmd, args)
    if cmd == "lastlog" then
        local found = {}
        if #args < 1 then 
            server:msg(channel, user[1]..": Not enough arguments.")
            return
        end
        local hitlimit = false
        for i,v in ipairs(backlog[channel]) do
            local temp = string.join(" ", args)
            local m
            if logfmt then
                local ch = channel:gsub('[%-%.%+%[%]%(%)%$%^%%%?%*]','%%%1')
                local t = temp:gsub('[%-%.%+%[%]%(%)%$%^%%%?%*]','%%%1')
                m = string.format("^%%[.+%%] <%s> .+: [^%%%s]*.*%s.*", ch, comchar, t:lower())
            else
                local ch = channel:gsub('[%-%.%+%[%]%(%)%$%^%%%?%*]','%%%1')
                local t = temp:gsub('[%-%.%+%[%]%(%)%$%^%%%?%*]','%%%1')
                m = string.format("^<%s> .+: [^%%%s]*.*%s.*", ch, comchar, t:lower())
            end
            if v:lower():match(m) then
                table.insert(found, #found+1, v)
                if #found >= MAX_RESULTS then 
                    hitlimit = true
                    break 
                end
            end
        end
        local text = string.format("<%s> %s: +%s %s", channel, user[1], cmd, string.join(" ", args))
        if logfmt then text = "["..os.date(logfmt).."] "..text end
        if #found > 1 then
            for i,v in ipairs(found) do
                if v ~= text then
                    server:msg(channel, v)
                end
            end
            if hitlimit then
                server:msg(channel, "(limiting results to maximum of "..MAX_RESULTS..")")
            end
        else
            server:msg(channel, user[1]..": Query not found.")
        end
    end
end
