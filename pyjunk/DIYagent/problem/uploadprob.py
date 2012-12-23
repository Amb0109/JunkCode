from urllib import urlencode
from urllib2 import *
from cookielib import CookieJar
from inforeader import getVal, getText

#login
cookiejar = CookieJar()
cookieproc = HTTPCookieProcessor(cookiejar)
urlOpener = build_opener(cookieproc)

loginurl = "http://acm.hdu.edu.cn/userloginex.php?action=login&cid=0&notice=0"
loginval = {"username":"ambition0109", "userpass":"bx1035920299", "login":"Sign In"}
urlOpener.open(Request(loginurl, urlencode(loginval)))

#read file
infofile = 'problem.info'

cid = getVal(infofile, 'cid', 'cid')
pid = getVal(infofile, 'pid', 'pid')

proval = {}
proval['pid'] =					pid
#proval['title'] =				getVal(infofile, 'title', 'title')
proval['title'] =				"<h1>" + getVal(infofile, 'title', 'title') + "</h1>"
proval['timelimit'] =			getVal(infofile, 'timelimite', 'timelimite')
proval['javatimelimit'] =		getVal(infofile, 'timelimite', 'javatimelimite')
proval['memorylimit'] =			getVal(infofile, 'memorylimite', 'memorylimite')
proval['javamemorylimit'] = 	getVal(infofile, 'memorylimite', 'javamemorylimite')
proval['outputlimit'] =			getVal(infofile, 'outputlimite', 'outputlimite')
proval['author'] =				getVal(infofile, 'author', 'author')
proval['source'] =				getVal(infofile, 'source', 'source')
proval['content'] =				getText(infofile, 'description')
proval['input'] =				getText(infofile, 'input')
proval['output'] =				getText(infofile, 'output')
proval['sampleinput'] =			getText(infofile, 'sampleinput')
proval['sampleoutput'] =		getText(infofile, 'sampleoutput')
proval['submit'] = 				"Submit"

#print proval
prourl = "http://acm.hdu.edu.cn/diy/diy_makeyourproblem.php?action=update&cid=" + cid + "&pid=" + pid
urlOpener.open(Request(prourl, urlencode(proval)))

raw_input("I have finished my work!")
