
local nk = require("nakama")
local utility = require("utility")

nk.logger_info("Lua Blob module loaded.")

-- For managing persistence and blob data

-- Blob structure: 
-- 	mySeedData
-- 		mySeed:0
-- 	myLBData
--		myEntries:[]
--			myName:"";
--			myUserID:"";
--			myRank:-1;
--			myScore:-1;
--			mySeed:-1;

-- Client can read blob without custom RPCs

-- Last seed

local function GetLastSeed(user_id)
	local object_ids = {{ 
		collection = "blob", 
		key = "mySeedData", 
		user_id = user_id
	}}
	local objects = nk.storage_read(object_ids)
	for _, r in ipairs(objects) do
		local message = string.format("LastSeed read: %q, write: %q, value: %q", r.permission_read, r.permission_write, r.value)
		nk.logger_info(message)
		-- convert from json and read value
		return r.value.mySeed
	end
	return 0
end

-- Leaderboard

-- Expose functions

return { 
	get_last_seed = GetLastSeed,
}