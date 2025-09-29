
-- Globals
nk = require("nakama")
utility = require("utility.utility")
storage = require("utility.storage")
map = require("utility.map")
lb = require("utility.lb")
uuid = require("utility.uuid")

-- rpcs
local rpc_play = require("rpcs.rpc_play")
local rpc_user = require("rpcs.rpc_user")
local rpc_user_info = require("rpcs.rpc_user_info")
local rpc_level = require("rpcs.rpc_level")
local rpc_room = require("rpcs.rpc_room")

nk.logger_info("Lua module loaded")