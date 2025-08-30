
nk.logger_info("RPC_User loaded")

local function rpc_try_claim_username(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)

	local userID = c.user_id
	local metadata = {}
	local username = p.name
	local display_name = ""
	local timezone = nil
	local location = ""
	local lang_tag = nil
	local avatar_url = "" -- from steam? 

	local status, err = pcall(nk.account_update_id, userID, metadata, username, display_name, timezone, location, lang_tag, avatar_url)

	if (not status) then
		nk.logger_info(("Account update error: %q"):format(err))
		return nk.json_encode({
			["payload"] = {},
			["success"] = false
		})
	end

	return nk.json_encode({
		["payload"] = {},
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
	if (not set) then delta = -1 end

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

nk.register_rpc(rpc_try_claim_username, "rpc_try_claim_username")
nk.register_rpc(rpc_set_favourite, "rpc_set_favourite")
