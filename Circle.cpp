#include <cmath>
#
#include "Circle.h"

Circle::Circle(unsigned int x, unsigned int y, unsigned int radius, sf::Color color)
{
  m_SFMLShape.setOrigin((float) radius, (float) radius);
  m_SFMLShape.setRadius((float) radius);
  m_SFMLShape.setPosition((float) x, (float) y);
  m_SFMLShape.setFillColor(color);
}

float
Circle::get_radius() const
{
  return m_SFMLShape.getRadius();
}

sf::Vector2f const&
Circle::get_position() const
{
  return m_SFMLShape.getPosition();
}

sf::Shape const&
Circle::get_SFML_shape() const
{
  return m_SFMLShape;
}
