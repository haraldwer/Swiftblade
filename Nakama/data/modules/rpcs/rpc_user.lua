
nk.logger_info("RPC_User loaded")

local function rpc_try_claim_username(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local userID = c.user_id
	local name = p.name
end

local function rpc_set_favorite(context, payload)
	local c = utility.parse(context)
	local p = utility.parse(payload)
	local userID = c.user_id
	local levelID = p.levelID
	local seed = p.seed -- Favourite is seed-specific!
	local set = p.set

	local delta = 1
	if (not set) then delta = -1 end

	-- Update level favourites
	local favs = storage.change(levelID, "favourites", delta)
	local leaderboard = "levels_favourites"
	lb.try_create_lb(leaderboard, "desc", "best")
	lb.try_write(leaderboard, favs)

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
end

nk.register_rpc(rpc_try_claim_username, "rpc_try_claim_username")
nk.register_rpc(rpc_set_favorite, "rpc_set_favorite")
