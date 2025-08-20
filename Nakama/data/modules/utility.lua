local nk = require("nakama")

nk.logger_info("Lua Utility module loaded.")

function print_table(table)
	local json = nk.json_encode(table)
	nk.logger_debug("print_table : " .. json)
end

function try_create_lbs(id, sort, operator, reset, meta) -- ids = A list of leaderboard ids to check
	local ids = { id }
	local lbs = nk.leaderboards_get_id(ids)
	if (lbs == nil or lbs == {}) then
		local authoritative = true
		local sort = "desc"
		local reset = "0 0 * * 1"
		local metadata = {
		  weather_conditions = "rain"
		}
		local enable_ranks = false
		nk.leaderboard_create(id, authoritative, sort, operator, reset, meta, enable_ranks)
		leaderboard_create(id)
end

-- Expose functions

return { 
	print_table = print_table 
	try_create_lbs = try_create_lbs
}