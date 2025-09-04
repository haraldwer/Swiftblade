
nk.logger_info("RPC_Level_List loaded")

local function list_lb(lbID)
	local limit = 50
	local records, ownerRecords, nextCursor, prevCursor, error = nk.leaderboard_records_list(lbID, {}, limit, "", 0)

	if (not error == nil) then
		nk.logger_error("LB list failed for " .. lbID .. " : " .. error)
		return nil
	end

	utility.print_table(records)

	return nk.json_encode({ 
		["payload"] = records,
		["success"] = true
	})
end

local function rpc_level_list(context, payload)
	local list = payload.List;
	local result = list_lb(list)

	-- now fetch data for each level before returning
	

end

nk.register_rpc(rpc_level_list, "rpc_level_list")
