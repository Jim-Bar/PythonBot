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
