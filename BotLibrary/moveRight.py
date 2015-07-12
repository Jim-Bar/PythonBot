def main(bot):
  while True:
    scanResult = bot.scan(20, 12, True, True, False)
    if scanResult[0] > 0 or scanResult[1] > 0:
      bot.rotate(15)
    else:
      bot.move()
