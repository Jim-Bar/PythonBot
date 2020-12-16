#!/usr/bin/env python3

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


import Bot
import glob
import importlib
import os
import socket
import subprocess
import threading


# Load bots, launch the game and bots.
def launch_game():
    # Detect bots.
    bots = glob.glob(os.path.join('Bots', '*.py'))
    # Remove 'Bots/' and '.py' in the names, and __init__.py from the names.
    bots = [bot[5:-3] for bot in bots if bot != os.path.join('Bots', '__init__.py')]
    print('Bots detected: {}'.format(bots))

    # Import the bots and check that they compile.
    bot_modules = []
    # Create a __init__.py file in the Bots/ directory (to get import_module() function working).
    open(os.path.join('Bots', '__init__.py'), 'w').close()
    for bot in list(bots):  # Iterate over a copy to be able to remove elements during the iteration process.
        try:
            bot_module = importlib.import_module('Bots.' + bot)  # Import the bot python file.
        except Exception as exception:
            print('Error: Problem with bot {}: {}'.format(bot, exception))
            bots.remove(bot)
        else:
            bot_modules.append(bot_module)

    # Exchange ports, and launch the engine.
    # Prepare the socket which will receive the port on which the engine is listening for bots.
    tcp_listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    tcp_listener.bind(('localhost', 0))
    contact_port = tcp_listener.getsockname()[1]  # Get the port on which the engine will send its port (see above).
    # Prepare command to execute to start the engine.
    args = [os.path.join(os.getcwd(), 'pythonbot_core'), '-n', '{}'.format(len(bots)), '-c', '{}'.format(contact_port)]
    print('Lauching {}'.format(' '.join(args)))
    subprocess.Popen(args)  # Start the engine!

    # Receive the port to which bots will talk.
    tcp_listener.listen(1)  # Only one client will connect (the engine), no need for queueing.
    contact_socket = tcp_listener.accept()[0]  # Accept the connection.
    bot_port = int(contact_socket.recv(6).decode())  # Receive the port.
    contact_socket.close()
    print('Received port for bots: {}'.format(bot_port))

    # Launch the bots.
    threads = []
    for i in range(0, len(bots)):
        thread = threading.Thread(None, load_bot, 'pythonbot_bot_{}'.format(i), (bot_modules[i], bot_port, bots[i]))
        thread.start()
        threads.append(thread)

    # Wait for the bots to finish.
    for thread in threads:
        thread.join()
    print('All threads joined')


# Return a free port on localhost. Note that another process could bind a socket to that port after it is closed.
def get_free_port():
    tcp_socket = socket.socket()
    tcp_socket.bind(('localhost', 0))
    port = tcp_socket.getsockname()[1]
    tcp_socket.close()
    return port


# Load a bot from the python file in 'botName' and launch it.
def load_bot(bot_module, port, bot_name):
    name_max_length = 10
    if len(bot_name) > name_max_length:  # Cut the name if too long.
        Bot.safe_print('Warning: Bot name too long ({}) truncated to {}'.format(bot_name, bot_name[:name_max_length]))
        bot_name = bot_name[:name_max_length]
    bot = Bot.Bot(port, bot_name)  # Create the bot.
    try:
        bot_module.main(bot)  # Run the bot!
    except socket.error:  # The exception is raised when the socket is closed on server's side.
        Bot.safe_print('Bot {} shut down'.format(bot_name))
        del bot
    except Exception as exception:  # Unexpected error (usually function main missing).
        Bot.safe_print('Error: Problem with bot {} : {}'.format(bot_name, exception))
        del bot


if __name__ == '__main__':
    launch_game()
