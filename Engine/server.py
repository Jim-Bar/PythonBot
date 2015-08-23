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
import os # To construct cross-compatible paths.
import socket
import pythonbot
from time import sleep

# Server configuration.
host = '127.0.0.1' # We are the host.
port = 5006 # Port to listen to.
bufferSize = 256 # Maximum data we will receive.

# Create a new TCP socket.
tcpListener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)    

try:
  tcpListener.bind((host, port)) # Bind the socket to the address (double parenthesis build a tuple).
  
  while True:
    tcpListener.listen(5) # Allow at most 5 queued connections.
    print('Listening on 0.0.0.0 port {}...'.format(port))

    tcpSocket, address = tcpListener.accept() # Accept the connection.
    print('A client just connected: {}'.format(address))
    botCode = ''
    botCode = tcpSocket.recv(bufferSize).decode()
    if botCode == '':
      print('Received empty string ! Closing down.')
    else:
      print('Received:\n"\n{}\n"\nfrom {}'.format(botCode, address))
    #tcpSocket.close()
    print('Connection with {} closed'.format(address))
    
    try:
      os.remove(os.path.join('Bots', 'imported.pyc')) # Remove the previously compiled bot file.
    except OSError:
      pass # If the file does not exists, no need to delete it.
    botFile = open(os.path.join('Bots', 'imported.py'), 'w')
    botFile.write(botCode)
    botFile.close()
    pythonbot.launch_game(tcpSocket)
except KeyboardInterrupt:
  tcpListener.close()
  print('Exited')
