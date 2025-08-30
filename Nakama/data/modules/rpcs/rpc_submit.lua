
nk.logger_info("RPC_Submit loaded")

local function rpc_submit_room(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)

	local roomID = "room_" .. nk.uuid_v4() -- generate id
	if (not storage.read(roomID, "name") == nil) then
		nk.logger_error("Room submission failed, ID already exists " .. roomID)
		return nk.json_encode({
			["payload"] = {},
			["success"] = false
		})
	end

	local data = {
		{ key = "name", value = p.name },
		{ key = "creator", value = c.user_id },
		{ key = "data", value = p.data },
		{ key = "size", value = p.size },
		{ key = "enemies", value = p.enemies },
		{ key = "status", value = "Pending" },
		{ key = "date", value = p.date },
		{ key = "favourites", value = 0 },
		{ key = "playcount", value = 0 },
	}
	storage.write_many(roomID, data)

	return nk.json_encode({
		["payload"] = { ["roomID"] = roomID },
		["success"] = true
	})
end

local function rpc_submit_level(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)

	local levelID = "level" .. nk.uuid_v4 -- generate id
	if (not storage.read(levelID, "name") == nil) then
		nk.logger_error("Level submission failed, ID already exists " .. levelID)
		return nk.json_encode({
			["payload"] = {},
			["success"] = false
		})
	end

	local data = {
		{ key = "name", value = p.name },
		{ key = "hash", value = p.hash },
		{ key = "rooms", value = p.rooms },
		{ key = "status", value = "Pending" },
		{ key = "date", value = p.date },
		{ key = "creator", value = c.user_id },
		{ key = "favourites", value = 0 },
		{ key = "playcount", value = 0 },
		{ key = "finishcount", value = 0 },
	}
	storage.write_many(levelID, data)

	return nk.json_encode({
		["payload"] = { ["levelID"] = levelID },
		["success"] = true
	})
end

nk.register_rpc(rpc_submit_room, "rpc_submit_room")
nk.register_rpc(rpc_submit_level, "rpc_submit_level")