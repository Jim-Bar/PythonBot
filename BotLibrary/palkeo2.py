from __future__ import division
import math
import random

def main(bot):
    p = Palkeo(bot)
    p.play()
        
class Palkeo:
    def __init__(self, bot):
        self.bot = bot
        self.dist_min = 0
        self.dist_max = 500
        self.scan_ba = 0
    
    def play(self):
        def sign(a):
            if a >= 0:
                return 1
            return -1

        while True:
            self.radius = self.nearest_dist()
            a = self.angle_scan(-30+self.scan_ba, 30+self.scan_ba)
            if a is not None:
                self.bot.rotate(a[0])
                angle = 0
                ia = 10*math.asin(1/self.radius)*360/(2*math.pi)
                while self.bot.fire():
                    angle = (angle + ia*sign(angle)) * -1
                    self.bot.rotate(angle)
                self.bot.rotate(90 - angle)
                self.scan_ba -= a[0] + 90 - angle
            else:
                self.scan_ba = (self.scan_ba + 220) % 360
            
            if sum(self.bot.scan(30, 30, True, True, False)[0:2]) > 0:
                self.bot.rotate(random.randint(0,360))
            else:
                for i in range(15):
                    self.bot.move()
        
    def nearest_dist(self):
        while self.bot.scan(0, self.dist_min, False, True, False)[1] > 0 and self.dist_min > 1:
            self.dist_min = max(1, self.dist_min - 50)
        while self.bot.scan(0, self.dist_max, False, True, False)[1] == 0 and self.dist_max < 500:
            self.dist_max += 50
        d = 0
        while self.dist_max - self.dist_min > 5:
            d = (self.dist_min + self.dist_max) // 2
            r = self.bot.scan(0, d, False, True, False)
            if r[1] == 0:
                self.dist_min = d
            else:
                self.dist_max = d
        return d
    
    def angle_scan(self, amin, amax):
        r = 2*self.radius*math.sin((amax-amin)/4 *(2*math.pi)/360) + 2
        if r < 10:
            return None
        
        cmax = (3*amax+amin)/4
        cmin = (amax+3*amin)/4
        cmoy = (amin+amax)/2
        
        self.bot.rotate(cmax)
        fmax = self.bot.scan(self.radius, int(r), False, True, False)[1]
        self.bot.rotate(-cmax+cmin)
        fmin = self.bot.scan(self.radius, int(r), False, True, False)[1]
        self.bot.rotate(-cmin)
        
        def merge(a, b):
            if a is None:
                return b
            elif b is None:
                return a
            if a[1] < b[1]:
                return a
            else:
                return b
        
        result = None
        if fmax > 0:
            result = merge((cmoy, r * math.sqrt(fmax)), self.angle_scan(cmoy, amax))
        if fmin > 0:
            result = merge((cmoy, r * math.sqrt(fmin)), self.angle_scan(amin, cmoy))
        
        return result
