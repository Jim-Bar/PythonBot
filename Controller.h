/*
 * PythonBot - A game by a developer for developers.
 * Copyright (C) 2015-2021 Jean-Marie BARAN (jeanmarie.baran@gmail.com)
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

#ifndef CONTROLLER__HEADER
#define CONTROLLER__HEADER

#include "Model.h"
#include "View.h"

class Controller {
public:
  /* Constructor. */
  Controller(Model &model, View &view, bool startPaused = true);

  /* Game loop. */
  void loop();

private:
  bool m_startPaused;

  Model &m_model;
  View &m_view;

  /* Manage bullets' movements, collisions... */
  void update_bullets();

  /* Communicate with the bots, move them... */
  void update_bots();
};

#endif
