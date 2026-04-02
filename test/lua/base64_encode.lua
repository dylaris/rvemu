-- Usage: lua base64_encode.lua <FILE>

local map = {}
local fill_char = '='

local function build_map()
    for i = 0, 25 do
        map[i]    = string.char(string.byte('A') + i)
        map[i+26] = string.char(string.byte('a') + i)
    end
    for i = 0, 9 do map[52+i] = i end
    map[62] = '+'
    map[63] = '/'

    -- for i = 0, 63 do print(map[i]) end
end

local function base64_encode1(c1)
    --[[
        8bit: [AAAA AAAA]
        6bit: [AAAAAA] [AA0000] [000000] [000000]
    --]]
    local res = {}
    table.insert(res, map[(string.byte(c1)>>2)&0x3f])
    table.insert(res, map[(string.byte(c1)<<4)&0x3f])
    table.insert(res, fill_char)
    table.insert(res, fill_char)
    return table.concat(res, "")
end

local function base64_encode2(c1, c2)
    --[[
        8bit: [AAAA AAAA] [BBBB BBBB]
        6bit: [AAAAAA] [AABBBB] [BBBB00] [000000]
    --]]
    local res = {}
    table.insert(res, map[(string.byte(c1)>>2)&0x3f])
    table.insert(res, map[((string.byte(c1)<<4)|(string.byte(c2)>>4))&0x3f])
    table.insert(res, map[(string.byte(c2)<<2)&0x3f])
    table.insert(res, fill_char)
    return table.concat(res, "")
end

local function base64_encode3(c1, c2, c3)
    --[[
        8bit: [AAAA AAAA] [BBBB BBBB] [CCCC CCCC]
        6bit: [AAAAAA] [AaBBBB] [BBBBCC] [CCCCCC]
    --]]
    local res = {}
    table.insert(res, map[(string.byte(c1)>>2)&0x3f])
    table.insert(res, map[((string.byte(c1)<<4)|(string.byte(c2)>>4))&0x3f])
    table.insert(res, map[((string.byte(c2)<<2)|(string.byte(c3)>>6))&0x3f])
    table.insert(res, map[string.byte(c3)&0x3f])
    return table.concat(res, "")
end

local function base64_encode(s)
    local res = {}
    local start, length = 1, #s
    while start <= length do
        local count = length - start
        if count == 0 then
            table.insert(res, base64_encode1(
                s:sub(start, start)
            ))
        elseif count == 1 then
            table.insert(res, base64_encode2(
                s:sub(start, start),
                s:sub(start+1, start+1)
            ))
        else
            table.insert(res, base64_encode3(
                s:sub(start, start),
                s:sub(start+1, start+1),
                s:sub(start+2, start+2)
            ))
        end
        start = start + 3
    end
    return table.concat(res, "")
end

build_map()

local function main()
    local filename = arg[1]
    if not filename then return end

    local file = assert(io.open(filename, "r"))
    local content = file:read("*a")
    file:close()
    print(base64_encode(content))
end

main()
