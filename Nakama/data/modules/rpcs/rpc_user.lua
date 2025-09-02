
nk.logger_info("RPC_User loaded")

local function rpc_login_user(context, payload)
	local p = utility.parse(payload)
	local newName = p.Name
	local user = "user_" .. context.user_id
	if (not newName == nil) then
		nk.logger_info("New name: " .. newName)
	end

	-- Read existing user info
	local info = storage.read(user, "info")
	utility.print_table(info)
	local currName = nil
	if info ~= nil then
		currName = info.name
		nk.logger_info("Curr name: " .. currName)
	end

	-- Maybe use current name
	local shouldUseCurrentName = currName == newName or newName == "" or newName == nil
	if (shouldUseCurrentName and currName ~= nil and currName ~= "") then
		nk.logger_info("Using current name: " .. currName)
		return nk.json_encode({
			["payload"] = { 
				["Name"] = currName,
				["LoggedIn"] = true
			},
			["success"] = true
		})
	end

	-- Maybe no new name was specified
	if newName == "" or newName == nil then
		nk.logger_info("No new name")
		return nk.json_encode({
			["payload"] = {
				["Name"] = currName,
				["LoggedIn"] = false
			},
			["success"] = true
		})
	end

	-- Is new name already claimed?
	local nameObj = storage.read("usernames", newName)
	if nameObj ~= nil then
		if nameObj.user ~= context.user_id then
			nk.logger_info("Name claimed: " .. newName )
			return nk.json_encode({
				["payload"] = {
					["Name"] = currName,
					["LoggedIn"] = false
				},
				["success"] = true
			})
		end
	end

	-- Remove traces of previous name
	if currName ~= "" and currName ~= nil then
		storage.remove("usernames", currName)
	end

	-- Write new name
	storage.write("usernames", newName, {
		["user"] = context.user_id,
		["date"] = os.date("%d-%m-%Y")
	})

	-- Write default user data
	if info == nil then
		info = { 
			["name"] = newName,
			["first_login"] = os.date("%d-%m-%Y")
		}
	else
		info.name = newName
		-- TODO: List of previous names
	end
	storage.write(user, "info", info)

	return nk.json_encode({
		["payload"] = {
			["Name"] = newName,
			["LoggedIn"] = true
		},
		["success"] = true
	})
end

local function update_fav_lb(lb, id, favs)
	lb.try_create_lb(lb, "desc", "set", "", {})
	lb.write(lb, id, favs)
end

local function rpc_set_favourite(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local userID = c.user_id
	local levelID = p.levelID
	local seed = p.seed -- should favourites be seed-specific? 
	local levelSeedID = levelID .. "_" .. seed
	local set = p.set

	local delta = 1
	if not set then delta = -1 end

	-- Update level favourites
	local favs = storage.change(levelID, "favourites", delta)
	update_fav_lb("level_favourites", levelID, favs)
	-- Also update lb for specific seed?

	-- Update user favourites
	local user_favs = storage.read(userID, "favourites")
	if (set) then
		if (not map.contains(user_favs, levelID)) then
			map.add(user_favs, levelID)
		end
	else
		if (map.contains(user_favs, levelID)) then
			map.remove(user_favs, levelID)
		end
	end

	-- Also update room favourites
	local rooms = storage.read(levelID, "rooms")
	for _,room in ipairs(rooms) do
		local roomFavs = storage.change(room, "favourites", delta)
		update_fav_lb("room_favourites", room, roomFavs)
	end

	return nk.json_encode({
		["payload"] = {},
		["success"] = true
	})
end

nk.register_rpc(rpc_login_user, "rpc_login_user")
nk.register_rpc(rpc_set_favourite, "rpc_set_favourite")
