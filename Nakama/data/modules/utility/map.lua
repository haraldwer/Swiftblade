
nk.logger_info("Map loaded")

local function map_add(set, key)
    set[key] = true
end

local function map_remove(set, key)
    set[key] = nil
end

local function map_contains(set, key)
    return set[key] ~= nil
end

return {
	add = map_add,
	remove = map_remove,
	contains = map_contains
}