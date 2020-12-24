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

#include <cmath>
#
#include "Circle.h"

Circle::Circle(unsigned int x, unsigned int y, unsigned int radius,
               sf::Color color) {
  m_SFMLShape.setOrigin((float)radius, (float)radius);
  m_SFMLShape.setRadius((float)radius);
  m_SFMLShape.setPosition((float)x, (float)y);
  m_SFMLShape.setFillColor(color);
}

float Circle::get_radius() const { return m_SFMLShape.getRadius(); }

sf::Vector2f const &Circle::get_position() const {
  return m_SFMLShape.getPosition();
}

sf::Shape const &Circle::get_SFML_shape() const { return m_SFMLShape; }
