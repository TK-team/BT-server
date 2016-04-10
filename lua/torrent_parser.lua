#!/usr/local/bin/lua

t_p = require "table_print"
--[[
	input the torrent string;
	output the target string and next-string
]]--
torrent_parser = {}

function parse_string(str)
	d_num = string.find(str, ":")
	if d_num == nil then
		return nil, str 
	end
	len = tonumber(string.sub(str, 1, d_num - 1))
	if len == nil then
		return nil, str 
	end
	str = string.sub(str, d_num + 1)
	target_str = string.sub(str, 1, len)
	if target_str == nil then
		return nil, str
	end
	--print("String "..target_str)

	next_str = string.sub(str, len + 1)
	return target_str, next_str
end
--[[
t, n = parse_string("3:1234")
print(t)
print(n)
]]--

function parse_int(str)
	flag = string.sub(str, 1, 1)
	if flag ~= "i" then
		return nil, str
	end

	str = string.sub(str, 2)
	end_num = string.find(str, "e")
	if end_num == nil then
		return nil, str 
	end
	i = tonumber(string.sub(str, 1, end_num - 1))
	next_str = string.sub(str, end_num + 1)
	--print("Integer "..i)
	return i, next_str
end
--[[
t, n = parse_int("l12341231eladfasdfae")
print(t)
print(n)
]]--

function parse_list(input_str)
	local list = {}
	local flag = string.sub(input_str, 1, 1)
	local i = 0
	if flag ~= "l" then
		return nil, input_str
	end
	str = string.sub(input_str, 2)
	flag = string.sub(str, 1, 1)
	while flag ~= "e" do
		local bak_str = str
		if flag == "" then
			return nil, nil
		end
		if flag == "l" then
			l, str = parse_list(str)
			if l == nil or str == nil then
				return nil, nil 
			end
			list[i] = l
		elseif flag == "i" then
			ret_i, str = parse_int(str)
			if ret_i == nil or str == nil then
				return nil, nil
			end
			list[i] = ret_i
		elseif flag == "d" then
			d, str = parse_dict(str)
			if d == nil or str == nil then
				return nil, nil
			end
			list[i] = d
		else
			s, str = parse_string(str)
			if s == nil or str == nil then
				return nil, nil
			end
			list[i] = s
		end
		if bak_str == str then
			return nil, nil
		end
		flag = string.sub(str, 1, 1)
		i = i + 1
	end
	str = string.sub(str, 2)
	return list, str
end

--normal
--parse_list("l3:123i123e5:dwdswi3212321ee")

-- without end 'e'
--parse_list("l3:123i123e5:dwdswi3212321e")

-- error sub-item
--parse_list("l3:123i123e10:dwdswi3212321ee")
--l, str = parse_list("ll3:123el4:5432ee")

function parse_dict(input_str)
	local key = ""
	local dict = {}
	local flag = string.sub(input_str, 1, 1)
	if flag ~= "d" then
		return nil, input_str
	end
	str = string.sub(input_str, 2)
	flag = string.sub(str, 1, 1)
	while flag ~= "e" do
		local bak_str = str
		-- get the key
		if tonumber(flag) ~= nil then
			key, str = parse_string(str)
		end

		if key == nil or str == nil then
			return nil, nil
		end

		flag = string.sub(str, 1, 1)
		if flag == "" then
			return nil, nil
		end

		-- get the value
		if flag == "l" then
			l, str = parse_list(str)
			if l == nil or str == nil then
				return nil, nil 
			end
			dict[key] = l
		elseif flag == "i" then
			ret_i, str = parse_int(str)
			if ret_i == nil or str == nil then
				return nil, nil
			end
			dict[key] = ret_i
		elseif flag == "d" then
			d, str = parse_dict(str)
			if d == nil or str == nil then
				return nil, nil
			end
			dict[key] = d
		else
			s, str = parse_string(str)
			if s == nil or str == nil then
				return nil, nil
			end
			dict[key] = s
		end
		if bak_str == str then
			return nil, nil
		end
		flag = string.sub(str, 1, 1)
	end
	str = string.sub(str, 2)
	return dict, str
end
--d, str = parse_dict("d3:aaali32123e5:tristee")

--d, str = parse_dict("d3:111d2:216:211111e6:222222d1:34:2333e4:4444d6:244444l2:444:5555eee")
--d, str = parse_dict("d3:111d2:216:211111ee")
--d, str = parse_dict("de")
--d, str = parse_dict("d3:123d2:124:1234ee")

function torrent_parser.parse_torrent_file(file)
	fp = io.open(file, "rb")	
	torrent_buf = fp:read("*all")
	fp:close()
	d, str = parse_dict(torrent_buf)
	return d
	--table_print(d)
end

return torrent_parser
--torrent_info = parse_torrent_file("test.torrent")
--t_p.table_print(torrent_info)

--[[
for b in string.gfind(d["info"]["pieces"], ".") do
	io.write(string.format("%02X ", string.byte(b)))
end
]]--
