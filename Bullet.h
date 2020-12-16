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

#ifndef BULLET__HEADER
#define BULLET__HEADER

#include "Bot.h"
#include "Circle.h"

class Bullet : public Circle
{
public:
  /* Constructor. */
  Bullet(Bot *botOrigin, unsigned int radius = 3.0f, sf::Color color = sf::Color::White);

  /* Getters. */
  Bot* get_bot_origin() const;
  
  /* Move the bullet forward. */
  void move();

private:
  static float const bulletSpeed = 9.0f;
  
  sf::Vector2f m_direction; // Directional vector (normalized).
  Bot *m_botOrigin; // The bot which fired the bullet.
};

#endif
