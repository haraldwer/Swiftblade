
nk.logger_info("RPC_Level_List loaded")

local function list_lb(lbID)
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

local function rpc_get_submissions(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	return list_lb("submissions")
end

local function rpc_get_most_played(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	return list_lb("most_played")
end

local function rpc_get_most_favorites(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	return list_lb("most_favourites")
end

nk.register_rpc(rpc_get_submissions, "rpc_get_submissions")
nk.register_rpc(rpc_get_most_played, "rpc_get_most_played")
nk.register_rpc(rpc_get_most_favorites, "rpc_get_most_favorites")