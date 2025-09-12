
nk.logger_info("Utility loaded")

local function print_table(table)
	local str = ""
	if type(table) == "string" then
		str = table
	else
		str = nk.json_encode(table)
	end
	nk.logger_debug("print_table : " .. str)
end

local function parse(json)
	if type(json) ~= "string" then
		return json -- Json is already parsed
	end
	nk.logger_debug("parse : " .. json)
	return nk.json_decode(json)
end

return {
	print_table = print_table,
	parse = parse
}