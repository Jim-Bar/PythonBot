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

#include "Scan.h"

Scan::Scan(sf::Color color) : Circle(0, 0, 0, sf::Color::Transparent), m_active(false)
{
  m_SFMLShape.setOutlineThickness(-1.0f);
  m_SFMLShape.setOutlineColor(color);
}

bool
Scan::get_active() const
{
  return m_active;
}

void
Scan::set_color(sf::Color color)
{
  m_SFMLShape.setOutlineColor(color);
}

void
Scan::set_position(sf::Vector2f const& position)
{
  m_SFMLShape.setPosition(position);
}

void
Scan::set_radius(unsigned int radius)
{
  m_SFMLShape.setOrigin((float) radius, (float) radius);
  m_SFMLShape.setRadius((float) radius);
}

void
Scan::set_active()
{
  m_active = true;
}
