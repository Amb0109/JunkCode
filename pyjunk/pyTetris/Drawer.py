'''
Created on 2012-1-6

@author: Amb
'''

import pygame
import Colors
import Blocks
import os

RECTH = 20
RECTW = 20

def draw_rect(scr, col, rect):
    ''' draw a rect at pos with color[col]
    col is an int val
    '''
    
    scr.fill(col,rect)
    pygame.draw.rect(scr,Colors.WHITE,rect,1)

def block_rect(ph,pw):
    ''' get the rect of the block
    the w and h changed
    '''
    return [pw*RECTW, ph*RECTH, RECTW, RECTH]

def draw_blocks(scr, size, gamemap, block = None, blockpos = None):
    scr.fill(Colors.WHITE)
    for ph in xrange(size[0]):
        for pw in xrange(size[1]):
            if gamemap[ph][pw] == -1: continue
            draw_rect(scr, Colors.color[gamemap[ph][pw]], block_rect(ph,pw))

    if block != None and blockpos != None:
        shape = Blocks.getshape(block)
        for tph in range(Blocks.BLOCKH):
            for tpw in range(Blocks.BLOCKW):
                if shape[tph][tpw] == 0: continue
                ph = blockpos[0] + tph
                pw = blockpos[1] + tpw
                if ph < 0 or ph > size[0]: continue
                if pw < 0 or pw > size[1]: continue
                draw_rect(scr, Colors.color[block[0]], block_rect(ph,pw))

def draw_tips(scr,block):
    scr.fill(Colors.WHITE)
    brect = Blocks.getminirect(block)
    lh = brect[2] - brect[0] + 1
    lw = brect[3] - brect[1] + 1
    sh = (Blocks.BLOCKH - lh) * RECTH / 2 + 10
    sw = (Blocks.BLOCKW - lw) * RECTW / 2 + 10
    shape = Blocks.getshape(block)
    for th in range(lh):
        for tw in range(lw):
            if shape[brect[0] + th][brect[1] + tw] == 0: continue
            rect = [sw + tw * RECTW, sh + th * RECTH, RECTW, RECTH]
            draw_rect(scr, Colors.color[block[0]], rect)

# init font
font = None
fontcol  = Colors.color[11]

pygame.font.init()
    
if 'playbill' in pygame.font.get_fonts():
    font = pygame.font.SysFont('playbill', 28)
else:
    if os.path.isfile('PLAYBILL.TTF'):
        font = pygame.font.Font('PLAYBILL.TTF',28)
    else:
        print 'Font not find!'
        
#-----------------------------

def draw_info(scr, info):
    
    if font == None: return
    
    scr.fill(Colors.BLACK)
    
    level = info[0]
    line = info[1]
    score = info[2]
    
    frender = font.render('Lv.:', True, fontcol,Colors.BLACK)
    scr.blit(frender,[0,0])
    
    frender = font.render('Ln.:', True, fontcol,Colors.BLACK)
    scr.blit(frender,[0,30])
    
    frender = font.render('Sc.:', True, fontcol,Colors.BLACK)
    scr.blit(frender,[0,60])
    
    frender = font.render('%8d' % level, True, fontcol,Colors.BLACK)
    scr.blit(frender,[30,0])
    
    frender = font.render('%8d' % line, True, fontcol,Colors.BLACK)
    scr.blit(frender,[30,30])
    
    frender = font.render('%8d' % score, True, fontcol,Colors.BLACK)
    scr.blit(frender,[30,60])


















