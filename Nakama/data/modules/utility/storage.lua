
nk.logger_info("Storage loaded")

local function storage_read(collection, key)
	if (collection == "" or collection == nil) then
		nk.logger_error("Unknown collection")
		return nil
	end
	local object_ids = { { collection = collection, key = key } }
	local objects = nk.storage_read(object_ids)
	if (objects == nil or not error == nil) then
		nk.logger_error("Failed to read storage: " .. collection .. " - " .. key .. " (" .. error .. ")")
		return nil
	end
	for _, r in ipairs(objects) do
		utility.print_table(r)
		if r.value ~= nil then
			return r.value
		end
	end
	nk.logger_error("Failed to read storage: " .. collection .. " - " .. key)
	return nil
end

local function storage_read_many(collection, keys)
	if (collection == "" or collection == nil) then
		nk.logger_error("Unknown collection")
		return nil
	end
	local obj = {}
	for _, value in ipairs(keys) do
		table.insert(obj, { collection = collection, key = value })
	end
	local objects, error = nk.storage_read(obj)
	if (objects == nil or not error == nil) then
		nk.logger_error("Failed to read storage: " .. collection .. " (" .. error .. ")")
		return nil
	end

	local result = {}
	for _, obj in ipairs(objects) do
		result[obj.key] = obj.value
	end
	return result
end

local function storage_write(collection, key, value)
	if (collection == "" or collection == nil) then
		nk.logger_error("Unknown collection")
		return false
	end
	local obj = {{ collection = collection, key = key, value = value, permission_read = 1, permission_write = 1 }}
	local acks, err = nk.storage_write(obj)
	if (not err == nil) then
		nk.logger_error("Failed to write storage: " .. collection .. " - " .. key .. " - " .. value .. " (" .. err .. ")")
		return false
	end
	return true
end

local function storage_write_many(collection, pairs)
	if (collection == "" or collection == nil) then
		nk.logger_error("Unknown collection")
		return false
	end
	local obj = {}
	utility.print_table(pairs)
	local any_written = false
	for i, e in ipairs(pairs) do
		if e.key ~= nil and e.value ~= nil then
			table.insert(obj, { collection = collection, key = e.key, value = e.value, permission_read = 1, permission_write = 1 })
			any_written = true
		end
	end
	utility.print_table(obj)
	local acks, err = nk.storage_write(obj)
	if (not err == nil) then
		nk.logger_error("Failed to write storage: " .. collection .. " (" .. err .. ")")
		return false
	end
	return any_written
end

local function storage_change(collection, key, delta)
	if (collection == "" or collection == nil) then
		nk.logger_error("Unknown collection")
		return nil
	end
	local val_obj = storage_read(collection, key)
	local val = 0
	if (type(val_obj) == "number") then
		val = val_obj
	end
	val = val + delta
	storage_write(collection, key, val)
	return val
end

local function storage_remove(collection, key)
	if (collection == "" or collection == nil) then
		nk.logger_error("Unknown collection")
		return nil
	end
	local object_ids = {{ collection = collection, key = key }}
	nk.storage_delete(object_ids)
end

return {
	read = storage_read,
	read_many = storage_read_many,
	write = storage_write,
	write_many = storage_write_many,
	change = storage_change
}