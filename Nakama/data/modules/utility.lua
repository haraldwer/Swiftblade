local nk = require("nakama")

nk.logger_info("Lua Utility module loaded.")

function PrintTable(table)
	local json = nk.json_encode(table)
	nk.logger_debug("print_table : " .. json)
end

-- Expose functions

return { print_table = PrintTable }