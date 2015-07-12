def main(bot):
  while True:
    if bot.health > 5:
      bot.fire()
    elif bot.bullets < 5:
      bot.rotate(11)
    else:
      bot.rotate(-11)