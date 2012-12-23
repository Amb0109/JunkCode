'''
Created on 2012-1-4

@author: Amb
'''

import pygame
import Game

class Tetris(object):
    
    # status
    EXIT = 0
    RUN = 1
    PAUSE = 2
    OVER = 3
    
    def __init__(self, screen):
        ''' The game run from here '''
        
        pygame.init()
        
        self.screen = screen
        
        self.level = 1
        
        self.game = Game.game([20,15]) # h*w = 20*15

        self.board = pygame.Surface([300,400]) # game screen
        self.tipbox = pygame.Surface([100,100]) # tip box
        self.infobox = pygame.Surface([100,100]) # info box
        
        self.status = self.PAUSE
        
        

    def updateboard(self):
        ''' update game '''
        
        self.game.drawblocks(self.board)
        self.screen.blit(self.board,[20,20])
        pygame.display.update(self.board.get_rect())
        
        self.game.drawtips(self.tipbox)
        self.screen.blit(self.tipbox,[350,20])
        pygame.display.update(self.board.get_rect())
        
        self.game.drawinfo(self.infobox)
        self.screen.blit(self.infobox,[350,140])
        pygame.display.update(self.infobox.get_rect())
        
    
    def start(self):
        ''' start the game '''
        
        self.level = 1
        self.updategap = 1000 * 0.8 ** (self.level - 1)
        self.timepass = 0
        
        self.ispressUD = False # if I pressing UP DOWN, this only press once
        self.ispressLR = 0 # the time of I press LEFT RIGHT, this can press many times
        
        self.game.start(self.level)
        self.status = self.RUN
        
        
    def run(self, time):
        ''' RUN FOR EACH FRAME '''
        
        self.timepass += time
        
        for event in pygame.event.get():
            if event.type == pygame.QUIT: self.status = self.EXIT
            elif event.type == pygame.KEYDOWN:
                
                if self.status == self.OVER:
                    self.start()
                    break
            
                if event.key == pygame.K_LEFT:
                    self.ispressLR = 1
                elif event.key == pygame.K_RIGHT:
                    self.ispressLR = 1

                if self.ispressUD == False:
                    self.ispressUD = True
                    if event.key == pygame.K_UP:
                        self.game.run(Game.game.ROTATE)
                    if event.key == pygame.K_DOWN:
                        self.game.run(Game.game.DOWN)
            if event.type == pygame.KEYUP:
                self.ispressUD = False
                self.ispressLR = 0
        
        if self.ispressLR != 0:
            pressedkey = pygame.key.get_pressed()
            if self.ispressLR == 1 or self.ispressLR >= 8:
                if pressedkey[pygame.K_LEFT]: self.game.run(Game.game.LEFT)
                if pressedkey[pygame.K_RIGHT]: self.game.run(Game.game.RIGHT)
            self.ispressLR += 1
        
        if self.timepass > self.updategap:
            self.timepass -= self.updategap
            self.game.run(self.game.STEPDOWN)
            
        if self.level != self.game.getlevel():
            self.level = self.game.getlevel()
            self.updategap = 1000 * 0.8 ** (self.level - 1)
        
        if self.game.checkover():
            self.status = self.OVER
        
        self.updateboard()
        return self.status
                
                