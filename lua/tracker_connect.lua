#!/usr/local/bin/lua
-- Install luasocket cmd: sudo luarocks install luasocket

local t_b = require "table_print"
local socket = require "socket"
local torrent_parser = require "torrent_parser"

local torrent_info = torrent_parser.parse_torrent_file("test.torrent")

t_b.table_print(torrent_info)

local dl_ctx = {}
dl_ctx.listen_port = 10240
dl_ctx.peer_id = "skwix,slqxssihidlhsq"
dl_ctx.uploaded = 0
dl_ctx.downloaded = 0
dl_ctx.left = 0
dl_ctx.event = "start"
dl_ctx.numwant = 10
dl_ctx.hashinfo = "skwix,slqxssihidlhsq"

local function construct_url(tracker_url, dl_ctx)
	local url = tracker_url.."?"..
	"hashinfo="..dl_ctx.hashinfo..
	"&peer_id="..dl_ctx.peer_id..
	"&port="..dl_ctx.listen_port..
	"&unloaded="..dl_ctx.uploaded..
	"&downloaded="..dl_ctx.downloaded..
	"&left="..dl_ctx.left..
	"&event="..dl_ctx.event
	print(url)
end

if torrent_info["announce-list"][1][0] then
	construct_url(torrent_info["announce-list"][4][0], dl_ctx)
end

--local http = require "socket.http"
--local response = http.request("http://www.baidu.com/")
--local response = http.request(construct_url(torrent_info["announce-list"][1][0], dl_ctx))
--t_b.table_print(_G)

local sock = socket.connect(construct_url(torrent_info["announce-list"][1][0], dl_ctx), 2710)
print(sock)


