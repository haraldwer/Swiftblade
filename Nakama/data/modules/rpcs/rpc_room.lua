
nk.logger_info("RPC_Room loaded")

local function rpc_room_info(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local roomID = p.ID

	local data = storage.read(roomID, "data")
	local info = storage.read(roomID, "info")
	local scene = storage.read(roomID, "scene")

	if data == nil then
		nk.logger_error("Failed to get room data " .. roomID)
		return nk.json_encode({ 
			["payload"] = {},
			["success"] = false
		})
	end

	if info == nil then
		nk.logger_error("Failed to get room info " .. roomID)
		return nk.json_encode({ 
			["payload"] = {},
			["success"] = false
		})
	end

	if scene == nil then
		nk.logger_error("Failed to get room scene " .. roomID)
		return nk.json_encode({ 
			["payload"] = {},
			["success"] = false
		})
	end

	return nk.json_encode({ 
		["payload"] = {
			["Data"] = {
				["Playcount"] = data.playcount,
				["Favourites"] = data.favourites,
				["Status"] = data.status
			},
			["Info"] = info,
			["Scene"] = nk.json_encode(scene)
		},
		["success"] = true
	})
end

-- Room lists:
-- user_rooms_(id) (user_current)
-- rooms_most_played
local function rpc_room_list(context, payload)
	local p = utility.parse(payload)
	local c = utility.parse(context)
	local listName = p.List;

	if listName == "user_current" then
		listName = "user_rooms_" .. c.user_id
	end

	local records = lb.list(listName)
	if records == nil then
		nk.logger_error("Failed to list rooms " .. listName)
		return nk.json_encode({ 
			["payload"] = {},
			["success"] = false
		})
	end

	local result = {}
	for _, record in ipairs(records) do
		if record.metadata == nil then
			nk.logger_error("No metadata for entry " + record.username)
		else
			local entry = record.metadata
			entry["ID"] = record.username
			entry["Score"] = record.score
			entry["Rank"] = record.rank
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

local function rpc_submit_room(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)

	local info = p.Info;
	local scene = p.Scene;

	if info == nil then
		nk.logger_error("Failed to parse room info")
		return nk.json_encode({
			["payload"] = {
				["Error"] = "Failed to parse room info",
				["Success"] = false
			},
			["success"] = true
		})
	end

	if scene == nil then
		nk.logger_error("Failed to parse scene")
		return nk.json_encode({
			["payload"] = {
				["Error"] = "Failed to parse scene",
				["Success"] = false
			},
			["success"] = true
		})
	end

	if info.Name == nil then
		nk.logger_error("Failed to parse room name")
		return nk.json_encode({
			["payload"] = {
				["Error"] = "Room does not have a valid name",
				["Success"] = false
			},
			["success"] = true
		})
	end

	if info.Type == nil then
		nk.logger_error("Failed to parse room type")
		return nk.json_encode({
			["payload"] = {
				["Error"] = "Room does not have a valid type",
				["Success"] = false
			},
			["success"] = true
		})
	end

	local roomID = "room_" .. c.user_id .. "_" .. nk.md5_hash(info.Name)
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

	local user = "user_" .. c.user_id
	local userInfo = storage.read(user, "info")
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

	local roomUUID = nk.uuid_v4()
	local data = {
		["uuid"] = roomUUID,
		["status"] = "Pending",
		["date"] = os.date("%d-%m-%Y"), -- todays date
		["favourites"] = 0,
		["playcount"] = 0
	}
	local dataSuccess = storage.write(roomID, "data", data)
	if not dataSuccess then
		return nk.json_encode({
			["payload"] = { 
				["Error"] = "Failed to write data to storage",
				["Success"] = false
			},
			["success"] = true
		})
	end

	info["ID"] = roomID
	info["CreatorID"] = c.user_id
	info["CreatorName"] = userInfo.name
	local infoSuccess = storage.write(roomID, "info", info)
	if not infoSuccess then
		return nk.json_encode({
			["payload"] = { 
				["Error"] = "Failed to write info to storage",
				["Success"] = false
			},
			["success"] = true
		})
	end

	local sceneObj = nk.json_decode(scene)
	local sceneSuccess = storage.write(roomID, "scene", sceneObj)
	if sceneObj == nil or not sceneSuccess then
		return nk.json_encode({
			["payload"] = { 
				["Error"] = "Failed to write scene to storage",
				["Success"] = false
			},
			["success"] = true
		})
	end

	-- Room user lb
	local creatorLB = "user_rooms_" .. c.user_id
	lb.create_write(creatorLB, "desc", "best", "", {}, roomUUID, roomID, info, 0)

	-- Room lbs
	local lbID = "rooms_most_played"
	if info.Type ~= "ROOM" then
		lbID = lbID .. "_" .. info.Type
	end

	lb.create_write(lbID, "desc", "best", "", {}, roomUUID, roomID, info, 0)
	lb.create_write(lbID .. "_weekly", "desc", "best", "0 0 * * 1", {}, roomUUID, roomID, info, 0)
	lb.create_write(lbID .. "_monthly", "desc", "best", "0 0 1 * *", {}, roomUUID, roomID, info, 0)

	return nk.json_encode({
		["payload"] = {
			["ID"] = roomID,
			["Success"] = true
		},
		["success"] = true
	})
end

nk.register_rpc(rpc_room_info, "rpc_room_info")
nk.register_rpc(rpc_room_list, "rpc_room_list")
nk.register_rpc(rpc_submit_room, "rpc_submit_room")
