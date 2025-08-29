
nk.logger_info("RPC_Play loaded")

local function update_level_lb(levelID, userID, username, leaderboard, cron, score)
	lb.try_create_lb(leaderboard, "desc", "best", cron, {})
	lb.try_write(leaderboard, userID, username, score)
end

local function rpc_begin_play(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local levelID = p.levelID
	local level_hash = p.level_hash
	local userID = context.user_id
	local username = context.username

	if (not storage.read(levelID, "hash") == level_hash) then
		nk.logger_error("BeginPlay didnt count because of invalid level hash");
		return 
	end

	-- Increase storage stats
	local room_list = parse(storage.read(levelID, "rooms")) -- Todo: Parse
	for roomID in room_list do
		storage.change(roomID, "playcount", 1)
	end
	local playcount = storage.change(levelID, "playcount", 1)
	storage.change(userID, "plays", 1)

	-- Update level leaderboards
	if (storage.read(levelID, "status") == "Approved") then
		update_level_lb(levelID, userID, username, "most_played", playcount);
		update_level_lb(levelID, userID, username, "most_played_weekly", playcount);
		update_level_lb(levelID, userID, username, "most_played_monthly", playcount);
	end

	-- Update user current level
	storage.write(userID, "current", level_hash)

end

local function rpc_end_play(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local levelID = p.levelID
	local level_hash = p.level_hash
	local userID = context.user_id

	if (not storage.read(levelID, "hash") == level_hash) then
		nk.logger_error("EndPlay didnt count because of invalid level hash");
		return 
	end
	if (not storage.read(userID, "current") == level_hash) then
		nk.logger_error("EndPlay didnt count because of invalid user hash");
		return 
	end

	storage.change(userID, "kills", 1);
	storage.change(levelID, "finish_count", 1);

	-- Calculate rank?
	if (storage.read(levelID, "status") == "Approved") then
		-- Set user top ranks
	end

	storage.write(userID, "current", "")
end

nk.register_rpc(rpc_begin_play, "rpc_begin_play")
nk.register_rpc(rpc_end_play, "rpc_end_play")