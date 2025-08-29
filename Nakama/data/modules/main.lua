
-- Globals
nk = require("nakama")
utility = require("utility.utility")
storage = require("utility.storage")
map = require("utility.map")
lb = require("utility.lb")

-- rpcs
local rpc_play = require("rpcs.rpc_play")
local rpc_submit = require("rpcs.rpc_submit")
local rpc_user = require("rpcs.rpc_user")
local rpc_user_info = require("rpcs.rpc_user_info")
local rpc_level_info = require("rpcs.rpc_level_info")
local rpc_level_list = require("rpcs.rpc_level_list")

nk.logger_info("Lua module loaded")