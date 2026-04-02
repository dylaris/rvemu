-- Usage: lua caesar_cipher.lua <KEY> <FILE> <MODE>

local function isalpha(c)
    return (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z')
end

local function read_file(filename)
    local file = assert(io.open(filename, "r"))
    local raw_text = file:read("a")
    io.close(file)
    local text = {}
    for i = 1, #raw_text do
        text[#text+1] = raw_text:sub(i, i)
    end
    return text
end

local function encode(text, key)
    for i = 1, #text do
        if isalpha(text[i]) then
            text[i] = string.char(string.byte(text[i]) + key)
        end
    end
    return table.concat(text)
end

local function decode(text, key)
    for i = 1, #text do
        if isalpha(text[i]) then
            text[i] = string.char(string.byte(text[i]) - key)
        end
    end
    return table.concat(text)
end

local function check_key(key)
    if type(key) ~= "number" then return false end
    if key < 1 or key > 26 then return false end
    return true
end

local function check_mode(mode)
    if type(mode) ~= "string" then return false end
    if mode ~= "+" and mode ~= "-" then return false end
    return true
end

local function usage()
    io.stderr:write[[
Usage: lua caesar_cipher.lua <FILE> <KEY> <MODE>
    key (1-26)
    mode (+ encode, - decode)
]]
end

if #arg ~= 3 then
    usage()
else
    local filename = arg[1]
    local key = tonumber(arg[2])
    local mode = arg[3]

    if not check_key(key) or not check_mode(mode) then
        usage()
        return
    end

    local text = read_file(filename)
    if mode == "+" then
        io.stdout:write(encode(text, key))
    else
        io.stdout:write(decode(text, key))
    end
end
