
nk.logger_info("RPC_Level loaded")

local function rpc_level_info(context, payload)
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

-- scores
-- scores_weekly
-- scores_monthly
-- (can also list seed specific info!)
local function rpc_level_leaderboard(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local levelID = p.levelID
	local seed = p.seed

	-- Find seed uuid?
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

-- user_levels_(id)
-- most_played
-- most_played_weekly
-- most_played_monthly
-- seed_most_played
-- seed_most_played_weekly
-- seed_most_played_monthly
local function rpc_level_list(context, payload)
	local p = utility.parse(payload)
	local c = utility.parse(context)
	local listName = p.List;

	if listName == "user_current" then
		listName = "user_levels_" .. c.user_id
	end

	local records = lb.list(listName)
	if records == nil then
		nk.logger_error("Failed to list rooms " .. listName)
		return nk.json_encode({ 
			["payload"] = {},
			["success"] = false
		})
	end

	utility.print_table(records)

	local result = {}
	for _, record in ipairs(records) do
		if record.metadata == nil then
			nk.logger_error("No metadata for entry " + record.username)
		else
			-- Use metadata to avoid fetching data for each room
			local entry = {
				["ID"] = record.username,
				["Name"] = record.metadata.levelName,
				["CreatorName"] = record.metadata.creatorName,
				["CreatorID"] = record.metadata.creatorID,
				["Rank"] = record.rank,
				["Score"] = record.score,
				["Date"] = record.update_time
			}
			table.insert(result, entry)
		end
	end

	return nk.json_encode({ 
		["payload"] = {
			["List"] = listName,
			["Entries"] = result
		},
		["success"] = true
	})
end

local function rpc_submit_level(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)

	local levelHash = p.Hash;
	local levelData = p.Data;
	if levelData == nil or levelHash == nil then
		nk.logger_error("Failed to parse room data")
		return nk.json_encode({
			["payload"] = {
				["Error"] = "Failed to parse room data",
				["Success"] = false
			},
			["success"] = true
		})
	end

	local levelID = "level_" .. c.user_id .. "_" .. nk.md5_hash(levelData.Name)
	if storage.read(levelID, "name") ~= nil then
		nk.logger_error("Level submission failed, ID already exists " .. levelID)
		return nk.json_encode({
			["payload"] = {},
			["success"] = false
		})
	end

	local user = "user_" .. c.user_id
	local userInfo = storage.read(user)
	if userInfo == nil then
		nk.logger_error("Room submission failed, failed to get username for " .. user)
		return nk.json_encode({
			["payload"] = {
				["Error"] = "Failed to get username",
				["Success"] = false
			},
			["success"] = true
		})
	end

	local levelUUID = nk.uuid_v4()
	local success = storage.write(levelID, "info", {
		["uuid"] = levelUUID,
		["name"] = levelData.Name,
		["hash"] = levelHash,
		["creatorID"] = c.user_id,
		["creatorName"] = userInfo.name,
		["rooms"] = utility.parse(levelData.Rooms),
		["status"] = "Pending",
		["date"] = os.date("%d-%m-%Y"), -- todays date
		["favourites"] = 0,
		["playcount"] = 0,
		["finishcount"] = 0
	})

	if not success then
		return nk.json_encode({
			["payload"] = { 
				["Error"] = "Failed to write to storage",
				["Success"] = false
			},
			["success"] = true
		})
	end


	local lbMeta = {
		["levelName"] = levelData.Name,
		["creatorID"] = c.user_id,
		["creatorName"] = userInfo.name
	}

	-- Level user lb
	local creatorLB = "user_levels_" .. c.user_id
	lb.create_write(creatorLB, "desc", "best", "", {}, levelUUID, levelID, lbMeta, 0)

	-- Level score lbs
	lb.try_create("scores_" .. levelID, "desc", "best", "", {})
	lb.try_create("scores_weekly_" .. levelID, "desc", "best", "0 0 * * 1", {})
	lb.try_create("scores_monthly_" .. levelID, "desc", "best", "0 0 1 * *", {})

	-- Level lbs
	lb.create_write("most_played", "desc", "best", "", {}, levelUUID, levelID, lbMeta, 0)
	lb.create_write("most_played_weekly", "desc", "best", "0 0 * * 1", {}, levelUUID, levelID, lbMeta, 0)
	lb.create_write("most_played_monthly", "desc", "best", "0 0 1 * *", {}, levelUUID, levelID, lbMeta, 0)

	-- Level seed lbs
	lb.try_create("seed_most_played", "desc", "best", "", {})
	lb.try_create("seed_most_played_weekly", "desc", "best", "0 0 * * 1", {})
	lb.try_create("seed_most_played_monthly", "desc", "best", "0 0 1 * *", {})

	return nk.json_encode({
		["payload"] = { ["levelID"] = levelID },
		["success"] = true
	})
end

nk.register_rpc(rpc_level_info, "rpc_level_info")
nk.register_rpc(rpc_level_leaderboard, "rpc_level_leaderboard")
nk.register_rpc(rpc_level_list, "rpc_level_list")
nk.register_rpc(rpc_submit_level, "rpc_submit_level")