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
