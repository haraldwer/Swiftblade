
nk.logger_info("RPC_Level_List loaded")

local function list_lb(lbID)
	local limit = 50
	local records, ownerRecords, nextCursor, prevCursor, error = nk.leaderboard_records_list(lbID, {}, limit, "", 0)
	if error ~= nil then
		nk.logger_error("LB list failed for " .. lbID .. " : " .. error)
		return nil
	end
	return records;
end

local function rpc_level_list(context, payload)
	local p = utility.parse(payload)
	local c = utility.parse(context)
	local listName = p.List;

	if listName == "user_current" then
		listName = "user_levels_" .. c.user_id
	end


	local records = list_lb(listName)
	utility.print_table(records)

	-- now fetch data for each level before returning
	-- Name
	-- Creator
	-- Plays
	-- Starred
	-- (depends on what's requested)

	local result = {}

	return nk.json_encode({ 
		["payload"] = result,
		["success"] = true
	})

end

nk.register_rpc(rpc_level_list, "rpc_level_list")
