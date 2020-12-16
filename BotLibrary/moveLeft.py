def main(bot):
    while True:
        scan_result = bot.scan(20, 12, True, True, False)
        if scan_result[0] > 0 or scan_result[1] > 0:
            bot.rotate(-15)
        else:
            bot.move()
