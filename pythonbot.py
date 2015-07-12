# Python version 2.7
from os import path
from glob import glob
from subprocess import Popen
from importlib import import_module
import threading
import socket
import Bot

# Load bots, launch the game and bots.
def launch_game():
  # Detect bots.
  botList = glob(path.join('Bots/', '*.py'))
  botList = [bot[5:-3] for bot in botList if bot != 'Bots/__init__.py'] # Remove 'Bots/' and '.py' in the names, and __init__.py from the names.
  print 'Bots detected : {}'.format(botList)
  
  # Import the bots and check that they compile.
  botModuleList = []
  open('Bots/__init__.py', 'w').close() # Create a __init__.py file in the Bots/ directory (to get import_module() function working).
  for bot in list(botList): # Iterate over a copy to be able to remove elements during the iteration process.
    try:
      botModule = import_module('Bots.' + bot) # Import the bot python file.
    except Exception, exception:
      print 'Error : Problem with bot {} : {}'.format(bot, exception)
      botList.remove(bot)
    else:
      botModuleList.append(botModule)

  # Launch the game.
  port = get_free_port()
  args = ['./pythonbot_core', '{}'.format(port), '{}'.format(len(botList))]
  print 'Lauching {} {} {}'.format(args[0], args[1], args[2])
  Popen(args)

  # Launch the bots.
  threads = []
  for i in range(0, len(botList)):
    thread = threading.Thread(None, load_bot, 'pythonbot_bot_{}'.format(i), (botModuleList[i], port, botList[i]))
    thread.start()
    threads.append(thread)

  # Wait for the bots to finish.
  for thread in threads:
    thread.join()
  print 'All threads joined'

# Return a free port on localhost. Note that another process could bind a socket to that port after it is closed.
def get_free_port():
  tcpSocket = socket.socket()
  tcpSocket.bind(('localhost', 0))
  port = tcpSocket.getsockname()[1]
  tcpSocket.close()
  return port

# Load a bot from the python file in 'botName' and launch it.
def load_bot(botModule, port, botName):
  nameMaxLength = 10
  if len(botName) > nameMaxLength: # Cut the name if too long.
    Bot.safe_print('Warning : Bot name too long ({}) truncated to {}'.format(botName, botName[:nameMaxLength]))
    botName = botName[:nameMaxLength]
  bot = Bot.Bot(port, botName) # Create the bot.
  try:
    botModule.main(bot) # Run the bot !
  except socket.error, exception: # The exception is raised when the socket is closed on server's side.
    Bot.safe_print('Bot {} shut down'.format(botName))
    del bot
  except Exception, exception: # Unexpected error (usually function main missing).
    Bot.safe_print('Error : Problem with bot {} : {}'.format(botName, exception))
    del bot

if __name__ == '__main__':
  launch_game()
