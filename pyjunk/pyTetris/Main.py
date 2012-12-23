'''
Created on 2012-1-5

@author: Amb
'''

import pygame
import Tetris

pygame.init()

screen = pygame.display.set_mode([500,440])
pygame.display.set_caption('Tetris')

clock = pygame.time.Clock()

tetris = Tetris.Tetris(screen)
tetris.start()
pygame.display.flip()

status = Tetris.Tetris.PAUSE
while status != Tetris.Tetris.EXIT:
        
    timedif = clock.tick(25)
    
    status = tetris.run(timedif)
    
    pygame.display.flip()
        
pygame.quit()

