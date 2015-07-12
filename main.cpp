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
