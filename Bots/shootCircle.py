from random import randint

def main(bot):
  while True:
    for i in range(0, 10):
      bot.fire()
      bot.rotate(3)
    for i in range(0, 30):
      bot.rotate(randint(-90, 90))
      scan = bot.scan(40, 45, True, True, False)
      if scan[0] > 0 or scan[1] > 0:
	bot.rotate(randint(0, 360))
      else:
	for i in range(0, 20):
	  bot.move()