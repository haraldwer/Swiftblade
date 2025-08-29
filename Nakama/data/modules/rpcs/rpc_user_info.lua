
nk.logger_info("RPC_User_Info loaded")

local function rpc_get_user_info(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local user = p.userID
	local keys = {
		"name",
		"plays",
		"kills",
		"first_login",
	}

	local obj = storage.read_many(user, keys)
	utility.print_table(obj) -- Whats in the response??
	-- parse response and return result

	return nk.json_encode({ 
		["payload"] = obj,
		["success"] = true
	})
end

local function list_user_lb(userID, lb)
	local lbID = userID .. "_" .. lb
	local limit = 25
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

local function rpc_get_user_top_ranks(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	return list_user_lb(p.userID, "top_ranks")
end

local function rpc_get_user_most_played(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	return list_user_lb(p.userID, "most_played")
end

local function rpc_get_user_recent_rooms(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	return list_user_lb(p.userID, "recent_rooms")
end

local function rpc_get_user_recent_levels(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	return list_user_lb(p.userID, "recent_levels")
end

local function rpc_get_user_favourites(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)

	local user = p.userID
	local obj = storage.read(user, "favourites")
	utility.print_table(obj) -- Whats in the response??
	-- parse response and return result
	
	return nk.json_encode({
		["payload"] = obj,
		["success"] = true
	})
end

local function rpc_get_user_submissions(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)

	local user = p.userID
	local obj = storage.read(user, "submissions")
	utility.print_table(obj) -- Whats in the response??
	-- parse response and return result

	return nk.json_encode({
		["payload"] = obj,
		["success"] = true
	})
end

nk.register_rpc(rpc_get_user_info, "rpc_get_user_info")
nk.register_rpc(rpc_get_user_top_ranks, "rpc_get_user_top_ranks")
nk.register_rpc(rpc_get_user_most_played, "rpc_get_user_most_played")
nk.register_rpc(rpc_get_user_recent_rooms, "rpc_get_user_recent_rooms")
nk.register_rpc(rpc_get_user_recent_levels, "rpc_get_user_recent_levels")
nk.register_rpc(rpc_get_user_favourites, "rpc_get_user_favourites")
nk.register_rpc(rpc_get_user_submissions, "rpc_get_user_submissions")
