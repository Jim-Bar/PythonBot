def main(bot):
    avoid_phase(bot)
    find_wall(bot)
    shoot(bot)

# Initial phase : avoid bullets.


def avoid_phase(bot):
    for j in range(0, 100):
        scan = bot.scan(0, 70, False, False, True)
        if scan[2] > 0:
            for i in range(0, 4):
                scan = bot.scan(20, 20, False, False, True)
                if scan[2] > 0 and i != 3:
                    bot.rotate(90)
                else:
                    drive(bot)
                    break


def drive(bot):
    for i in range(0, 50):
        scan = bot.scan(20, 15, True, True, False)
        if scan[0] > 0 or scan[1] > 0:
            bot.rotate(-25)
        else:
            bot.move()

# Second phase : find wall.


def find_wall(bot):
    seek_wall(bot)
    good_position = False
    i = 0
    while not good_position:
        if i > 0:
            seek_wall(bot)
        bot.rotate(-90)
        scan = bot.scan(5, 15, True, False, False)
        bot.rotate(90)
        if scan[0] > 0:
            good_position = True
        i += 1
    bot.rotate(90)  


def seek_wall(bot):
    wall_found = False
    while not wall_found:
        scan = bot.scan(5, 14, True, True, False)
        if scan[1] > 0:
            bot.rotate(-5)
        elif scan[0] > 0:
            wall_found = True
        else:
            bot.move()
    parallelize(bot)


def parallelize(bot):
    i = 0
    while i < 15:
        scan = bot.scan(10, 15, True, False, False)
        if scan[0] == 0:
            bot.move()
            i += 1
        else:
            bot.rotate(2)

# Third phase : shoot.


def shoot(bot):
    while True:
        scan = bot.scan(50, 100, False, True, False)
        if scan[1] > 0:
            search_proximity(bot)
        else:
            search_far_away(bot)


def search_proximity(bot):
    angle = 0
    for j in range(0, 30):
        bot.rotate(11)
        angle += 11
        if angle >= 90:
            bot.rotate(-180)
            angle -= 180
        for i in range(0, 5):
            bot_count = 1
            while bot_count > 0:
                scan_result = bot.scan(10 + i * 30, 10, False, True, False)
                bot_count = scan_result[1]
                if bot_count > 0:
                    bot.fire()
    bot.rotate(-angle)


def search_far_away(bot):
    angle = 0
    for j in range(0, 30):
        bot.rotate(6)
        angle += 6
        if angle >= 70:
            bot.rotate(-140)
            angle -= 140
        for i in range(0, 5):
            bot_count = 1
            num_shoots = 0
            while bot_count > 0 and num_shoots < 10:
                scan_result = bot.scan(180 + i * 30, 20, False, True, False)
                bot_count = scan_result[1]
                if bot_count > 0:
                    bot.fire()
                    num_shoots += 1
                else:
                    break
    bot.rotate(-angle)
