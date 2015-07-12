#ifndef CIRCLE__HEADER
#define CIRCLE__HEADER

#include <SFML/Graphics.hpp>
#
#include "Object.h"

class Circle : public Object
{
public:
  /* Constructor. */
  Circle(unsigned int x, unsigned int y, unsigned int radius, sf::Color color = sf::Color(128, 128, 128));
  
  /* Getters. */
  float get_radius() const;
  sf::Vector2f const& get_position() const;
  sf::Shape const& get_SFML_shape() const;

protected:
  sf::CircleShape m_SFMLShape;
};

#endif
