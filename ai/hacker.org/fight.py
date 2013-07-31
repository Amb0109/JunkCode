import urllib2
import re, random

flog = open('Dungeon.log','w')

name = "Amb"
pswd = "hacker@amb"
game = "cavern"
url = 'http://www.hacker.org/challenge/misc/d/' + game \
	+'.php?name='+ name +'&password=' + pswd

pat = ['m=e','m=s','m=n','m=w']
_url = url
while True:
	msg = urllib2.urlopen(_url).read()
	
	if re.compile('tres=1').search(msg):
		_url = url + '&tres=1'
		print 'pick up a treasure'
		print >>flog,'pick up a treasure'
		continue

	if re.compile('m=d').search(msg):
		lmsg = re.compile('Dungeon Level [0-9]*').search(msg).group()[14:]
		dglv = int(lmsg)
		lmsg = re.compile('weapon[^0-9]{0,25}[0-9]*',re.S).search(msg).group()[25:]
		mylv = int(lmsg)
		print mylv, dglv
		if mylv > dglv+3:
			_url = url + '&m=d'
			print 'down stairs'
			print >>flog,'down stairs'
			continue
 
	can = []
	for p in pat:
		if re.compile(p).search(msg):
			can.append(p)

	if len(can) != 0:
		mov = can[random.randint(0,len(can)-1)]
		print mov
		print >>flog,mov
		_url = url + '&' + mov
		continue

	if re.compile('attack=1').search(msg):
		if re.compile('Big monster.').search(msg):
			_url = url + '&attack=1'
			print 'attack'
			print >>flog,'attack'
		else:
			print 'The Boss'
			print >>flog, 'The Boss'
			print msg
			print >>flog,msg
			_url = url + '&attack=1'
		continue
	
	_url = url
	print 'reset...'
	print >>flog,'reset...'

