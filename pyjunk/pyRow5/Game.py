'''
Created on 2011-12-19

@author: Amb
'''

import struct
import random
import Queue

class game:
    def __init__(self, game_info):
        # info: 0.size 1.num of color 2.num of init drops 3.num of round drops
        self.info = game_info
        self.map = [[-1 for y in range(self.info[0][1])] for x in range(self.info[0][0])]
        self.score = 0
        self.empty = self.info[0][0] * self.info[0][1]
        self.tip = []
        
        if self.info[1] == 5: self.level = 0
        elif self.info[1] == 6: self.level = 1
        elif self.info[1] == 8: self.level = 2
        
        self.topscore = self.readdata()
    
    def getcolor(self, pos):
        return self.map[pos[0]][pos[1]]
    
    def getscore(self):
        return [self.score, self.topscore]
    
    def isover(self):
        if self.empty == 0: return True
        else: return False
    
    def gettip(self):
        return self.tip
    
    def randcolor(self, num = -1):
        if num == -1: cnt = self.info[3]
        else: cnt = num
        col = []
        for i in range(cnt):
            col.append(random.randint(0, self.info[1] - 1))
        if num == -1: self.tip = col
        return col
    
    def fill(self, col):
        if self.empty == 0: return [];
        while True:
            rx = random.randint(0, self.info[0][0] - 1)
            ry = random.randint(0, self.info[0][1] - 1)
            if self.map[rx][ry] == -1:
                self.map[rx][ry] = col
                self.empty -= 1
                return [rx, ry]
    
    def fillballs(self, col = -1):
        if col == -1: col = self.tip
        fillarray = []
        for i in col:
            tpos = self.fill(i)
            if tpos == []: return fillarray
            else: fillarray.append([tpos, i])
        return fillarray
    
    def writedata(self, score = -1):
        try:
            if score == -1:
                fout = open('game.dat', 'wb')
                _bytes = struct.pack('iii', 0, 0, 0)
                fout.write(_bytes)
                fout.close()
            else:
                fin = open('game.dat', 'rb')
                _bytes = fin.read()
                fin.close()
                val = [0, 0, 0]
                val[0], val[1], val[2] = struct.unpack('iii', _bytes)
                val[self.level] = score
                _bytes = struct.pack('iii', val[0], val[1], val[2])
                fout = open('game.dat', 'wb')
                fout.write(_bytes)
                fout.close()
        except IOError:
            pass
    
    def readdata(self):
        try:
            fin = open('game.dat', 'rb')
            _bytes = fin.read()
            fin.close()
            val = struct.unpack('iii', _bytes)
            return val[self.level]
        except:
            self.writedata()
            return 0
        
    def updatascore(self):
        self.writedata(self.topscore)
    
    def initcore(self):
        self.fillballs(self.randcolor(self.info[2]))
        self.randcolor(-1)
    
    def getpath(self, spos, tpos):
        pre = [[[-1, -1] for y in range(self.info[0][1])] for x in range(self.info[0][0])]
        _dir = [[1, 0], [0, 1], [-1, 0], [0, -1]]
        que = Queue.Queue();
        que.put(spos)
        pre[spos[0]][spos[1]] = [-2, -2];
        while que.empty() == False:
            upos = que.get()
            for i in range(4):
                vpos = [upos[0] + _dir[i][0], upos[1] + _dir[i][1]]
                if vpos[0] < 0 or vpos[0] >= self.info[0][0]: continue
                if vpos[1] < 0 or vpos[1] >= self.info[0][1]: continue
                if self.map[vpos[0]][vpos[1]] != -1: continue
                if pre[vpos[0]][vpos[1]][0] != -1: continue
                pre[vpos[0]][vpos[1]] = upos
                if vpos == tpos:
                    vx = vpos[0]
                    vy = vpos[1]
                    path = [[vx, vy]]
                    while pre[vx][vy][0] != -2:
                        path.append(pre[vx][vy])
                        vx = pre[vpos[0]][vpos[1]][0]
                        vy = pre[vpos[0]][vpos[1]][1]
                        vpos = [vx, vy]
                    path.reverse()
                    
                    self.map[tpos[0]][tpos[1]] = self.map[spos[0]][spos[1]]
                    self.map[spos[0]][spos[1]] = -1;
                    return path
                que.put(vpos)
        return []
    
    def clearmap(self):
        vis = [[False for y in range(self.info[0][1])] for x in range(self.info[0][1])]
        _dir = [[1, 0], [-1, 0], [0, 1], [0, -1], [1, 1], [-1, -1], [1, -1], [-1, 1]]
        for i in range(self.info[0][0]):
            for j in range(self.info[0][1]):
                if self.map[i][j] == -1: continue
                for k in [0, 2, 4, 6]:
                    tx = i
                    ty = j
                    cnt = 1
                    while True:
                        tx += _dir[k][0]
                        ty += _dir[k][1]
                        if tx < 0 or tx >= self.info[0][0]: break
                        if ty < 0 or ty >= self.info[0][1]: break
                        if self.map[tx][ty] != self.map[i][j]: break
                        cnt += 1
                    if cnt >= 5:
                        while True:
                            tx -= _dir[k][0]
                            ty -= _dir[k][1]
                            if tx < 0 or tx >= self.info[0][0]: break
                            if ty < 0 or ty >= self.info[0][1]: break
                            if self.map[tx][ty] != self.map[i][j]: break
                            vis[tx][ty] = True
        block = []
        for i in range(self.info[0][0]):
            for j in range(self.info[0][1]):
                if vis[i][j]:
                    self.empty += 1
                    self.score += 1
                    block.append([i, j])
                    self.map[i][j] = -1
        if self.score > self.topscore:
            self.topscore = self.score
        return block





