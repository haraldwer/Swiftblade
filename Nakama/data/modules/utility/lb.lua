
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
		nk.leaderboard_ranks_disable(id)
	else
		nk.logger_error("lb already exists " .. id)
	end
	return true
end

local function lb_write(lbID, entryID, entryName, entryMeta, score)
	nk.logger_error("LB write " .. lbID .. " " .. entryID .. " " .. entryName .. " " .. score .. "")
	local record, err = nk.leaderboard_record_write(lbID, entryID, entryName, score, 0, entryMeta)
	if err ~= nil then
		nk.logger_error("Failed to write to lb " .. lbID .. " (" .. entryName .. ":" .. score .. ") | " .. err)
		return false
	end
	return true
end

local function lb_create_write(lbID, sort, operator, reset, meta, entryID, entryName, entryMeta, score)
	lb_try_create(lbID, sort, operator, reset, meta)
	return lb_write(lbID, entryID, entryName, entryMeta, score)
end

local function lb_list(id)
	local owners = nil
	local limit = 100
	local cursor = ""
	local override_expiry = 0
	local records, owner_records, next_cursor, prev_cursor, error = nk.leaderboard_records_list(id, owners, limit, cursor, override_expiry)
	if records == nil then
		nk.logger_error("LB list failed for " .. id)
		return nil
	end
	return records;
end

return {
	exists = lb_exists,
	try_create = lb_try_create,
	create_write = lb_create_write,
	write = lb_write,
	list = lb_list
}