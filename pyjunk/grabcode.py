#!/usr/bin/env python
# -*- encoding: utf-8 -*-

import requests
import re, sys, codecs

host = "acm.hdu.edu.cn"
username = ""
userpass = ""

s = requests.session()
def login():
    loginurl = "http://" + host + "/userloginex.php?action=login&cid=0&notice=0"
    loginval = {"username": username, "userpass": userpass, "login": "Sign In"}
    try:
        res = s.post(loginurl, loginval).text
    except requests.exceptions.ConnectionError:
        raw_input("Error! Internet connect Error")
        sys.exit()
    
    if re.search("No such user...", res, re.S):
        raw_input("Error! Username or Password is wrong")
        sys.exit()

def logout():
    logouturl = "http://" + host + "/userloginex.php?action=logout"
    try:
        s.get(logouturl, allow_redirects=False)
    except requests.exceptions.ConnectionError:
        raw_input("Error! Internet connect Error")
        sys.exit() 
    
def readcode(rid):
    codeurl = "http://" + host + "/viewcode.php?rid=" + str(rid)
    try:
	    html = s.get(codeurl).text
    except:
        raw_input("Error! Internet connect Error")
        return
    
    if re.search("No such solution", html):
        raw_input("Error! No such solution or you have no privilege to view this code")
        return
    
    code = re.search("usercode.*?<", html, re.S).group()
    code = re.search(">.*?<", code, re.S).group()[1:-1]
    
    code = re.sub("&lt;", "<", code)
    code = re.sub("&gt;", ">", code)
    code = re.sub("&amp;", "&", code)
    code = re.sub("&quot;", "\"", code)
    code = re.sub("\r", "", code)

    _ext = re.search("Language.*?&nbsp;", html, re.S).group()
    if re.search("C[+][+]", _ext):
        ext = "c.cpp"
    elif re.search("G[+][+]", _ext):
        ext = "g.cpp"
    elif re.search("GCC", _ext):
        ext = "g.c"
    elif re.search("C", _ext):
        ext = "c.c"
    elif re.search("Java", _ext):
        ext = "java"
    codecs.open("%s.%s" % (rid, ext), "w", "utf-8").write(code)


if __name__ == "__main__":
    login()
    
    #print sys.argv
    if len(sys.argv) > 1:
        for rid in sys.argv[1:]:
            try:
                readcode(rid)
            except: print "Error! Unknown error"
    
    while True:
        try:
            rid = raw_input("rid>> ")
            if rid == "": continue
        except: break
        
        try:
            readcode(rid)
        except: print "Error! Unknown error"
    
    logout()
