# -*- encoding: utf-8 -*-

import urllib2
import re
from time import sleep
import md5

def get_host(url):
	if url == None: return None

	if re.match(r"http://", url): url = url[7:]
	split_url = url.split("/")
	res_url = split_url[0]
	
	return res_url

def get_dir(url):
	if url == None: return None

	if re.match(r"http://", url): url = url[7:]
	split_url = url.split("/")
	if len(split_url) == 1: res_url = split_url[0]
	else: res_url = "/".join(split_url[:-1])
	return res_url + '/'


def norm_url(url, pre_url = None):
	if url == None or url == "": return None

	if re.match(r"javascript", url): return None

	re_s = re.search(r"#", url)
	if re_s: url = url[:re_s.start()]
	
	if url == "": return None
	if not re.match(r"http://", url):
		if pre_url != None:
			if url[0] == '/': url = get_host(pre_url) + url
			else: url = get_dir(pre_url) + url
		url = "http://" + url

	if url[-1] == '/': url = url[:-1]
	return url


def grap(url):
	if url == None: return None

	hash_md5 = md5.new()
	hash_md5.update(url)
	htmlfile = open(hash_md5.hexdigest() + ".html", "w")

	try:
		msg = urllib2.urlopen(url).read()
		print url, ", Ok"
	except Exception:
		print url, ", Error"
		return None

	#sleep(1)
	print >>htmlfile, msg
	
	return msg

def analysis(html, url):
	if url == None: return None
	if html == None: return None

	hash_md5 = md5.new()
	hash_md5.update(url)
	logfile = open(hash_md5.hexdigest() + ".log", "w")

	re_tag_a = r"<a.*?href=.*?>"
	re_href = r"href=\".*?\""

	#print html
	res_tag_a = re.findall(re_tag_a, html, re.S)

	res_href = []

	for tag in res_tag_a:
		re_s = re.search(re_href, tag, re.S)
		if re_s:
			res_url = norm_url(re_s.group()[6:-1], url)
			if res_url != None:
				res_href.append(res_url)
				print >>logfile, res_url

	return res_href


def BFS(url):
	url = norm_url(url)
	if url == None:
		print "Error!"
		return None

	que = [url]
	vis = []

	while len(que) != 0:
		now_url = que.pop(0)
		if now_url in vis: continue
		vis.append(now_url)
		next = analysis(grap(now_url), now_url)
		if next != None:
			que = list(set(que+next))


url = raw_input("URL:")
BFS(url)
