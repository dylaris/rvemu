-- Usage: lua count_words.lua <filename>

local function count_words(text)
  local words = {}

  for word in text:gmatch("[%a']+") do
    lower_word = word:lower()
    words[lower_word] = (words[lower_word] or 0) + 1
  end

  return words
end

local function sort_words(words)
  local sorted_words = {}

  for word, count in pairs(words) do
    table.insert(sorted_words, {key = word, value = count})
  end

  table.sort(sorted_words, function(a, b)
    if a.value ~= b.value then
        return a.value > b.value
    end
    return a.key < b.key
  end)

  return sorted_words
end

local function count_file_words(filename)
  local file, err = io.open(filename, "r")
  assert(file, "cannot open file: " .. tostring(err))

  local text, read_err = file:read("a")
  file:close()
  assert(text, "read failed: " .. tostring(read_err))

  return count_words(text)
end

local function draw_words(words, sorted)
  if sorted then
    for idx, pair in ipairs(words) do
      print(string.format("[%04d] %-15s (%04d) %s",
        idx, pair.key, pair.value, string.rep("+", pair.value)))
    end
  else
    for word, count in pairs(words) do
      print(string.format("%-15s (%04d) %s",
        word, count, string.rep("+", count)))
    end
  end
end

local function main()
  local filename = arg[1]
  if not filename then
    print("Usage: lua count_words.lua <filename>")
    os.exit(1)
  end

  local words = count_file_words(filename)
  local sorted = sort_words(words)
  draw_words(sorted, true)
end

main()
