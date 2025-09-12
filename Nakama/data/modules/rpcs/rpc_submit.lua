
nk.logger_info("RPC_Submit loaded")

local function rpc_submit_room(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)

	local roomID = "room_" .. c.user_id .. "_" .. p.Name
	if storage.read(roomID, "info") ~= nil then
		nk.logger_error("Room submission failed, ID already exists " .. roomID)
		return nk.json_encode({
			["payload"] = {
				["Error"] = "Room already submitted",
				["Success"] = false
			},
			["success"] = true
		})
	end

	local success = storage.write(roomID, "info", {
		["name"] = p.Name,
		["creator"] = c.user_id,
		["scene"] = utility.parse(p.Scene),
		["size"] = p.Size,
		["length"] = p.Length,
		["enemies"] = p.Enemies,
		["status"] = "Pending",
		["date"] = os.date("%d-%m-%Y"), -- todays date
		["favourites"] = 0,
		["playcount"] = 0
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

	-- Create LB for rooms
	local creatorLB = "user_rooms_" .. c.user_id
	lb.try_create(creatorLB, "desc", "best", "", {})

	return nk.json_encode({
		["payload"] = {
			["RoomID"] = roomID,
			["Success"] = true
		},
		["success"] = true
	})
end

local function rpc_submit_level(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)

	local levelID = "level_" .. c.user_id .. "_" .. p.Name
	if storage.read(levelID, "name") ~= nil then
		nk.logger_error("Level submission failed, ID already exists " .. levelID)
		return nk.json_encode({
			["payload"] = {},
			["success"] = false
		})
	end

	local success = storage.write(levelID, "info", {
		["name"] = p.Name,
		["hash"] = p.Hash,
		["creator"] = c.user_id,
		["rooms"] = utility.parse(p.Rooms),
		["status"] = "Pending",
		["date"] = os.date("%d-%m-%Y"), -- todays date
		["favourites"] = 0,
		["playcount"] = 0,
		["finishcount"] = 0
	})

	-- Create LB for levels
	local creatorLB = "user_levels_" .. c.user_id
	lb.try_create_lb(creatorLB, "desc", "best", "", {})

	return nk.json_encode({
		["payload"] = { ["levelID"] = levelID },
		["success"] = true
	})
end

nk.register_rpc(rpc_submit_room, "rpc_submit_room")
nk.register_rpc(rpc_submit_level, "rpc_submit_level")