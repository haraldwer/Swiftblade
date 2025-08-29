
nk.logger_info("Storage loaded")

local function storage_read(collection, key)
	local object_ids = { { collection = collection, key = key } }
	local objects = nk.storage_read(object_ids)
	for _, r in ipairs(objects) do
		return r.value
	end
	nk.logger_error("Failed to read storage: " .. collection .. " - " .. key)
	return nil
end

local function storage_read_many(collection, keys)
	local obj = {}
	for _, value in ipairs(keys) do
		table.insert(obj, { collection = collection, key = value })
	end
	local objects = nk.storage_read(obj)
	if (not objects == nil) then
		return objects
	end
	nk.logger_error("Failed to read storage: " .. collection)
	return nil
end

local function storage_write(collection, key, value)
	local obj = {{ collection = collection, key = key, value = value }}
	local acks, err = nk.storage_write(obj)
	if (not err == nil) then
		nk.logger_error("Failed to write storage: " .. collection .. " - " .. key .. " - " .. value .. " (" .. err .. ")")
	end
end

local function storage_write_many(collection, pairs)
	local obj = {}
	for key, value in ipairs(pairs) do
		table.insert(obj, { collection = collection, key = key, value = value })
	end
	local acks, err = nk.storage_write(obj)
	if (not err == nil) then
		nk.logger_error("Failed to write storage: " .. collection .. " (" .. err .. ")")
	end
end

local function storage_change(collection, key, delta)
	local val_obj = storage_read(collection, key)
	local val = 0
	if (type(val_obj) == "number") then
		val = val_obj
	end
	val = val + delta
	storage_write(collection, key, val)
	return val
end

return {
	read = storage_read,
	read_many = storage_read_many,
	write = storage_write,
	write_many = storage_write_many,
	change = storage_change
}