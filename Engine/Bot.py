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
from sys import stdout # Quick and dirty hack for getting 'thread safe' prints.
from time import sleep
from re import findall
import socket

safe_print = lambda x: stdout.write("%s\n" % x) # Again for the above mentioned hack.

class Bot:
  def __init__(self, port, botName):
    host = 'localhost'
    self.tcpSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Connect to the game. Proceed by attempts since the server may not be ready.
    attemptsLeft = 10
    while attemptsLeft > 0:
      try:
        self.tcpSocket.connect((host, port))
      except Exception as exception: # If the connection has failed, the server is probably not ready yet...
        if attemptsLeft < 10: # Print the error only from the second time since the first try is usually a fail.
          safe_print('Warning : Connection to {}:{} failed (attempt {} over 10) : {}'.format(host, port, 10 - attemptsLeft + 1, exception))
        attemptsLeft = attemptsLeft - 1 # ...so we prepare another try (up to 10)...
        sleep(0.1) # ...after waiting a little.
      else: # The connection succeeded.
        safe_print('Bot {} connected to the game'.format(botName))
        attemptsLeft = 0
        self._communicate(botName) # Communicate the name.

  def __del__(self):
    self.tcpSocket.close()

  def _communicate(self, data):
    # Send and receive data.
    data = data.encode()
    self.tcpSocket.send(data)
    data = self.tcpSocket.recv(100) # Wait for response (or simply validation).
    
    # Read health, bullets left and number of kills.
    data = data.decode()
    dataTuple = tuple(int(n) for n in findall("[0-9]+", data))
    self.health = dataTuple[0]
    self.bullets = dataTuple[1]
    self.kills = dataTuple[2]
    return dataTuple

  def skip(self):
    # Format : "0".
    data = '0'
    self._communicate(data)

  def move(self):
    # Format : "1".
    data = '1'
    self._communicate(data)

  def rotate(self, degrees):
    # Format : "2 numDegrees" where 'numDegrees' is a signed integer.
    data = '2 {}'.format(int(degrees))
    self._communicate(data)

  # Return the number of bullets left.
  def fire(self):
    # Format : "3".
    data = '3'
    self._communicate(data)

  # Do not scan the bot itself nor its bullets nor dead bots. 'scanCircles' includes world's edges.
  def scan(self, distance, radius, scanCircles, scanBots, scanBullets):
    # Format : "4 distance radius objectScanned" where 'distance' and 'radius' are integers, 'objectScanned' bits integer.
    objectScanned = 0
    if scanCircles:
      objectScanned |= 1
    if scanBots:
      objectScanned |= 2
    if scanBullets:
      objectScanned |= 4
    data = '4 {} {} {}'.format(int(distance), int(radius), objectScanned)
    data = self._communicate(data)
    return tuple(data[n] for n in range(3, 6))

