
local function storage_read(collection, key)
	local object_ids = { { collection = collection, key = key } }
	local objects = nk.storage_read(id)
	for _, r in ipairs(objects) do
		return r.value
	nk.logger_debug("Failed to read storage: " + collection + " - " + key)
	return ""
end

local function storage_write(collection, key, value)
	
end

local function storage_increase(collection, key, value)
	-- read, parse, set and write
end

local function storage_decrease(collection, key, value)
	-- read, parse, set and write
end

local function read_level_info(levelID, key)
	return storage_read("level_info_" + levelID, key)
end

local function check_level_hash(levelID, level_hash)
	return read_level_info(levelID, "hash") == level_hash)
end

local function check_level_approved(levelID)
	return read_level_info(levelID, "status") == "Approved")
end

local function increase_room_playcount(levelID)
	local room_list = parse(read_level_info(levelID, "rooms")) -- Todo: Parse
	for roomID in room_list
		metrics_counter_add("room_playcount_" + roomID, {}, 1)
	end
end

local function increase_level_playcount(levelID)
	metrics_counter_add("level_playcount_" + levelID, {}, 1)
end

local function increase_user_playcount()
	metrics_counter_add("user_plays_" + userID, {}, 1)
end

local function update_level_lb(levelID, leaderboard)
	local playcount = metrics_counter_
end

local function begin_play(levelID, seed, level_hash)
	if (not check_level_hash(levelID, level_hash)) then
		return 
	end

	-- for each room
	increase_room_playcount(levelID)
	increase_level_playcount(levelID)
	increase_user_playcount()

	if (check_level_approved) then
		update_level_lb(levelID, "most_played");
		update_level_lb(levelID, "most_played_weekly");
		update_level_lb(levelID, "most_played_monthly");
	end

end





local function end_play

end