from subprocess import *
from time import clock
import sys


class renju:
	''' The renju_simple game control. '''

	def __init__(self):
		self.N = 15
		self.dir_x = (1, 1, 0, 1)
		self.dir_y = (0, 1, 1, -1)

	def init(self):
		self.bd = [[0 for x in xrange(self.N)] for y in xrange(self.N)]
		self.turn = 0

	def inboard(self, x, y):
		''' Check a pos is in board or not. '''

		if x<0 or x>=self.N: return False
		if y<0 or y>=self.N: return False
		return True

	def pia(self, x, y, flg):
		if not self.inboard(x, y): return False
		if self.bd[x][y] != 0: return False
		self.bd[x][y] = flg
		self.turn += 1
		return True

	def win(self, x, y):
		for d in xrange(4):
			for k in xrange(5):
				lx = x-k*self.dir_x[d]
				ly = y-k*self.dir_y[d]
				rx = x+(4-k)*self.dir_x[d]
				ry = y+(4-k)*self.dir_y[d]
				if not self.inboard(lx, ly): continue
				if not self.inboard(rx, ry): continue
				for s in xrange(0, 5):
					tx = lx+s*self.dir_x[d]
					ty = ly+s*self.dir_y[d]
					if self.bd[tx][ty] != self.bd[x][y]: break
					if s==4: return True
		return False

	def tie(self):
		return self.turn == 255

	def show(self):
		''' Show the board. '''

		for x in xrange(self.N):
			line = ""
			for y in xrange(self.N):
				if self.bd[x][y]==0: line += "."
				elif self.bd[x][y]==1: line += "B"
				elif self.bd[x][y]==2: line += "W"
			print line



class player():

	def __init__(self):
		self.isconnected = False

	def __del__(self):
		if self.isconnected == True: self.AI.kill()

	def get(self, FILE):
		''' Try to connect a exe file. '''

		try:
			AI_FILE = FILE + ".exe"
			self.AI = Popen(AI_FILE, stdout=PIPE, stdin=PIPE, stderr=PIPE)
			self.isconnected = True
		except: pass
		return self.isconnected

	def connected(self):
		''' Return the status. '''

		return self.isconnected

	def send(self, msg):
		''' Send a msg. '''

		self.AI.stdin.write(msg + "\n")

	def recv(self):
		''' Recv a msg. '''
		
		return self.AI.stdout.readline()[:-2]


def init_players():
	''' Init the players. '''

	pl_B = player()
	pl_W = player()

	if len(sys.argv) >= 2:
		pl_B.get(sys.argv[1])

	while not pl_B.connected():
		FILE_NAME = raw_input("Black player: ")
		pl_B.get(FILE_NAME)

	if len(sys.argv) >= 3:
		pl_W.get(sys.argv[2])

	while not pl_W.connected():
		FILE_NAME = raw_input("White player: ")
		pl_W.get(FILE_NAME)

	return {"B":pl_B, "W":pl_W}


def main():

	pl = init_players()

	game = renju()

	game.init()
	game.show()

	BLACK_TIME = 0
	WHITE_TIME = 0

	BLACK_LINE = []
	WHITE_LINE = []

	pre_time = clock()

	pl["B"].send("first")
	pl["W"].send("second")

	while True:
		BLACK_LINE.append(pl["B"].recv())
		now_time = clock()

		BLACK_TIME += now_time-pre_time
		print BLACK_LINE[-1], "||", (now_time-pre_time)*1000, "ms"

		x, y = BLACK_LINE[-1].split(' ')
		if game.pia(int(x), int(y), 1): pass
		else:
			print "Black player>> %s %s illegal move." % (x, y)
			break

		game.show()
		if game.win(int(x), int(y)):
			print "Black player win!"
			break
		elif game.tie():
			print "Game TIE!"
			break

		pre_time = clock()
		pl["W"].send(BLACK_LINE[-1])

		WHITE_LINE.append(pl["W"].recv())
		now_time = clock()

		WHITE_TIME += now_time-pre_time
		print WHITE_LINE[-1], "||", (now_time-pre_time)*1000, "ms"

		x, y = WHITE_LINE[-1].split(' ')
		if game.pia(int(x), int(y), 2): pass
		else:
			print "White player>> %s %s illegal move." % (x, y)
			break

		game.show()
		if game.win(int(x), int(y)):
			print "White player win!"
			break
		elif game.tie():
			print "Game TIE!"
			break

		pl["B"].send(WHITE_LINE[-1])

	print "BLACK_TIME:", BLACK_TIME*1000, "ms"
	print "WHITE_TIME:", WHITE_TIME*1000, "ms"

	return (BLACK_LINE, WHITE_LINE, BLACK_TIME, WHITE_TIME)


if __name__ == "__main__":
	#print main()
	main()
