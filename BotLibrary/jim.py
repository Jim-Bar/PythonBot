def main(bot):
  while True:
    rotate(bot)
    move(bot)

def rotate(bot):
  for i in range(0, 150):
    bot.rotate(11)
    for i in range(0, 10):
      botCount = 1
      obstacleCount = 0
      while botCount > 0:
        scanResult = bot.scan(10 + i * 30, 10, True, True, False)
        obstacleCount = scanResult[0]
        botCount = scanResult[1]
        if botCount > 0:
          bot.fire()
      if obstacleCount > 0:
        break

def move(bot):
  for i in range(0, 150):
    scanResult = bot.scan(20, 12, True, True, False)
    if scanResult[0] > 0 or scanResult[1] > 0:
      bot.rotate(15)
    else:
      bot.move()