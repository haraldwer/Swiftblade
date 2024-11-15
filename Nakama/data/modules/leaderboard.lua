local nk = require("nakama")
local utility = require("utility")
local blob = require("blob")

nk.logger_info("Lua Leaderboard module loaded.")

-- https://heroiclabs.com/docs/nakama/server-framework/lua-runtime/function-reference/#Leaderboards

-- Helper

local function LeaderboardExists(seed)
	local leaderboard_ids = { seed }
	local leaderboards = nk.leaderboards_get_id(leaderboard_ids)
	if (not leaderboards == nil and not leaderboards == {}) then
		return false
	end
	return true
end

-- List leaderboard

local function Lua_LB_List(context, payload)
	
	utility.print_table(context)
	utility.print_table(payload)
	
	-- Resolve payload
	local json = nk.json_decode(payload)
	local type = json.myType
	local seed_type = json.mySeedType
	local seed = json.mySeed
	local entries = json.myEntries

	-- Owners
	local type_switch = {
		["PERSONAL"] = function()
				return { context.user_id }
			end,

		["FRIENDS"] = function()
				-- User friends?
				return nil
			end,

		["GLOBAL"] = function()
				return nil
			end
	}
	local owners = type_switch[type]()

	-- ID
	local seed_switch = {
		["ANY"] = function()
				return "Global"
			end,
		["LAST_PLAYED"] = function()
				return tostring(blob.get_last_seed(context.user_id))
			end,
		["SPECIFIC"] = function()
				return tostring(seed)
			end
	}
	local id = seed_switch[seed_type]()
	if (not LeaderboardExists(id)) then
		id = "Global"
	end

	-- List scores
	local cursor = ""
	local records, owner_records, next_cursor, prev_cursor, list_error = nk.leaderboard_records_list(id, owners, entries, cursor)
	if (list_error) then 
		nk.logger_error("Failed to list leaderboard");
		return nk.json_encode({ 
			["payload"] = payload,
			["success"] = false
		})
	end

	-- Layout:
	-- 	[{
	-- 		\"create_time\":1670191318,
	-- 		\"expiry_time\":1670198400,
	-- 		\"leaderboard_id\":\"Global\",
	-- 		\"max_num_score\":1000000,
	-- 		\"metadata\":{},
	-- 		\"num_score\":1,
	-- 		\"owner_id\":\"6f2faca1-b9a7-4967-8bd3-e136f9d94a21\",
	-- 		\"rank\":1,
	-- 		\"score\":11113,
	-- 		\"subscore\":0,
	-- 		\"update_time\":1670191318,
	-- 		\"username\":\"haral\"
	-- }]

	-- Convert to matching structure
	local entries = {}
	for i,v in ipairs(records) do
		table.insert(entries, {
			["myName"] = v.username,
			["myUserID"] = v.owner_id,
			["myRank"] = v.rank,
			["myScore"] = v.score,
			["mySeed"] = tonumber(v.metadata.seed)
		})
	end
	local result = {
		["myType"] = type,
		["mySeedType"] = seed_type,
		["mySeed"] = seed,
		["myEntries"] = entries
	}

	return nk.json_encode({ 
		["payload"] = result,
		["success"] = true
	})
end

-- Submit score

local function CreateLeaderboard(id)

	--if (LeaderboardExists(id)) then
	--	return nil
	--end

	local authoritative = true
	local sort = "desc"
	local operator = "best"
	local reset = "0 0 * * 1"
	local lb_meta = { }
	local create_error = nk.leaderboard_create(id, authoritative, sort, operator, reset, lb_meta)
	if (create_error) then 
		nk.logger_error("Failed to create leaderboard" .. id)
		return create_error
	end
	return nil
end

local function SubmitScore(id, seed, owner, username, score)

	local create_error = CreateLeaderboard(id)
	if (create_error) then 
		return create_error
	end

	local subscore = 0
	local score_meta = { ["seed"] = seed }
	local new_record, write_error = nk.leaderboard_record_write(id, owner, username, score, subscore, score_meta)
	if (write_error) then 
		nk.logger_error("Failed to write to leaderboard " .. id);
		return write_error
	end
	return nil
end

local function Lua_LB_Write(context, payload)

	utility.print_table(context)
	utility.print_table(payload)

	-- Parse json
	local json = nk.json_decode(payload)
	local seed = tostring(json.mySeed)
	local score = json.myScore
	local owner = context.user_id
	local username = context.username

	-- Submit score to global lb
	local global_error = SubmitScore("Global", seed, owner, username, score)
	if (global_error) then
		return nk.json_encode({ 
				["payload"] = payload,
				["success"] = false
			})
	end

	-- Submit score to seed lb
	local submit_error = SubmitScore(seed, seed, owner, username, score)
	if (submit_error) then
		return nk.json_encode({ 
				["payload"] = payload,
				["success"] = false
			})
	end

	-- Return result
	return nk.json_encode({ 
		["payload"] = payload,
		["success"] = true
	})
end

-- Register RPCs 

nk.register_rpc(Lua_LB_List, "Lua_LB_List")
nk.register_rpc(Lua_LB_Write, "Lua_LB_Write")