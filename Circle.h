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

#ifndef CIRCLE__HEADER
#define CIRCLE__HEADER

#include <SFML/Graphics.hpp>
#
#include "Object.h"

class Circle : public Object {
public:
  /* Constructor. */
  Circle(unsigned int x, unsigned int y, unsigned int radius,
         sf::Color color = sf::Color(128, 128, 128));

  /* Getters. */
  float get_radius() const;
  sf::Vector2f const &get_position() const;
  sf::Shape const &get_SFML_shape() const;

protected:
  sf::CircleShape m_SFMLShape;
};

#endif
