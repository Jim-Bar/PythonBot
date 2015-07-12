#include <cmath>
#
#include "Object.h"

Object::~Object()
{}

float
Object::distance(Object *object) const
{
  unsigned int deltaX(get_position().x - object->get_position().x), deltaY(get_position().y - object->get_position().y);
  
  return sqrt(deltaX * deltaX + deltaY * deltaY);
}
