import urllib, urllib2, cookielib, re, time
import json

server_url = "http://nat-traversal.tplinkcloud.com:5000/download/"
server_login = "http://nat-traversal.tplinkcloud.com:5000/login"
save_dir = "./data"

def get_full_server_url():
	return server_url

def post(url, data):
	req = urllib2.Request(url)
	data = urllib.urlencode(data)
	opener = urllib2.build_opener(urllib2.HTTPCookieProcessor())
	response = opener.open(req, data)
	return response.headers['Set-Cookie']

def login_server(url, user_name, password):
	data = {
		'username' : user_name,
		'password' : password,
	}
	return post(url, data)

def get_year_month_info(buf):
	"""<a href="..">..</a><br><a href="/download/201511/">201511/</a>"""
	tmp = re.findall(buf, '/.*/')
	if tmp is not None:
		print(tmp.__dict__)
def get_url(url, cookie):
	g = urllib2.Request(url)
	opener = urllib2.build_opener(urllib2.HTTPCookieProcessor())
	g.add_header('Cookie', cookie)
	g.add_header('User-Agent', 'Mozilla/5.0 (Windows NT 5.1; rv:12.0) Gecko/20100101 Firefox/12.0')
	g.add_header('Connection', 'keep-alive')
	g.add_header('Referer', server_url + "201511")
	g.add_header('Content-Type', 'application/x-www-form-urlencoded')
	resp = opener.open(g)
	return resp.read()

""" return list of files name """
def parse_files_dir(buf):
	print(buf)
	"""href="/download/201511/23/022516-26007189-IP"""
	tmp_re = re.findall("href=\"(.+?)\"", buf)
	return tmp_re

def get_single_day_files(single_day, cookie):
	files_dir_buf = get_url(server_url + "201511/" + single_day + '/', cookie)
	files_name_list = parse_files_dir(files_dir_buf)
	for name in files_name_list:
		if name == "..":
			continue
		tmp_name = re.findall("/.+/(.+?)$", name)
		if tmp_name is not None:
			full_name = server_url + "201511/" + single_day + '/' + tmp_name[0]
			buf = get_url(full_name, cookie)
			if buf is not None:
				fd = open( save_dir + tmp_name[0], "a+")
				fd.write(buf)
				fd.close()
def get_user_info(bufline):
"""{"Email":"tim.xiang@tp-link.com","Country":"Singapore","City":"Adjuneid","ISP":"Starhub","Network":"Office Network","Vendor":"D-LINK","Model":"DIR-868L","WAN":"27.54.61.88"}"""



if __name__ == '__main__':
	print "hello world"
	cookie = login_server(server_login,'tplink', 'smartlife')
	get_single_day_files("23", cookie)
	get_single_day_files("24", cookie)







