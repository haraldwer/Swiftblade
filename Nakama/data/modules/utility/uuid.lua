
nk.logger_info("UUID loaded")

-- Create an uuid for a generic string
-- Store uuid -> string for later retreival
local function create_uuid(str)
	local uuid = nk.uuid_v4()
	if not storage.write("uuids", uuid, { ["value"] = str }) then
		nk.logger_error("Failed to write uuid " .. uuid .. " for " .. str)
	end
	return uuid
end

-- Function for getting the value of an uuid
local function get_uuid_value(uuid)
	return storage.read("uuids", uuid).value
end

return {
	create = create_uuid,
	get_value = get_uuid_value
}