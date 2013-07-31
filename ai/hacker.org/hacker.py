from requests import post
from subprocess import Popen, PIPE
from re import search

game = 'oneofus'
name = 'Amb'
pswd = 'hacker@amb'
post_url = 'http://www.hacker.org/%s/index.php' % (game)
print post_url

gametype0 = ("coil", "runaway", "push", "oneofus", "brick")
gametype1 = ("cross")
gametype2 = ("tapeworm")
gametype3 = ("modulo")

class player():

	def __init__(self):
		self.AI = None


	def __del__(self):
		if not self.connected(): self.AI.kill()


	def get(self, exefile):
		''' Try to connect a exe file. '''

		try:
			self.AI = Popen(exefile, stdout=PIPE, stdin=PIPE, stderr=PIPE)
		except: return False
		return not self.AI.poll()


	def connected(self):
		''' Return the status. '''
		return self.AI != None and self.AI.poll() == None


	def send(self, msg):
		''' Send a msg. '''

		self.AI.stdin.write(msg + "\n")


	def recv(self):
		''' Recv a msg. '''
		
		return self.AI.stdout.readline()[:-2]


def read(url, payload):
#def read(url):
	content = post(url, data = payload).text
	#content = get(url).text
	#print >>open("content.log", "w"), content
	try:
		gvars = None
		if game in gametype0:
			gvars_pre = search(r"FlashVars=\".*?\"", content).group()
			gvars = search(r"\".*?\"", gvars_pre).group()[1:-1]
		elif game in gametype1:
			gvars_pre = search(r"boardinit.*?\".*?\"", content).group()
			gvars_a = search(r"\".*?\"", gvars_pre).group()[1:-1]
			gvars_pre = search(r"level.*?;", content).group()
			gvars_b = search(r"[0-9]+", gvars_pre).group()
			gvars = gvars_a + " " + gvars_b
		elif game in gametype2:
			gvars_pre = search(r"boardString.*?\".*?\"", content).group()
			gvars_a = search(r"\".*?\"", gvars_pre).group()[1:-1]
			gvars_pre = search(r"wormString.*?\".*?\"", content).group()
			gvars_b = search(r"\".*?\"", gvars_pre).group()[1:-1]
			gvars_pre = search(r"curLevel.*?;", content).group()
			gvars_c = search(r"[0-9]+", gvars_pre).group()
			gvars = gvars_a + " " + gvars_b + " " + gvars_c
		elif game in gametype3:
			gvars_pre = search(r"depth.*?\".*?\"", content).group()
			gvars_a = search(r"\".*?\"", gvars_pre).group()[1:-1]
			gvars_pre = search(r"board.*?\".*?\"", content).group()
			gvars_b = search(r"\".*?\"", gvars_pre).group()[1:-1]
			gvars_pre = search(r"pieces.*?\".*?\"", content).group()
			gvars_c = search(r"\".*?\"", gvars_pre).group()[1:-1]
			gvars = gvars_a + " " + gvars_b + " " + gvars_c
		return gvars
	except: return None


if __name__ == "__main__":
	

	pl = player()
	while not pl.connected():
		#fi = raw_input("EXEFILE: ")
		fi = "oneofus242"
		pl.get(fi)

	now_url = post_url
	payload = {"name":name, "password":pswd}
	while True:
		case = read(now_url, payload)
		while case == None:
			print "Error! Wrong answer or internet Fall"
			now_url = post_url
			payload = {"name":name, "password":pswd}
			case = read(now_url, payload)

		print "[:", case, ":]"

		pl.send(case)
		ans = pl.recv()

		#now_url = post_url + '&' + ans
		payload['path'] = ans[5:]
		print payload


