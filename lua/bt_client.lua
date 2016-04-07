#!/usr/local/bin/lua

io = require "io"

function download_curl(file_path, store_file_path)
	f = io.popen("curl "..file_path, "r")
	s = io.open(store_file_path, "w")
	for str in f:lines() do
		s:write(str)
	end
	f:close()
	s:close()
end

download_curl("www.baidu.com", "1.txt")
