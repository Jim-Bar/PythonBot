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

#ifndef SCAN__HEADER
#define SCAN__HEADER

#include "Circle.h"

class Scan : public Circle
{
public:
  /* Constructor. */
  Scan(sf::Color color);
  
  /* Getters. */
  bool get_active() const;
  
  /* Setters. */
  void set_color(sf::Color color);
  void set_position(sf::Vector2f const& position);
  void set_radius(unsigned int radius);
  void set_active();

private:
  bool m_active; // The bot has scanned at least once : true. Otherwise : false.
};

#endif
