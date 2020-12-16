def main(bot):
    count = 100
    while True:
        scan_result = bot.scan(18, 10, True, True, False)
        if scan_result[0] == 0 and scan_result[1] == 0:
            bot.move()
            if count == 0:
                count = 100
                for i in range(0, 10):
                    bot.fire()
            count -= 1
        else:
            bot.rotate(10)
