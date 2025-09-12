
nk.logger_info("LB loaded")

local function lb_exists(id)
	local ids = { id }
	local lbs, err = nk.leaderboards_get_id(ids)
	if err ~= nil then
		nk.logger_error("lb exists error: " .. err)
		return false
	end
	if lbs == nil or next(lbs) == nil then
		nk.logger_error("lb does not exist " .. id)
		return false
	end
	nk.logger_error("lb exists " .. id)
	utility.print_table(lbs)
	return true
end

local function lb_try_create(id, sort, operator, reset, meta)
	if not lb_exists(id) then
		nk.logger_error("Creating lb " .. id)
		local authoritative = true
		local enable_ranks = false
		local err = nk.leaderboard_create(id, authoritative, sort, operator, reset, meta, enable_ranks)
		if err ~= nil then
			nk.logger_error("Failed to create lb " .. id .. " | " .. err)
			return false
		end
	else
		nk.logger_error("lb already exists " .. id)
	end

	return true
end

local function lb_write(lbID, entry, score)
	local subscore = 0
	local meta = {}
	local override = 0
	local record, err = nk.leaderboard_record_write(lbID, entry, "", score, subscore, meta, override)
	if err ~= nil then
		nk.logger_error("Failed to write to lb " .. lbID .. " (" .. entry .. ":" .. score .. ") | " .. err)
		return false
	end
	return true
end

return {
	exists = lb_exists,
	try_create = lb_try_create,
	write = lb_write
}