#include "Scan.h"

Scan::Scan(sf::Color color) : Circle(0, 0, 0, sf::Color::Transparent)
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
