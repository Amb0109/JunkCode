'''
Created on 2011-12-30

@author: Amb
'''

import pygame
import Game
import Menu

pygame.init()

screen = pygame.display.set_mode([600,440])
pygame.display.set_caption('Snake')

game = Game.game()
game.init()

gamepad = pygame.Surface([400,400])
Game.draw(gamepad,game.snake,game.food)
screen.blit(gamepad,[20,20])

sidebar = pygame.Surface([150,400])
Menu.draw(sidebar, [0,0])
screen.blit(sidebar,[450,20])

pygame.display.flip()

clock = pygame.time.Clock()

level_speed = [500,350,200,100,50,10]
score_prefd = [10,15,20,25,30,35]

level = 0
pre_step = level_speed[0]
time_pass = 0
isover = False
count = 0
score = 0

def init():
    global level,score,count
    global pre_step,time_pass
    global isover
    level = 0
    pre_step = level_speed[0]
    time_pass = 0

    isover = False
    count = 0
    score = 0

print sidebar.get_rect()
Menu.draw(sidebar, [level+1, score])
pygame.display.update([450,20,150,400])

end = False
while end == False:
    for event in pygame.event.get():
        if event.type == pygame.QUIT: end = True
    
    pressed_keys = pygame.key.get_pressed()
    if isover and pressed_keys[pygame.K_SPACE]:
        init()
        game.init()
        Game.draw(gamepad,game.snake,game.food)
        screen.blit(gamepad,[20,20])
        pygame.display.flip()
        clock.tick()
        continue
    
    if isover: continue
 
    if pressed_keys[pygame.K_UP]:
        game.setdir(game.D_UP)
    elif pressed_keys[pygame.K_DOWN]:
        game.setdir(game.D_DOWN)
    elif pressed_keys[pygame.K_LEFT]:
        game.setdir(game.D_LEFT)
    elif pressed_keys[pygame.K_RIGHT]:
        game.setdir(game.D_RIGHT)
    
    time_pass += clock.tick(30)    
    if time_pass > pre_step:
        retval = game.run()
        if retval == -1:
            time_pass = 0
            isover = True
        else:
            Game.draw(gamepad,game.snake,game.food)
            screen.blit(gamepad,[20,20])
            pygame.display.update([20,20,400,400])
            time_pass -= pre_step
            if retval == 1:
                count += 1
                score += score_prefd[level]
            if count >= 10:
                count -= 10
                level += 1
                pre_step = level_speed[level]
        Menu.draw(sidebar,[level+1,score])
        screen.blit(sidebar,[450,20])
        pygame.display.update([450,20,150,400])
                

pygame.quit()