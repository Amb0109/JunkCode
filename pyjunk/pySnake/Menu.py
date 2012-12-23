'''
Created on 2011-12-30

@author: Administrator
'''

import pygame

pygame.init()

myfont = pygame.font.Font("PLAYBILL.TTF", 28)

backcol = [0,0,0]
fontcol = [192, 80, 77]

def draw(scr,info):
    text = myfont.render('Level: %3d' % info[0], True, fontcol,backcol)
    scr.blit(text,[0,0])
    text = myfont.render('Score: %3d' % info[1], True, fontcol,backcol)
    scr.blit(text,[0,30])