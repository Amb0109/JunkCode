'''
Created on 2011-12-19

@author: Amb
'''

import pygame
import Game
import Draw

pygame.init()

def initgame(game_info):
    global game, draw
    game = Game.game(game_info)
    draw = Draw.draw(game_info)
    game.initcore()
    draw.initdraw(game)

def run(pos):
    global fclick, isover
    global start_pos, target_pos
    if pos[0] >= game_info[0][0] or pos[1] >= game_info[0][1]: return
    if fclick:
        if game.getcolor(pos) != -1:
            start_pos = pos
            fclick = False
            draw.circle(start_pos, game.getcolor(start_pos), 1)
    else:
        if game.getcolor(pos) != -1:
            if pos == start_pos:
                fclick = True
                draw.circle(start_pos, game.getcolor(start_pos))
            else:
                pre = start_pos
                draw.circle(pre, game.getcolor(pre), 0)
                start_pos = pos
                draw.circle(start_pos, game.getcolor(start_pos), 1)
        else:
            target_pos = pos
            path = game.getpath(start_pos, target_pos)
            if path != []:
                col = game.getcolor(target_pos)
                draw.path(start_pos, path, col)
                clock.tick(40)
            else: return
            fclick = True
            clear_arr = game.clearmap()
            if clear_arr == []:
                draw.circles(game.fillballs())
                game.randcolor()
                draw.tips(game.gettip())
                draw.grids(game.clearmap())
            else:
                draw.grids(clear_arr)
            draw.score(game.getscore())

game_info=[[10,10],5,4,3]
initgame(game_info)
fclick = True
isover = False
ispause = False
level_pos = []

clock = pygame.time.Clock()

end = False
while end == False:
    clock.tick(10)
    for event in pygame.event.get():
        if event.type == pygame.QUIT: end = True
        if event.type == pygame.MOUSEBUTTONDOWN:
            point = pygame.mouse.get_pos()
            if ispause:
                if pygame.Rect(level_pos[0]).collidepoint(point):
                    draw.initdraw(game)
                    ispause = False
                    continue
                elif pygame.Rect(level_pos[1]).collidepoint(point):
                    game_info=[[10,10],5,4,3]
                    ispause = False
                    isover = True
                elif pygame.Rect(level_pos[2]).collidepoint(point):
                    game_info=[[12,12],6,4,4]
                    ispause = False
                    isover = True
                elif pygame.Rect(level_pos[3]).collidepoint(point):
                    game_info=[[16,16],8,4,4]
                    ispause = False
                    isover = True
                else: continue
            menu_pos = draw.getmenupos()
            if isover or pygame.Rect(menu_pos[0]).collidepoint(point):
                game.updatascore()
                initgame(game_info)
                isover = False
                fclick = True
                continue
            elif pygame.Rect(menu_pos[1]).collidepoint(point):
                ispause = True
                level_pos = draw.level()
                game.updatascore()
            elif pygame.Rect(menu_pos[2]).collidepoint(point):
                end = True
            else:
                pos = draw.getpos(point)
                run(pos)
    if game.isover():
        isover = True
        draw.isover()
        game.updatascore()

game.updatascore()
pygame.quit()


