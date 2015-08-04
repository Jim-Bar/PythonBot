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

#include <cstdlib>
#include <iostream>
#
#include "Model.h"      // M
#include "View.h"       // V
#include "Controller.h" // C

int main(int argc, char *argv[])
{
  // Manage arguments.
  if (argc != 3)
  {
    std::cerr << "Usage : " << argv[0] << " [port] [number of bots]" << std::endl;
    return EXIT_FAILURE;
  }
  int port(atoi(argv[1]));
  int numBots(atoi(argv[2]));
  if (port <= 0 || numBots <= 0)
  {
    std::cerr << "Invalid argument" << std::endl;
    return EXIT_FAILURE;
  }
  
  // Create game.
  Model model(port, numBots);
  View view(model);
  Controller controller(model, view);
  
  // Launch the game.
  controller.loop();
  
  return EXIT_SUCCESS;
}
