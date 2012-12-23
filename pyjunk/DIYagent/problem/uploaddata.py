from urllib import urlencode
from urllib2 import *
from cookielib import CookieJar
from multipart import Multipart, Part
from inforeader import getVal
import os

infile = 'data.in'
outfile = 'data.out'
cid = getVal("init.info", "diy", "cid")
pid = getVal("init.info", "problem", "pid")
url = 'http://acm.hdu.edu.cn/diy/diy_uploaddata.php?action=upload&cid=' + cid + '&pid=' + pid

body = Multipart()

intext = open(infile, "rb").read()
outtext = open(outfile, "rb").read()

inpart = Part({"name":"inputfile", "filename":infile}, intext, "text/plain")
outpart = Part({"name":"outputfile", "filename":outfile}, outtext, "text/plain")

body.attach(inpart)
body.attach(outpart)

request = Request(url)
request.add_data(str(body))
(header, value) = body.header()
request.add_header(header, value)

cookiejar = CookieJar()
cookieproc = HTTPCookieProcessor(cookiejar)
urlOpener = build_opener(cookieproc)

# login
loginurl = "http://acm.hdu.edu.cn/userloginex.php?action=login&cid=0&notice=0"
loginval = {"username":"ambition0109", "userpass":"bx1035920299", "login":"Sign In"}
urlOpener.open(Request(loginurl, urlencode(loginval)))

#print >>open("uploaddata.multipart", "w"), str(body)
urlOpener.open(request)

raw_input("I have finished my work!")
