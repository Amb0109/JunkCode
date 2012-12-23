'''
Created on 2012-1-6

@author: Amb
'''

''' some useful color'''

BLACK = (0,0,0)
WHITE = (255,255,255)

color = [
       (240,64,31),
       (174,26,147),
       (244,142,24),
       (47,182,204),
       (159,194,61),
       (28,96,177),
       (211,211,28),
       (52,147,45),
       (90,47,190),
       (59,205,132),
       (255,106,5),
       (108,158,12)]

def colex(col,  deg):
    '''
    deg <= 100 and deg >= -100
    if deg < 0: light color
    if deg > 0: deep color
    '''
    
    if deg == 0: pass
    elif deg < 0:
        for c in [0,1,2]:
            col[c] -= (255 - col[c])*deg/100
    elif deg > 0:
        for c in [0,1,2]:
            col[c] -= col[c]*deg/100
    return col
    
def gery(deg):
    ''' black 0 <= deg <= 100 white '''
    
    if deg < 0: return BLACK
    if deg > 100: return WHITE
    
    return [2.55*deg,2.55*deg,2.55*deg]
    
def alpha(col, deg):
    ''' nothing 0 <= deg <= 100 color '''
    
    col.append(2.55*deg)
    return col
    