#ifndef CONTROLLER__HEADER
#define CONTROLLER__HEADER

#include "Model.h"
#include "View.h"

class Controller
{
public:
  /* Constructor. */
  Controller(Model& model, View& view);
  
  /* Game loop. */
  void loop();

private:
  Model& m_model;
  View& m_view;

  /* Manage bullets' movements, collisions... */
  void update_bullets();
  
  /* Communicate with the bots, move them... */
  void update_bots();
};

#endif
