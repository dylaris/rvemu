-- Usage: lua count_words.lua <FILE>

local function count_words(text)
    local words = {}

    for w in text:gmatch("%w+") do
        lw = w:lower()
        words[lw] = (words[lw] or 0) + 1
    end

    return words
end

local function sort_words(words)
    local sorted_words = {}

    for w, count in pairs(words) do
        table.insert(sorted_words, {key = w, value = count})
    end

    table.sort(sorted_words, function (a, b)
        return a.value > b.value
    end)

    return sorted_words
end

local function count_file_words(filename)
    local file = assert(io.open(filename, "r"))
    local words = count_words(file:read("a"))
    return words
end

local function draw_words(words, sorted)
    if sorted then
        for idx, pair in ipairs(words) do
            print(string.format("[%04d] %-15s (%04d) %s", idx, pair.key, pair.value, string.rep("+", pair.value)))
        end
    else
        for w, count in pairs(words) do
            print(string.format("%-15s (%04d) %s", w, count, string.rep("+", count)))
        end
    end
end

local function main()
    local filename = arg[1]
    if not filename then return end

    local words = count_file_words(filename)
    words = sort_words(words)
    draw_words(words, true)
end

main()
