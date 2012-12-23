'''
Created on 2011-12-30

@author: Amb
'''

import random
import pygame
#import pygame._view

pygame.init()

white = [255,255,255]
snkcol = [80,80,80]
fodcol = [255,0,0]

def draw(scr,snake,food):
    scr.fill(white)
    lw=scr.get_width()/20
    lh=scr.get_height()/20
    for pos in snake:
        pygame.draw.rect(scr,snkcol,[pos[0]*lw,pos[1]*lh,lw,lh])
    for pos in food:
        pygame.draw.rect(scr,fodcol,[pos[0]*lw,pos[1]*lh,lw,lh])

class game:
    _dir = [[1,0],[0,1],[-1,0],[0,-1]]
    D_UP = 3
    D_DOWN = 1
    D_LEFT = 2
    D_RIGHT = 0
    
    def __init__(self):
        self.snake = []
        self.food = []
    
    def init(self):
        self.snake = []
        self.food = []
        for i in range(6):
            self.snake.append([i,19])
        self.len = 5
        self.pre_dir = 0
        self.now_dir = 0
        self.setfood()
    
    def setfood(self):
        while True:
            px = random.randint(0,19)
            py = random.randint(0,19)
            if [px,py] in self.snake: continue
            self.food.append([px,py])
            break
    
    def setdir(self,_dir):
        if (self.pre_dir+2)%4 != _dir:
            self.now_dir = _dir
    
    def run(self):
        self.pre_dir = self.now_dir
        pos = [self.snake[self.len][0],self.snake[self.len][1]]
        pos[0] += self._dir[self.now_dir][0]
        pos[1] += self._dir[self.now_dir][1]
        if pos[0] < 0 or pos[0] >= 20: return -1
        if pos[1] < 0 or pos[1] >= 20: return -1
        tail = self.snake[0]
        self.snake.pop(0)
        if pos in self.snake: return -1
        self.snake.append(pos)
        if pos in self.food:
            self.food.remove(pos)
            self.setfood()
            self.snake.insert(0,tail)
            self.len += 1
            return 1
        return 0
    
    
    
