from random import randint


def main(bot):
    while True:
        bot.rotate(randint(5, 355))
        bot.fire()
