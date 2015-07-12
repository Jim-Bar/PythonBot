#ifndef BOT__HEADER
#define BOT__HEADER

#include <SFML/Graphics.hpp>
#
#include "BotConnector.h"
#include "Scan.h"

class Bot : public Object
{
public:
  static float const botLength = 20.0f;
  static float const botWidth;
  
  /* Constructor. */
  Bot(sf::TcpListener& tcpListener, sf::Color color = sf::Color::White);
  
  /* Getters. */
  sf::Vector2f const& get_position() const;
  sf::Vector2f const& get_v1() const;
  sf::Vector2f const& get_v2() const;
  sf::Vector2f const& get_v3() const;
  unsigned int get_health() const;
  unsigned int get_bullets_left() const;
  unsigned int get_num_kills() const;
  std::string const& get_name() const;
  sf::Vector2f const& get_direction() const;
  sf::Shape const& get_SFML_shape() const;
  Scan& get_scan();
  BotConnector& get_connector();
  
  /* Setters. */
  void set_color(sf::Color color);
  void set_position(float x, float y);
  void set_scan(int distance, unsigned int radius);
  
  /* Decrease the number of bullets by one. */
  void fire_bullet();
  
  /* Increase the number of bullets by one (every half second). */
  void load_bullet();
  
  /* Increase the number of kills by one. */
  void add_kill();
  
  /* Return 'true' if the bot has remaining health points. */
  bool is_alive() const;
  
  /* Move the bot forward. */
  void move();
  
  /* Rotate the bot (offset in degrees). */
  void rotate(int degrees);
  
  /* Receive damages. */
  void receive_damages();

private:
  static float const botSpeed = 3.0f;
  
  sf::Vector2f m_v1;
  sf::Vector2f m_v2;
  sf::Vector2f m_v3;
  
  unsigned int m_health;
  unsigned int m_bulletsLeft;
  unsigned int m_numKills;
  std::string m_name;
  sf::Clock m_clockLastBulletLoaded; // Clock measuring time since last time a bullet was loaded.
  sf::Vector2f m_direction; // Directional vector (normalized).
  
  sf::ConvexShape m_SFMLShape;
  sf::ConvexShape m_SFMLDeadShape;
  
  Scan m_scan;
  BotConnector m_connector;
  
  /* Update the position of the vertices v1, v2, v3. Called when the bot moves or rotates. */
  void update_vertices();
  
  /* Update the direction. Called when the bot rotates. */
  void update_direction();
};

#endif
