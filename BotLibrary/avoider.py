def main(bot):
    while True:
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
