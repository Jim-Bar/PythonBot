def main(bot):
  surroundingsAcceptable = False
  while not surroundingsAcceptable:
    redirect(bot)
    find_wall(bot)
    surroundingsAcceptable = evaluate_surroundings(bot)
  bot.rotate(135)
  snipe(bot)

def evaluate_surroundings(bot):
  surroundings = 0
  for i in range(0, 72):
    scan = bot.scan(25, 3, True, False, False)
    surroundings += min(scan[0], 1)
    bot.rotate(5)
  if surroundings > 27:
    return True
  else:
    return False

def redirect(bot):
  done = False
  while not done:
    scan = bot.scan(20, 16, True, False, False)
    if scan[0] == 0:
      done = True
    else:
      bot.rotate(1)

def find_wall(bot):
  done = False
  while not done:
    scan = bot.scan(5, 14, True, True, False)
    if scan[1] > 0:
      bot.rotate(-5)
    elif scan[0] > 0:
      done = True
    else:
      bot.move()

def snipe(bot):
  while True:
    for i in range(0, 20):
      botPresent = True
      while botPresent:
        scan = bot.scan((i + 1) * 5, 8 + i / 2, False, True, False)
        if scan[1] == 0:
          botPresent = False
        else:
          bot.fire()
      