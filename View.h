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

#ifndef VIEW__HEADER
#define VIEW__HEADER

#include <SFML/Graphics.hpp>
#include <vector>
#
#include "Model.h"

class View {
public:
  enum EventRequest { REQUEST_NONE, REQUEST_PAUSE, REQUEST_QUIT };

  /* Deal with events. */
  virtual EventRequest read_events() = 0;

  /* Set a list of objects to be drawn. */
  virtual void draw(std::vector<Object *> const &objects) = 0;

  /* Display the scene. */
  virtual void render() = 0;

  /* Add a little explosion to the scene. */
  virtual void add_explosion(sf::Vector2f const &position,
                             float radius = 1.0f) = 0;

  /* Add a bot explosion to the scene. */
  virtual void add_bot_explosion(sf::Vector2f const &position,
                                 float radius = 5.0f) = 0;

  /* Draw "Pause" on top of the game. */
  virtual void draw_pause() = 0;
};

#endif
