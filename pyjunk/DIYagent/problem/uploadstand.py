import urllib, urllib2, cookielib
from inforeader import getVal
import re

cid = getVal("init.info", "diy", "cid")
pid = getVal("init.info", "problem", "pid")

username = 'amb'
userpass = '1035920299bx'
diypass = getVal("init.info", "diy", "pass")

cookiejar = cookielib.CookieJar()
cookieproc = urllib2.HTTPCookieProcessor(cookiejar)
urlOpener = urllib2.build_opener(cookieproc)

# login
loginurl = "http://acm.hdu.edu.cn/userloginex.php?action=login&cid=0&notice=0"
loginval = {"username": username, "userpass": userpass, "login":"Sign In"}
urlOpener.open(urllib2.Request(loginurl, urllib.urlencode(loginval)))

# enterdiy
diyurl = "http://acm.hdu.edu.cn/diy/contest_login.php?action=login&cid=" + cid
diyval = {"password":diypass, "submit":"Enter"}
print urlOpener.open(urllib2.Request(diyurl, urllib.urlencode(diyval))).read

# submit
submiturl = "http://acm.hdu.edu.cn/diy/contest_submit.php?action=submit&cid=" + cid
code = open("standard.cpp", "r").read()
code = re.sub("freopen", "//freopen", code)
submitval = {"problemid":pid, "language":"0", "usercode":code}
print urlOpener.open(urllib2.Request(submiturl, urllib.urlencode(submitval))).read()
