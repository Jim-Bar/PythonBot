Creating your bot is very simple. What you need is to create a python file named by the name of you bot (for example myBot.py).
Inside this file, create a function 'main' that takes one argument 'bot' :

def main(bot):

The object 'bot' gives you access to five functions :
- skip() : do nothing
- move() : advance your bot by one unit of distance
- rotate(angle) : rotate your bot by 'angle' degrees (not radians)
- fire() : fire, but only if you have some bullets left. Note that this function returns the number of bullets that your bot currently dispose.
- scan(distance, radius, scanCircles, scanBots, scanBullets) : Err, this one is harder. Basically scan the map in a circle to see if there are enemy bots, bullets, or just walls.
If for instance I want to move forward, I write : bot.move()

Description of the 'scan' function :
'distance' is the relative distance from your bot to the center of the circle you're scanning.
'radius' is the radius of this very circle.
'scanCircles' is a boolean that tells if walls (and wall disks) should be scanned.
'scanBots' is the same but for the bots. Note that dead bots nor your bot are scanned.
'scanBullets' is also the same, but for enemys' bullets.
This function returns a triplet which contains respectively :
1 - The number of walls (if there is at least one wall, you probably want to rotate)
2 - The number of enemy bots
3 - The number of enemy bullets

And that's it ! Just build your AI from these functions, and don't forget that you can look at the given bots to find inspiration ;)


Here is the most basic bot you can build (does nothing) :
def main(bot):
  while True:
    bot.skip()