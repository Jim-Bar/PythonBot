/*
 * PythonBot - A game by a developer for developers.
 * Copyright (C) 2015 Jean-Marie BARAN (jeanmarie.baran@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Refer to 'LICENSE.txt' for the full notice.
 */

/*
 * Standalone or online version.
 * 
 * 'LocalView' is the regular version, in standalone mode. It will open a window on the system, and draw the game.
 * 'RemoteView' is the online version. It will open a socket to a client, and send the graphical data to be drawn.
 *
 * If an extra port number is given, the online version will start. Otherwise the standalone version will be used.
 */

#include <cstdlib>
#include <iostream>
#
#include "Model.h"      // M
#include "LocalView.h"  // V
#include "RemoteView.h" // V
#include "Controller.h" // C

bool read_arguments(int argc, char *argv[], int& portBots, int& numBots, int& portRemoteView);
void create_game(int portBots, int numBots, int portRemoteView);

int main(int argc, char *argv[])
{

  int portBots(0), numBots(0), portRemoteView(0);
  if (!read_arguments(argc, argv, portBots, numBots, portRemoteView))
    return EXIT_FAILURE;
  
  create_game(portBots, numBots, portRemoteView);
  
  return EXIT_SUCCESS;
}

bool read_arguments(int argc, char *argv[], int& portBots, int& numBots, int& portRemoteView)
{
  // Check the number of arguments.
  if (argc != 3 && argc != 4)
  {
    std::cerr << "Usage : " << argv[0] << " PORT_BOTS NUMBER_OF_BOTS [PORT_REMOTE_VIEW]" << std::endl;
    return false;
  }
  
  // Read values.
  portBots = atoi(argv[1]);
  numBots = atoi(argv[2]);
  if (argc == 4)
    portRemoteView = atoi(argv[3]);
  else
    portRemoteView = 0;
  
  // Check the validity of the arguments. If 'portRemoteView' is 0, the standalone version will be used.
  if (portBots <= 1024 || numBots <= 0 || portRemoteView < 0 || (portRemoteView > 0 && portRemoteView <= 1024))
  {
    std::cerr << "Invalid argument" << std::endl;
    return false;
  }
  
  return true;
}

void create_game(int portBots, int numBots, int portRemoteView)
{
  if (portRemoteView == 0 && false) // Local view.
  {
    Model model(portBots, numBots);
    LocalView view(model);
    Controller controller(model, view);
    
    controller.loop();
  }
  else // Remote view.
  {
    Model model(portBots, numBots);
    RemoteView view(model);
    Controller controller(model, view);
    
    controller.loop();
  }
}