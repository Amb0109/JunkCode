from urllib import urlencode
from urllib2 import *
from cookielib import CookieJar
from random import randint
from time import localtime, time, strftime

nowtime = time()

host = "acm.hdu.edu.cn"

cookiejar = CookieJar()
cookieproc = HTTPCookieProcessor(cookiejar)
urlOpener = build_opener(cookieproc)

# login
loginurl = "http://" + host + "/userloginex.php?action=login&cid=0&notice=0"
loginval = {"username":"ambition0109", "userpass":"bx1035920299", "login":"Sign In"}
urlOpener.open(Request(loginurl, urlencode(loginval)))

#new DIY
newdiyurl = "http://" + host + "/diy/diy_makecontest.php?action=make&cid=0"
diyname = "So easy" #str(randint(10000000, 99999999))
diypass = "while(true);"#str(randint(100000, 999999))
tmptime = localtime(nowtime+100000)
newdiyval = {
		"contestname": diyname,
		"syear":strftime("%Y", tmptime),
		"smonth":strftime("%m", tmptime),
		"sday":strftime("%d", tmptime),
		"shour":strftime("%H", tmptime),
		"sminute":strftime("%M", tmptime),
		"ssecond":strftime("%S", tmptime),
		"lday":"1",
		"lhour":"0",
		"lminute":"0",
		"contesttype":"1",
		"password":diypass,
		"gpp":"1",
		"gcc":"2",
		"cpp":"4",
		"c":"8",
		"pascal":"16",
		"java":"32"}

res = urlOpener.open(Request(newdiyurl, urlencode(newdiyval))).read()
cid = re.compile(r'cid=[0-9]*\">' + diyname).search(res).group()
cid = re.compile(r'=[0-9]*\"').search(cid).group()[1:-1]

#Add empty problem
addprourl = "http://" + host + "/diy/diy_makeyourproblem.php?action=make&cid=" + cid +"&pid="
for pidcnt in range(1001,1031):
	addproval ={
		"pid":"%04d" % pidcnt,
		"timelimit":"1000",
		"memorylimit":"32768",
		"outputlimit":"256",
		"javatimelimit":"3000",
		"javamemorylimit":"65535",
		"author":"",
		"source":"",
		"title":"...",
		"content":"",
		"input":"",
		"output":"",
		"sampleinput":"",
		"sampleoutput":"",
		"submit":"Submit"}
	urlOpener.open(Request(addprourl, urlencode(addproval)))

upddiyurl = "http://" + host + "/diy/diy_makecontest.php?action=update&cid=" + cid
tmptime = localtime(nowtime)
upddiyval = {
		"contestname": diyname,
		"syear":strftime("%Y", tmptime),
		"smonth":strftime("%m", tmptime),
		"sday":strftime("%d", tmptime),
		"shour":strftime("%H", tmptime),
		"sminute":strftime("%M", tmptime),
		"ssecond":strftime("%S", tmptime),
		"lday":"25",
		"lhour":"0",
		"lminute":"0",
		"contesttype":"1",
		"password":diypass,
		"gpp":"1",
		"gcc":"2",
		"cpp":"4",
		"c":"8",
		"pascal":"16",
		"java":"32"}
urlOpener.open(Request(upddiyurl, urlencode(upddiyval)))

f = open("diycontest.info", "a")
print >>f, '<diy name=\"%s\" pass=\"%s\" cid=\"%s\" />' % (diyname, diypass, cid)
print 'diy: name = \"%s\" pass = \"%s\" cid = \"%s\"' % (diyname, diypass, cid)
