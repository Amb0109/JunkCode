'''
Created on 2012-1-4

@author: Amb
'''


import pygame

import Colors
import Blocks
import Drawer


pygame.init()

class game:
    
    ROTATE = 1
    DOWN = 2
    LEFT = 3
    RIGHT = 4
    STEPDOWN = 0
        
    def __init__(self,size):
        self.hide = 2
        self.size = [size[0] + self.hide,size[1]]
        self.map = []
        self.shapen = 10
        self.isover = False
        for pw in xrange(self.size[0]):
            self.map.append([])
            for ph in xrange(self.size[1]):
                self.map[pw].append(-1)
        
        self.level = 1
        self.line = 0
        self.score = 0
    
    def start(self, level):
        self.isover = False
        
        self.next = Blocks.randblock(self.shapen)
        self.now = Blocks.randblock(self.shapen)
        self.nowpos = [0,self.size[1]/2-2]
        for ph in xrange(self.size[0]):
            for pw in xrange(self.size[1]):
                self.map[ph][pw] = -1
                
        self.level = level
        self.line = 0
        self.score = 0
  
    def freeze(self):
        shape = Blocks.getshape(self.now)
        for tph in range(Blocks.BLOCKH):
            for tpw in range(Blocks.BLOCKW):
                if shape[tph][tpw] == 0: continue
                ph = self.nowpos[0] + tph
                pw = self.nowpos[1] + tpw
                self.map[ph][pw] = self.now[0]
    
    def run(self,msg):
        if msg == self.LEFT:
            self.nowpos[1] -= 1
            if self.__errmove__():
                self.nowpos[1] += 1
        elif msg == self.RIGHT:
            self.nowpos[1] += 1
            if self.__errmove__():
                self.nowpos[1] -= 1
        
        elif msg == self.STEPDOWN:
            self.nowpos[0] += 1
            if self.__errmove__():
                self.nowpos[0] -= 1
                self.freeze()
                self.newblock()
        elif msg == self.ROTATE:
            self.now = Blocks.rotate(self.now)
            if self.__errmove__():
                self.now = Blocks.undorotate(self.now)
        elif msg == self.DOWN:
            self.nowpos[0] += 1
            while not self.__errmove__():
                self.nowpos[0] += 1
            else:
                self.nowpos[0] -= 1
            self.freeze()
            self.newblock()
        self.clearline()
    
    def newblock(self):
        if self.isover: return
        self.now = self.next
        self.next = Blocks.randblock(self.shapen)
        self.nowpos = [0,self.size[1]/2-2]
        if self.__errmove__(): self.isover = True
    
    def checkover(self):
        return self.isover
    
    def clearline(self):
        k = 0
        for i in xrange(self.size[0]-1,-1,-1):
            i += k
            isfull = True
            for j in xrange(self.size[1]):
                if self.map[i-k][j] == -1: isfull = False
                self.map[i][j] = self.map[i-k][j]
            if isfull: k += 1
        if k != 0:
            self.line += k
            self.score += k*10
            if k == 2: self.score += k*2
            elif k == 3: self.score += k*5
            elif k == 4: self.score += k*10
            elif k == 5: self.score += k*20
            if (self.line - 1)/20 > self.level - 1:
                self.level += 1
            
    def getlevel(self):
        return self.level
                
    def __errmove__(self):
        shape = Blocks.getshape(self.now)
        for tph in range(Blocks.BLOCKH):
            for tpw in range(Blocks.BLOCKW):
                if shape[tph][tpw] == 0: continue
                if self.nowpos[0]+tph < 0: return True
                if self.nowpos[1]+tpw < 0: return True
                if self.nowpos[0]+tph >= self.size[0]: return True
                if self.nowpos[1]+tpw >= self.size[1]: return True
                if self.map[self.nowpos[0]+tph][self.nowpos[1]+tpw] != -1: return True
        return False
    
    def drawblocks(self,scr):
        drawsize = [self.size[0]-self.hide,self.size[1]]
        drawpos = [self.nowpos[0]-self.hide, self.nowpos[1]]
        Drawer.draw_blocks(scr, drawsize, self.map[self.hide:], self.now, drawpos)
    
    def drawtips(self,scr):
        Drawer.draw_tips(scr, self.next)
    
    def drawinfo(self,scr):
        Drawer.draw_info(scr, [self.level,self.line,self.score])
        
    

    
