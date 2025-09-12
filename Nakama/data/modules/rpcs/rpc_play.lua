
nk.logger_info("RPC_Play loaded")

local function update_lb(id, name, leaderboard, cron, score)
	lb.try_create(leaderboard, "desc", "best", cron, {})
	lb.try_write(leaderboard, id, name, score)
end

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
	for roomID in room_list do
		local roomPlaycount = storage.change(roomID, "playcount", 1)
		
		-- Increase room ranking for user submissions
		local roomInfo = storage.read(roomID, "info")
		if roomInfo ~= nil then
			local roomCreatorList = "user_rooms_" .. roomInfo.creator
			lb.try_write(roomCreatorList, roomID, roomInfo.name, roomPlaycount)
		end
	end

	local levelPlaycount = storage.change(levelID, "playcount", 1)
	local seedPlaycount = storage.change(seedID, "playcount", 1)
	storage.change(userID, "plays", 1)
	storage.write(userID, "current", level_hash) -- Update user current level

	local levelName = levelInfo.name
	local levelCreator = levelInfo.creator
	local levelCreatorList = "user_rooms_" .. levelInfo.creator
	lb.try_write(levelCreatorList, levelID, levelName, levelPlaycount)

	-- Update level leaderboards
	if (storage.read(levelID, "status") == "Approved") then
		update_lb(levelID, levelName, "most_played", "", levelPlaycount);
		update_lb(levelID, levelName, "most_played_weekly", "0 0 * * 1", levelPlaycount);
		update_lb(levelID, levelName, "most_played_monthly", "0 0 1 * *", levelPlaycount);
		local seedName = levelName .. " (" .. seed .. ")"
		update_lb(seedID, seedName, "seed_most_played", "", seedPlaycount);
		update_lb(seedID, seedName, "seed_most_played_weekly", "0 0 * * 1", seedPlaycount);
		update_lb(seedID, seedName, "seed_most_played_monthly", "0 0 1 * *", seedPlaycount);
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
		update_lb(levelID, userID, username, "scores", "", score)
		update_lb(levelID, userID, username, "scores_weekly", "0 0 * * 1", score)
		update_lb(levelID, userID, username, "scores_monthly", "0 0 1 * *", score)

		-- Set top ranks for seed
		update_lb(seedID, userID, username, "scores", "", score)
		update_lb(seedID, userID, username, "scores_weekly", "0 0 * * 1", score)
		update_lb(seedID, userID, username, "scores_monthly", "0 0 1 * *", score)
	end

	storage.write(userID, "current", "")

	return nk.json_encode({
		["payload"] = {},
		["success"] = true
	})
end

nk.register_rpc(rpc_begin_play, "rpc_begin_play")
nk.register_rpc(rpc_end_play, "rpc_end_play")