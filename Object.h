#ifndef OBJECT__HEADER
#define OBJECT__HEADER

#include <SFML/Graphics.hpp>

class Object
{
public:
  virtual ~Object();
  virtual sf::Vector2f const& get_position() const = 0;
  virtual sf::Shape const& get_SFML_shape() const = 0;
  
  /* Return the euclidean distance to another object (from centroid to centroid). */
  float distance(Object *object) const;
};

#endif
