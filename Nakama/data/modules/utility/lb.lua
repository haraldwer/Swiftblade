
nk.logger_info("LB loaded")

local function lb_exists(id)
	local ids = { id }
	local lbs = nk.leaderboards_get_id(ids)
	if (not lbs == nil and not lbs == {}) then
		return false
	end
	return true
end

local function lb_try_create(id, sort, operator, reset, meta)
	if (not lb_exists(id)) then
		local authoritative = true
		local enable_ranks = false
		nk.leaderboard_create(id, authoritative, sort, operator, reset, meta, enable_ranks)
	end
end

local function lb_write(id, userID, username, score)
	local record, err = nk.leaderboard_record_write(id, userID, username, score)
	if (not err == nil) then
		nk.logger_error("Failed to write to lb " + id + " (" + userID + ":" + score + ") | " + str(err))
	end
end

return {
	exists = lb_exists,
	try_create = lb_try_create,
	write = lb_write
}