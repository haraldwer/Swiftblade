
nk.logger_info("RPC_Play loaded")

local function rpc_begin_play(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	
	local levelID = p.levelID
	local userID = c.user_id

	local seed = p.seed
	local level_hash = p.level_hash
	local seedID = levelID .. "_" .. seed

	-- Increase level ranking for user submissions
	local levelInfo = storage.read(levelID, "info")
	if levelInfo == nil then
		nk.logger_error("Unknown level ID");
		return nk.json_encode({
			["payload"] = {},
			["success"] = false
		})
	end

	if levelInfo ~= level_hash then
		nk.logger_error("BeginPlay didnt count because of invalid level hash");
		return nk.json_encode({
			["payload"] = {},
			["success"] = false
		})
	end

	-- Increase storage stats
	local room_list = utility.parse(storage.read(levelID, "rooms")) -- Todo: Parse
	for roomID in room_list do -- _, value in ipairs(keys)
		local roomPlaycount = storage.change(roomID, "playcount", 1)
		local roomInfo = storage.read(roomID, "info")
		if roomInfo ~= nil then
			-- Increase overall room ranking
			lb.write("rooms_most_played", roomInfo.uuid, roomInfo.name, roomPlaycount);
			lb.write("rooms_most_played_weekly", roomInfo.uuid, roomInfo.name, roomPlaycount);
			lb.write("rooms_most_played_monthly", roomInfo.uuid, roomInfo.name, roomPlaycount);
			-- Increase room ranking for user submissions
			local roomCreatorList = "user_rooms_" .. roomInfo.creator
			lb.write(roomCreatorList, roomInfo.uuid, roomInfo.name, roomPlaycount)
		end
	end

	local levelPlaycount = storage.change(levelID, "playcount", 1)
	local seedPlaycount = storage.change(seedID, "playcount", 1)
	storage.change(userID, "plays", 1)
	storage.write(userID, "current", level_hash) -- Update user current level

	local levelName = levelInfo.name
	local levelCreator = levelInfo.creator
	local levelUUID = levelInfo.uuid
	local levelCreatorList = "user_levels_" .. levelInfo.creator
	lb.write(levelCreatorList, levelUUID, levelName, levelPlaycount)

	-- Update level leaderboards
	if (levelInfo.status == "Approved") then
		lb.write("most_played", levelUUID, levelName, levelPlaycount)
		lb.write("most_played_weekly", levelUUID, levelName, levelPlaycount)
		lb.write("most_played_monthly", levelUUID, levelName, levelPlaycount)

		-- Now get the seed UUID?
		--local seedName = levelName .. " (" .. seed .. ")"
		--lb.write("seed_most_played", seedID, seedName, seedPlaycount)
		--lb.write("seed_most_played_weekly", seedID, seedName, seedPlaycount)
		--lb.write("seed_most_played_monthly", seedID, seedName, seedPlaycount)
	end

	return nk.json_encode({
		["payload"] = {},
		["success"] = true
	})
end

local function rpc_end_play(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	
	local levelID = p.levelID
	local userID = c.user_id
	local user = "user_" .. userID

	-- Get username from user data
	local username = "" 
	local info = storage.read(user, "info")
	if info ~= nil then
		username = info.name
	end

	local seed = p.seed
	local score = p.score
	local level_hash = p.level_hash
	local seedID = levelID .. "_" .. seed

	if storage.read(levelID, "hash") ~= level_hash then
		nk.logger_error("EndPlay didnt count because of invalid level hash");
		return nk.json_encode({
			["payload"] = {},
			["success"] = false
		})
	end
	if storage.read(userID, "current") ~= level_hash then
		nk.logger_error("EndPlay didnt count because of invalid user hash");
		return nk.json_encode({
			["payload"] = {},
			["success"] = false
		})
	end

	storage.change(userID, "kills", 1);
	storage.change(levelID, "finishcount", 1);

	-- Calculate score
	if storage.read(levelID, "status") == "Approved" then

		-- Set top ranks for level
		lb.write("scores_" .. levelID, userID, username, score)
		lb.write("scores_weekly_" .. levelID, userID, username, score)
		lb.write("scores_monthly_" .. levelID, userID, username, score)

		-- Set top ranks for seed (create dynamically)
		lb.create_write("scores_" .. seedID, "desc", "best", "", {}, userID, username, score)
		lb.create_write("scores_weekly_" .. seedID, "desc", "best", "0 0 * * 1", {}, userID, username, score)
		lb.create_write("scores_monthly_" .. seedID, "desc", "best", "0 0 1 * *", {}, userID, username, score)
	end

	storage.write(userID, "current", "")

	return nk.json_encode({
		["payload"] = {},
		["success"] = true
	})
end

nk.register_rpc(rpc_begin_play, "rpc_begin_play")
nk.register_rpc(rpc_end_play, "rpc_end_play")