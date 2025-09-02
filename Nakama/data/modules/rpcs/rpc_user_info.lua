
nk.logger_info("RPC_User_Info loaded")

local function get_user(context, payload)
	local user = payload.User
	if (user == "" or user == nil) then
		user = context.user_id
	end
	return "user_" .. user
end


local function rpc_get_user_info(context, payload)
	local user = get_user(context, payload)
	
	storage.write(user, "stats", { 
		["plays"] = 123123,
		["kills"] = 5123,
	})

	local keys = {
		"info",
		"stats"
	}
	local obj = storage.read_many(user, keys)
	if (obj == nil or obj.info == nil or obj.stats == nil) then
		return nk.json_encode({
			["payload"] = "Failed to read user storage",
			["success"] = false
		})
	end

	local result = {
		["Username"] = obj.info.name,
		["FirstLogin"] = obj.info.first_login,
		["Plays"] = obj.stats.plays,
		["Kills"] = obj.stats.kills,
	}

	return nk.json_encode({
		["payload"] = result,
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

local function rpc_get_user_lb(context, payload)
	local user = get_user(context, payload)
	local lb_map = {
		"top_ranks",
		"most_played",
		"recent_levels",
		"recent_rooms",
		"favourites"
	}
	local lb = lb_map[payload.LB]
	return list_user_lb(user, payload.LB)
end

local function rpc_get_user_recent_rooms(context, payload)
	local user = get_user(context, payload)
	return list_user_lb(user, "recent_rooms")
end

local function rpc_get_user_favourites(context, payload)
	local user = get_user(context, payload)
	local obj = storage.read(user, "favourites")
	utility.print_table(obj) -- Whats in the response??
	-- parse response and return result

	return nk.json_encode({
		["payload"] = obj,
		["success"] = true
	})
end

local function rpc_get_user_submissions(context, payload)
	local user = get_user(context, payload)
	local obj = storage.read(user, "submissions")
	utility.print_table(obj) -- Whats in the response??
	-- parse response and return result

	return nk.json_encode({
		["payload"] = obj,
		["success"] = true
	})
end

nk.register_rpc(rpc_get_user_info, "rpc_get_user_info")
nk.register_rpc(rpc_get_user_lb, "rpc_get_user_lb")
nk.register_rpc(rpc_get_user_recent_rooms, "rpc_get_user_recent_rooms")
nk.register_rpc(rpc_get_user_favourites, "rpc_get_user_favourites")
nk.register_rpc(rpc_get_user_submissions, "rpc_get_user_submissions")
