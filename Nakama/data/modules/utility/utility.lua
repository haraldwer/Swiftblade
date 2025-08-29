
nk.logger_info("Utility loaded")

local function print_table(table)
	local json = nk.json_encode(table)
	nk.logger_debug("print_table : " .. json)
end

local function parse(json)
	nk.logger_debug("parse : " + json)
	return nk.json_decode(json)
end

return {
	print_table = print_table,
	parse = parse
}