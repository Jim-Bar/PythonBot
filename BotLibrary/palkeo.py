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
        self.dist_max = 800
        self.radius = 0
        self.scan_ba = 0

    def play(self):
        while True:
            self.radius = self.nearest_dist()
            a = self.angle_scan(-30 + self.scan_ba, 30 + self.scan_ba)
            if a is not None:
                self.bot.rotate(a[0])
                while self.bot.fire():
                    pass
                self.bot.rotate(90)
                self.scan_ba -= a[0] + 90
            else:
                self.scan_ba = (self.scan_ba + 220) % 360
            
            if sum(self.bot.scan(30, 30, True, True, False)[0:2]) > 0:
                self.bot.rotate(random.randint(0, 360))
            else:
                for i in range(15):
                    self.bot.move()

    def nearest_dist(self):
        while self.bot.scan(0, self.dist_min, False, True, False)[1] > 0 and self.dist_min > 10:
            self.dist_min -= 30
        while self.bot.scan(0, self.dist_max, False, True, False)[1] == 0 and self.dist_max < 800:
            self.dist_max += 30
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
        r = 2 * self.radius * math.sin((amax - amin) / 4 * (2 * math.pi) / 360) + 2
        if r < 5:
            return None

        cmax = (3 * amax + amin) // 4
        cmin = (amax + 3 * amin) // 4
        cmoy = (amin + amax) // 2

        self.bot.rotate(cmax)
        fmax = self.bot.scan(self.radius, r, False, True, False)[1]
        self.bot.rotate(-cmax + cmin)
        fmin = self.bot.scan(self.radius, r, False, True, False)[1]
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
            result = merge((cmoy, r), self.angle_scan(cmoy, amax))
        if fmin > 0:
            result = merge((cmoy, r), self.angle_scan(amin, cmoy))

        return result

    # def angle_scan(self, amin, amax):
    #     a = self.brute_scan((amax - amin) // 2)
    #     if a is not None:
    #         return a
    #     a = self.brute_scan(amin)
    #     if a is not None:
    #         return a
    #     a = self.brute_scan(amax)
    #     if a is not None:
    #         return a
    #
    # def brute_scan(self, cmoy):
    #     r = 15
    #     self.bot.rotate(cmoy)
    #     fmax = self.bot.scan(self.radius, r, False, True, False)[1]
    #     self.bot.rotate(-cmoy)
    #     if fmax > 0:
    #         return cmoy, r
    #     return None
