from flickrapi import uploadpic, getToken
from HTMLParser import HTMLParser
from win32com import client
from inforeader import getVal, getPairs
import re, os

imgs = {}
def updateimg():
	global imgs
	if os.path.exists('imgs.info') == False: return
	imgs = getPairs('imgs.info', 'img', 'path', 'url')

def getImg(path):
	print 'image: %s' % (path)
	if path not in imgs.keys():
		print 'uploading...'
		url = uploadpic(path).getURL()
		# there is some decode bug, just do this yet.
		print >>open('imgs.info', 'a'), '<img path=\"%s\" url=\"%s\"/>' % (path, url)
		updateimg()
	print imgs[path]
	return imgs[path]

# init img
getToken(getVal("init.info", "flickr", "token"))
updateimg()

froot = os.getcwd()
wordfilename = os.path.join(froot, 'problem.doc')
htmlfilename = os.path.join(froot, 'problem.html')

# turn the word file to html file
msword = client.Dispatch('Word.Application')
msword.Visible = 0
msword.DisplayAlerts = 0
doc = msword.Documents.Open(wordfilename)
doc.SaveAs(htmlfilename, 10) # wdFormatHTML = 10
doc.Close()
msword.Quit()

debugfile = open('Problem.tmp', "w")
infofile = open('problem.info', 'w')
htmlfile = open(htmlfilename, 'r')
paragraphs = re.findall(r'<p .*?>.*?</p>', htmlfile.read(), re.S)
htmlfile.close()

cid = getVal('init.info', 'diy', 'cid')
pid = getVal('init.info', 'problem', 'pid')

ProblemInfo = {
		'cid':				cid,
		'pid':				pid,
		'title':			'Unknown',
		'timelimite':		'1000',
		'memorylimite':		'32768',
		'javatimelimite':	'2000',
		'javamemorylimite':	'65535',
		'outputlimite':		'5120',
		'author':			'',
		'source':			'',
		'description':		'',
		'hint':				'',
		'input':			'',
		'output':			'',
		'sampleinput':		'',
		'sampleoutput':		'',
		'unknown':			'',
		}

keyRE = {
		'description':		r'\s*problem\s*description\s*?$',
		'input':			r'\s*input\s*?$',
		'output':			r'\s*output\s*?$',
		'sampleinput':		r'\s*sample\s*input\s*?$',
		'sampleoutput':		r'\s*sample\s*output\s*?$',
		'hint':				r'\s*hint\s*?$',
		'author':			r'\s*author\s*?$',
		'source':			r'\s*source\s*?$'
		}

tag = 'title'
for p in paragraphs:
	p = re.sub(r'\n', ' ', p)
	img = re.search(r'<img.*?>', p)
	if img or False:
		#print img.group()
		path = re.search(r'src=\".*?\"', img.group()).group()[5:-1]
		getImg(path)
		imgscr = '[center][img]' + imgs[path] + '[/img][/center]'
		p = re.sub(r'<img.*?>', imgscr, p)
	p = re.sub(r'<.*?>', '', p)
	p = re.sub(r'&nbsp;', '', p)
	p = HTMLParser().unescape(p)
	if p == '': continue

	# get usefully info
	timelimiteRE = re.search(r'time.*?\(java/others?\)', p, re.I)
	memorylimiteRE = re.search(r'memory\s*limite.*?\(java/others?\)', p, re.I)
	
	if timelimiteRE or memorylimiteRE:
		if timelimiteRE:
			tmp = timelimiteRE.group()
			timenum = re.findall(r'[0-9]+', tmp)
			if len(timenum) == 2:
				ProblemInfo['javatimelimite'] = timenum[0]
				ProblemInfo['timelimite'] = timenum[1]
			elif len(timenum) == 1:
				ProblemInfo['javatimelimite'] = str(int(timenum[0])*2)
				ProblemInfo['timelimite'] = timenum[0]
			tag = 'unknown'
	
		if memorylimiteRE:
			tmp = memorylimiteRE.group()
			memorynum = re.findall(r'[0-9]+', tmp)
			if len(memorynum) == 2:
				ProblemInfo['javamemorylimite'] = memorynum[0]
				ProblemInfo['memorylimite'] = memorynum[1]
			elif len(timenum) == 0:
				ProblemInfo['javamemorylimite'] = str(int(memorynum[0])*2)
				ProblemInfo['memorylimite'] = memorynum[0]
			tag = 'unknown'
	else:
		flag = True
		for key, kre in keyRE.items():
			if re.match(kre, p, re.I):
				tag = key
				flag = False
				break
	
		if tag == 'title' or flag:
			if tag == 'title': ProblemInfo[tag] = ''
			ProblemInfo[tag] += p
			if tag not in ('title', 'author', 'source'):
				ProblemInfo[tag] += '\n'
	
	print >>debugfile, p

# write to infofile
print >>infofile, '<cid: cid = \"%s\" />' % (ProblemInfo['cid'])
print >>infofile, '<pid: pid = \"%s\" />' % (ProblemInfo['pid'])
print >>infofile, '<title: title = \"%s\" />' % (ProblemInfo['title'])

print >>infofile, '<timelimite: timelimite = \"%s\" javatimelimite = \"%s\" />' \
	% (ProblemInfo['timelimite'], ProblemInfo['javatimelimite'])
print >>infofile, '<memorylimite: memorylimite = \"%s\" javamemorylimite = \"%s\" />' \
	% (ProblemInfo['memorylimite'], ProblemInfo['javamemorylimite'])

print >>infofile, '<outputlimite: outputlimite = \"%s\" />' % (ProblemInfo['outputlimite'])
print >>infofile, '<author: author = \"%s\" />' % (ProblemInfo['author'])
print >>infofile, '<source: source = \"%s\" />' % (ProblemInfo['source'])
print >>infofile, '<description>%s</description>' % (ProblemInfo['description'])
print >>infofile, '<input>%s</input>' % (ProblemInfo['input'])
print >>infofile, '<output>%s</output>' % (ProblemInfo['output'])
print >>infofile, '<sampleinput>%s</sampleinput>' % (ProblemInfo['sampleinput'])
if ProblemInfo['hint'] != '':
	print >>infofile, '<sampleoutput>%s[hint]%s[/hint]</sampleoutput>' % (ProblemInfo['sampleinput'], ProblemInfo['hint'])
else:
	print >>infofile, '<sampleoutput>%s</sampleoutput>' % (ProblemInfo['sampleoutput'])

if os.path.exists(htmlfilename):
	os.remove(htmlfilename)
if os.path.exists("problem.files"):
	for item in os.listdir("problem.files"):
		itemsrc = os.path.join("problem.files", item)
		os.remove(itemsrc)
	os.rmdir("problem.files")

raw_input("I have finished my work!")
