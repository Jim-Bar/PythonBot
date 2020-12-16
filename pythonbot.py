# PythonBot - A game by a developer for developers.
# Copyright (C) 2015 Jean-Marie BARAN (jeanmarie.baran@gmail.com)

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# Refer to 'LICENSE.txt' for the full notice.


# Python version 2.7 or 3.3+
from os import path # To construct cross-compatible paths.
from os import getcwd # To get the current working directory (cross-compatible paths again).
from glob import glob
from subprocess import Popen
from subprocess import check_output
from importlib import import_module
import threading
import socket
import Bot

# Load bots, launch the game and bots.
def launch_game():
  # Detect bots.
  botList = glob(path.join('Bots', '*.py'))
  botList = [bot[5:-3] for bot in botList if bot != path.join('Bots', '__init__.py')] # Remove 'Bots/' and '.py' in the names, and __init__.py from the names.
  print('Bots detected : {}'.format(botList))
  
  # Import the bots and check that they compile.
  botModuleList = []
  open(path.join('Bots', '__init__.py'), 'w').close() # Create a __init__.py file in the Bots/ directory (to get import_module() function working).
  for bot in list(botList): # Iterate over a copy to be able to remove elements during the iteration process.
    try:
      botModule = import_module('Bots.' + bot) # Import the bot python file.
    except Exception as exception:
      print('Error : Problem with bot {} : {}'.format(bot, exception))
      botList.remove(bot)
    else:
      botModuleList.append(botModule)

  # Exchange ports, and launch the engine.
  tcpListener = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Prepare the socket which will receive the port on which the engine is listening for bots.
  tcpListener.bind(('localhost', 0))
  contactPort = tcpListener.getsockname()[1] # Get the port on which the engine will send its port (see above).
  args = [path.join(getcwd(), 'pythonbot_core'), '-n', '{}'.format(len(botList)), '-c', '{}'.format(contactPort)] # Prepare command to execute to start the engine.
  print('Lauching {}'.format(' '.join(args)))
  Popen(args) # Start the engine !
  
  # Receive the port to which bots will talk.
  tcpListener.listen(1) # Only one client will connect (the engine), no need for queueing.
  contactSocket = tcpListener.accept()[0] # Accept the connection.
  botPort = int(contactSocket.recv(6).decode()) # Receive the port.
  contactSocket.close()
  print('Received port for bots : {}'.format(botPort))
  
  # Launch the bots.
  threads = []
  for i in range(0, len(botList)):
    thread = threading.Thread(None, load_bot, 'pythonbot_bot_{}'.format(i), (botModuleList[i], botPort, botList[i]))
    thread.start()
    threads.append(thread)  

  # Wait for the bots to finish.
  for thread in threads:
    thread.join()
  print('All threads joined')

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
  except socket.error as exception: # The exception is raised when the socket is closed on server's side.
    Bot.safe_print('Bot {} shut down'.format(botName))
    del bot
  except Exception as exception: # Unexpected error (usually function main missing).
    Bot.safe_print('Error : Problem with bot {} : {}'.format(botName, exception))
    del bot

if __name__ == '__main__':
  launch_game()
