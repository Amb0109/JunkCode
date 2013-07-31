from subprocess import *
from time import clock, sleep
import sys


class othello:
	''' The othello game control. '''

	def __init__(self):
		self.N = 8
		self.dir_x = (1, 1, 1, 0, 0, -1, -1, -1)
		self.dir_y = (0, -1, 1, -1, 1, 0, -1, 1)

		self.BLACK = 1
		self.WHITE = 2

	def init(self):
		''' Init the game. '''
		
		self.bd = [[0 for x in xrange(self.N)] for y in xrange(self.N)]
		self.bd[3][3] = self.bd[4][4] = self.BLACK
		self.bd[3][4] = self.bd[4][3] = self.WHITE

	def inboard(self, x, y):
		''' Check a pos is in board or not. '''

		if x<0 or x>=self.N: return False
		if y<0 or y>=self.N: return False
		return True

	def pia(self, x, y, flg):
		''' Do a move. Return the move is valid. '''

		if not self.valid(flg): return True

		if not self.inboard(x, y): return False
		if self.bd[x][y] != 0: return False

		flip = False
		for d in xrange(8):
			tx = x+self.dir_x[d]
			ty = y+self.dir_y[d]
			if not self.inboard(tx, ty): continue
			if self.bd[tx][ty] != 3-flg: continue

			while self.inboard(tx, ty) and self.bd[tx][ty]==3-flg :
				tx += self.dir_x[d]
				ty += self.dir_y[d]

			if self.inboard(tx, ty) and self.bd[tx][ty]==flg:
				flip = True
				tx -= self.dir_x[d]
				ty -= self.dir_y[d]
				while self.inboard(tx, ty) and self.bd[tx][ty] == 3-flg:
					self.bd[tx][ty] = flg
					tx -= self.dir_x[d]
					ty -= self.dir_y[d]

		if flip:
			self.bd[x][y] = flg
			return True
		else: return False

	def valid(self, flg):
		''' Check the board is there some valid position for the player.
		Black: flg = 1
		White: flg = 2
		'''

		for x in xrange(self.N):
			for y in xrange(self.N):
				if self.bd[x][y] != 0: continue

				for d in xrange(8):
					tx = x+self.dir_x[d]
					ty = y+self.dir_y[d]
					if not self.inboard(tx, ty): continue
					if self.bd[tx][ty] != 3-flg: continue
					while self.inboard(tx, ty) and self.bd[tx][ty]==3-flg:
						tx += self.dir_x[d]
						ty += self.dir_y[d]

					if self.inboard(tx, ty) and self.bd[tx][ty]==flg: return True
		return False

	def over(self):
		''' Check the board wether the game is over. '''
		empty = black = white = 0
		for x in xrange(self.N):
			for y in xrange(self.N):
				if self.bd[x][y] == 0: empty += 1
				if self.bd[x][y] == self.BLACK: black += 1
				if self.bd[x][y] == self.WHITE: white += 1

		if empty * black * white == 0: return True
		else:
			if self.valid(self.BLACK): return False
			if self.valid(self.WHITE): return False
			return True

	def result(self):
		''' If the game is end, Show the game result. '''

		black = white = 0
		for x in xrange(self.N):
			for y in xrange(self.N):
				if self.bd[x][y] == self.BLACK: black += 1
				elif self.bd[x][y] == self.WHITE: white += 1
		
		res = ""
		if black > white: res += "Black win. "
		elif black < white: res += "White win. "
		else: res += "Game Tie. "

		res += "B: %d  W: %d" % (black, white)
		return res

	def show(self):
		''' Show the board. '''

		for x in xrange(self.N):
			line = ""
			for y in xrange(self.N):
				if self.bd[x][y] == 0: line += "."
				elif self.bd[x][y] == self.BLACK: line += "B"
				elif self.bd[x][y] == self.WHITE: line += "W"
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
	''' main function '''

	player = init_players()

	game = othello()

	game.init()
	#game.show()

	BLACK_TIME = 0
	WHITE_TIME = 0

	BLACK_LINE = []
	WHITE_LINE = []

	pre_time = clock()

	player["B"].send("first")
	player["W"].send("second")

	while True:

		# BLACK PLAYER

		BLACK_LINE.append(player["B"].recv())
		now_time = clock()

		x, y = BLACK_LINE[-1].split(' ')
		if game.pia(int(x), int(y), game.BLACK): pass
		else:
			#print "Black player>> %s %s illegal move." % (x, y)
			break

		BLACK_TIME += now_time - pre_time
		#print BLACK_LINE[-1], "||", (now_time-pre_time)*1000, "ms"

		#game.show()
		if game.over():
			print game.result()
			break

		pre_time = clock()
		player["W"].send(BLACK_LINE[-1])


		# WHITE PLAYER

		WHITE_LINE.append(player["W"].recv())
		now_time = clock()

		x, y = WHITE_LINE[-1].split(' ')
		if game.pia(int(x), int(y), game.WHITE): pass
		else:
			#print "White player>> %s %s illegal move." % (x, y)
			break

		WHITE_TIME += now_time - pre_time
		#print WHITE_LINE[-1], "||", (now_time-pre_time)*1000, "ms"

		#game.show()
		if game.over():
			print game.result()
			break

		pre_time = clock()
		player["B"].send(WHITE_LINE[-1])

	print "BLACK_TIME>>", BLACK_TIME*1000, "ms"
	print "WHITE_TIME>>", WHITE_TIME*1000, "ms"

	return (BLACK_LINE, WHITE_LINE, BLACK_TIME, WHITE_TIME)


if __name__ == "__main__":
	#print main()
	main()

