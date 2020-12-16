def main(bot):
    while True:
        rotate(bot)
        move(bot)


def rotate(bot):
    for i in range(0, 150):
        bot.rotate(11)
        for j in range(0, 10):
            bot_count = 1
            obstacle_count = 0
            while bot_count > 0:
                scan_result = bot.scan(10 + j * 30, 10, True, True, False)
                obstacle_count = scan_result[0]
                bot_count = scan_result[1]
                if bot_count > 0:
                    bot.fire()
            if obstacle_count > 0:
                break


def move(bot):
    for i in range(0, 150):
        scan_result = bot.scan(20, 12, True, True, False)
        if scan_result[0] > 0 or scan_result[1] > 0:
            bot.rotate(15)
        else:
            bot.move()
