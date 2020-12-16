# PythonBot - A game by a developer for developers.
# Copyright (C) 2015-2021 Jean-Marie BARAN (jeanmarie.baran@gmail.com)

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


import re
import socket
import sys
import time


# Quick and dirty hack for getting 'thread safe' prints.
def safe_print(message):
    sys.stdout.write('{}\n'.format(message))


class Bot:
    def __init__(self, port, bot_name):
        host = 'localhost'
        self.tcpSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Connect to the game. Proceed by attempts since the server may not be ready.
        attempts_left = 10
        while attempts_left > 0:
            try:
                self.tcpSocket.connect((host, port))
            except Exception as exception:  # If the connection has failed, the server is probably not ready yet...
                # Print the error only after the second time since the first try is usually a fail.
                if attempts_left < 10:
                    safe_print('Warning: Connection to {}:{} failed (attempt {} over 10): {}'
                               .format(host, port, 10 - attempts_left + 1, exception))
                attempts_left = attempts_left - 1  # ...so we prepare another try (up to 10)...
                time.sleep(0.01)  # ...after waiting a little.
            else:  # The connection succeeded.
                safe_print('Bot {} connected to the game'.format(bot_name))
                attempts_left = 0
                self._communicate(bot_name)  # Communicate the name.

    def __del__(self):
        self.tcpSocket.close()

    def _communicate(self, data):
        # Send and receive data.
        data = data.encode()
        self.tcpSocket.send(data)
        data = self.tcpSocket.recv(100)  # Wait for response (or simply validation).

        # Read health, bullets left and number of kills.
        data = data.decode()
        data_tuple = tuple(int(n) for n in re.findall("[0-9]+", data))
        self.health = data_tuple[0]
        self.bullets = data_tuple[1]
        self.kills = data_tuple[2]
        return data_tuple

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
    def scan(self, distance, radius, scan_circles, scan_bots, scan_bullets):
        # Format : "4 distance radius object_scanned" where 'distance' and 'radius' are integers, 'object_scanned' bits
        # integer.
        object_scanned = 0
        if scan_circles:
            object_scanned |= 1
        if scan_bots:
            object_scanned |= 2
        if scan_bullets:
            object_scanned |= 4
        data = '4 {} {} {}'.format(int(distance), int(radius), object_scanned)
        data = self._communicate(data)
        return tuple(data[n] for n in range(3, 6))
