
nk.logger_info("RPC_Level_Info loaded")

local function rpc_get_room_info(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local roomID = p.roomID
	local keys = {
		"name",
		"creator",
		"date",
		"favourites",
		"playcount",
	}

	local obj = storage.read_many(roomID, keys)
	utility.print_table(obj) -- Whats in the response??
	return nk.json_encode({ 
		["payload"] = obj,
		["success"] = true
	})
end

local function rpc_get_level_info(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local levelID = p.levelID
	local keys = {
		"name",
		"creator",
		"date",
		"favourites",
		"playcount",
		"finishcount",
		"state"
	}

	local obj = storage.read_many(levelID, keys)
	utility.print_table(obj) -- Whats in the response??
	return nk.json_encode({ 
		["payload"] = obj,
		["success"] = true
	})
end

local function rpc_get_level_leaderboard(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local levelID = p.levelID
	local seed = p.seed

	local lbID = levelID .. "_" .. seed
	local limit = 50
	local records, ownerRecords, nextCursor, prevCursor, error = nk.leaderboard_records_list(lbID, {}, limit, "", 0)

	if (not error == nil) then
		nk.logger_error("LB list failed for " .. lbID .. " : " .. error)
		return
	end

	utility.print_table(records)

	return nk.json_encode({ 
		["payload"] = records,
		["success"] = true
	})
end

nk.register_rpc(rpc_get_room_info, "rpc_get_room_info")
nk.register_rpc(rpc_get_level_info, "rpc_get_level_info")
nk.register_rpc(rpc_get_level_leaderboard, "rpc_get_level_leaderboard")