import requests

username = ''
userpass = ''

# login
s = requests.session()
loginurl = "http://acm.hdu.edu.cn/userloginex.php?action=login&cid=0&notice=0"
loginval = {"username": username, "userpass": userpass, "login":"Sign In"}
s.post(loginurl, loginval)

# submit code
suburl = "http://acm.hdu.edu.cn/submit.php?action=submit"
subval = {"problemid":"1000", "language":"1", "usercode":open("code.c","r").read()}


s.post(suburl, subval)

