#include "Bullet.h"

float const Bullet::bulletSpeed;

Bullet::Bullet(Bot *botOrigin, unsigned int radius, sf::Color color) : Circle(0, 0, radius, sf::Color::Transparent), m_direction(botOrigin->get_direction()), m_botOrigin(botOrigin) 
{
  m_SFMLShape.setPosition(botOrigin->get_v1() + (2.0f + radius / 2.0f) * botOrigin->get_direction()); // +2.0f : to avoid self intersection with its own bot.
  m_SFMLShape.setOutlineThickness(-1.5f);
  m_SFMLShape.setOutlineColor(color);
}

Bot*
Bullet::get_bot_origin() const
{
  return m_botOrigin;
}

void
Bullet::move()
{
  m_SFMLShape.move(bulletSpeed * m_direction);
}
