'''
Created on 2011-12-29

@author: Administrator
'''

import pygame
#from pygame import _view

#graph
black = (0, 0, 0)
white = (255, 255, 255)
background = (100, 100, 100)
font_col = (192, 80, 77)

color = [[(31, 73, 125), (192, 80, 77), (155, 187, 89), (128, 100, 162), (75, 172, 198), (247, 150, 70), (255, 241, 59), (232, 12, 122)], \
        [(135, 176, 225), (227, 176, 175), (217, 229, 193), (202, 190, 216), (185, 223, 233), (252, 211, 178), (255, 248, 160), (231, 120, 169)]]

xlen = 30
ylen = 30
glen = 2
rlen = 12

class draw:
    def __init__(self, game_info):
        pygame.init()
        
        self.lx = game_info[0][0]
        self.ly = game_info[0][1]
        self.tipn = game_info[3]
        
        sizex = (xlen + glen) * self.lx + 50
        sizey = (ylen + glen) * (self.ly + self.tipn) + 40
        self.screen = pygame.display.set_mode([sizey, sizex])
        
        pygame.display.set_caption('Row 5')
        
        self.font = pygame.font.Font("PLAYBILL.TTF", 28)

    def getpos(self, pos):
        px = (pos[0] - glen - 20) / (xlen + glen)
        py = (pos[1] - glen - 20) / (ylen + glen)
        return [px, py]
    
    def getmenupos(self):
        return self.menu_pos;
    
    def rectangle(self, pos):
        px = (xlen + glen) * pos[0] + glen + 20
        py = (ylen + glen) * pos[1] + glen + 20
        pygame.draw.rect(self.screen, white, [px, py, xlen, ylen])
        pygame.display.update([px, py, xlen, ylen])
    
    def grids(self, pos = -1):
        if pos == -1:
            for i in range(self.lx):
                for j in range(self.ly):
                    self.rectangle([i, j])
        else:
            for _pos in pos:
                self.rectangle(_pos);
    
    def circle(self, pos, col, islight=0):
        if col == -1: return
        px = (xlen + glen) * pos[0] + glen + xlen / 2 + 20
        py = (ylen + glen) * pos[1] + glen + ylen / 2 + 20
        pygame.draw.circle(self.screen, color[islight][col], [px, py], rlen)
        pygame.draw.circle(self.screen, black, [px, py], rlen, 2)
        pygame.display.update([px - xlen / 2, py - ylen / 2, xlen, ylen])
    
    def path(self, spos, path, col):
        clock = pygame.time.Clock()
        pre = spos
        for pos in path:
            clock.tick(60)
            self.circle(pos, col)
            self.rectangle(pre)
            pre = pos
    
    def circles(self, balls):
        for ball in balls:
            self.circle(ball[0],ball[1])
    
    def tips(self, col):
        px = (xlen + glen) * self.lx + xlen / 2
        py = ylen / 2 + 30
        for i in range(self.tipn):
            px += xlen + glen
            pygame.draw.circle(self.screen, color[0][col[i]], [px, py], rlen)
            pygame.draw.circle(self.screen, black, [px, py], rlen, 2)
        px = (xlen + glen) * self.lx
        py = 30
        pygame.display.update([px + xlen + glen,py,(xlen+glen)*self.tipn,ylen])
     
    def score(self, val):
        px = self.screen.get_width() - 90
        py = 80
        score = self.font.render('%8d' % (val[1]), True, font_col, background)
        self.screen.blit(score, [px, py])
        score = self.font.render('%8d' % (val[0]), True, color[0][0], background)
        self.screen.blit(score, [px, py+30])
        pygame.display.update([px,py,90,60])
        
    def menu(self):
        self.menu_pos = []
        
        px = self.screen.get_width() - 90
        py = self.screen.get_height() - 150
        self.menu_pos.append([px, py, 70, 30])
        pygame.draw.rect(self.screen, [80, 80, 80], [px, py, 70, 30])
        self.menu_pos.append([px, py+35, 70, 30])
        pygame.draw.rect(self.screen, [80, 80, 80], [px, py+35, 70, 30])
        self.menu_pos.append([px, py+70, 70, 30])
        pygame.draw.rect(self.screen, [80, 80, 80], [px, py+70, 70, 30])
        
        button = self.font.render('Start', True, font_col)
        self.screen.blit(button, [px+15, py])
        button = self.font.render('Level', True, font_col)
        self.screen.blit(button, [px+15, py+35])
        button = self.font.render('Exit', True, font_col)
        self.screen.blit(button, [px+15, py+70])
        
        pygame.display.update(px,py,90,105)
    
    def level(self):
        menu_pos = []
        pygame.draw.rect(self.screen, background, self.screen.get_rect())
        px = self.screen.get_width() / 2 - 40
        py = self.screen.get_height() / 2 + 60
        menu_pos.append([px, py, 70, 30])
        pygame.draw.rect(self.screen, [80, 80, 80], [px, py, 70, 30])
        py = self.screen.get_height() / 2 - 60
        menu_pos.append([px, py, 70, 30])
        pygame.draw.rect(self.screen, [80, 80, 80], [px, py, 70, 30])
        py = self.screen.get_height() / 2 - 20
        menu_pos.append([px, py, 70, 30])
        pygame.draw.rect(self.screen, [80, 80, 80], [px, py, 70, 30])
        py = self.screen.get_height() / 2 + 20
        menu_pos.append([px, py, 70, 30])
        pygame.draw.rect(self.screen, [80, 80, 80], [px, py, 70, 30])
        
        px = self.screen.get_width() / 2 - 30
        py = self.screen.get_height() / 2 - 60
        button = self.font.render('Easy', True, font_col)
        self.screen.blit(button, [px, py])
        py = self.screen.get_height() / 2 - 20
        button = self.font.render('Normal', True, font_col)
        self.screen.blit(button, [px, py])
        py = self.screen.get_height() / 2 + 20
        button = self.font.render('Hard', True, font_col)
        self.screen.blit(button, [px, py])
        py = self.screen.get_height() / 2 + 60
        button = self.font.render('Cancel', True, font_col)
        self.screen.blit(button, [px, py])
        pygame.display.update()
        return menu_pos
    
    def initdraw(self, core):
        self.screen.fill(background)
        self.grids()
        for i in range(self.lx):
            for j in range(self.ly):
                self.circle([i, j], core.getcolor([i, j]))
        self.tips(core.gettip())
        self.score(core.getscore())
        self.menu()
        pygame.display.update()
    
    def over(self):
        px = (xlen + glen) * self.lx / 2 - 20
        py = (ylen + glen) * self.ly / 2 - 20
        pygame.draw.rect(self.screen, background, [px, py, 100, 50])
        over = self.font.render('Game over', True, font_col, background)
        self.screen.blit(over, [px+10, py+10])
        pygame.display.update()
       
    
