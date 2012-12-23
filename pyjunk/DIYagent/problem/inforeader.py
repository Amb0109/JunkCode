import os, sys, re

def getVal(filename, inforoot, infokey):
	if not os.path.exists(filename):
		raw_input("Error: file \"%s\" not exists!" % filename)
		sys.exit()

	infofile = open(filename, 'r')
	info = infofile.read()
	info = re.search('<\s*' + inforoot + '.*?/>', info, re.S)
	if info == None:
		raw_input("Error: root %s not exists!" % inforoot)
		sys.exit()
	
	info = info.group()
	val = re.compile(infokey + '\s*=\s*\".*?\"').search(info)
	if val == None:
		raw_input("Error: key %s not exists!" % infokey)
		sys.exit()

	val = val.group()
	val = re.search(r'\".*?\"', val).group()[1:-1]
	return val


def getText(filename, infokey):
	if not os.path.exists(filename):
		raw_input("Error: file \"%s\" not exists!" % filename)
		sys.exit()

	infofile = open(filename, 'r')
	info = infofile.read()
	info = re.search('<\s*' + infokey + '\s*>.*?</\s*' + infokey + '\s*>', info, re.S)

	if info == None:
		raw_input("Error: key %s not exists!" % infokey)
		sys.exit()

	info = info.group()
	text = re.search('>.*?</', info, re.S).group()[1:-2]
	return text


def getPairs(filename, inforoot, infokey, infoval):
	if not os.path.exists(filename):
		raw_input("Error: file \"%s\" not exists!" % filename)
		sys.exit()

	infofile = open(filename, 'r')
	infotext = infofile.read()
	
	pairs = {}

	infos = re.findall(r'<\s*' + inforoot + ' .*?/>', infotext)
	for info in infos:
		_key = re.search(infokey + '\s*=\s*\".*?\"', info)
		if not _key: continue
		key = re.search('\".*?\"', _key.group()).group()[1:-1]

		_val = re.search(infoval + '\s*=\s*\".*?\"', info)
		if not _val: continue
		val = re.search('\".*?\"', _val.group()).group()[1:-1]

		pairs[key] = val
	
	return pairs
