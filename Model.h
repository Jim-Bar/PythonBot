#ifndef MODEL__HEADER
#define MODEL__HEADER

#include <vector>
#
#include "Bot.h"
#include "Bullet.h"

class Model
{
public:
  /* Constructor. */
  Model(unsigned int port, unsigned int numBots, unsigned int width = 800, unsigned int height = 600); // This is not the resolution, but the arena dimensions.
  ~Model();
  
  /* Getters. */
  unsigned int get_width() const;
  unsigned int get_height() const;
  std::vector<Object*> const& get_circles() const;
  std::vector<Object*> const& get_bots() const;
  std::vector<Object*> const& get_dead_bots() const;
  std::vector<Object*> const& get_scans() const;
  std::vector<Object*> const& get_bullets() const;
  
  /* Return 'true' if a bot collides with another object, bullets excepted ('bot' is the bot of collision is any). */
  bool collides(Bot const *bot, Bot **botCollision, sf::Vector2f& hitPoint) const;
  
  /* Return 'true' if a bullet collides with an object of the world (circle or edge). */
  bool collides(Bullet const *bullet, sf::Vector2f& hitPoint) const;
  
  /* Return 'true' if a bullet collides with a bot ('bot' is the bot of collision is any). */
  bool collides(Bullet const *bullet, Bot **bot, sf::Vector2f& hitPoint) const;
  
  /* Create a bullet fired from 'bot'. */
  void fire_bullet(Bot *bot);
  
  /* Delete a bullet from the world. */
  void erase_bullet(unsigned int bulletId);
  
  /* Decrease health and move the bot to the deads list if it die. */
  void inflict_damages(Bot *bot);
  
  /* Scan the area. If an type of object is not asked to be scanned, put zero in the according variable. */
  void scan(Bot *bot, bool scanCircles, unsigned int& numCircles, bool scanBots, unsigned int& numBots, bool scanBullets, unsigned int& numBullets);
  
  /* Euclidean distance between to points. */
  float distance(sf::Vector2f const& p1, sf::Vector2f const& p2 = sf::Vector2f(0.0f, 0.0f)) const;

private:
  unsigned int const m_width;
  unsigned int const m_height;
  
  std::vector<Object*> m_circles;
  std::vector<Object*> m_bots;
  std::vector<Object*> m_deadBots;
  std::vector<Object*> m_scans;
  std::vector<Object*> m_bullets;
  
  sf::TcpListener m_tcpListener;
  
  /* Add a bot to the game. */
  void add_bot();
  
  /* Check that a bot is within the world. */
  bool is_in_world(Bot const *bot, sf::Vector2f& hitPoint) const;
  
  /* Check that a circle is within the world. */
  bool is_in_world(Circle const *circle, sf::Vector2f& hitPoint) const;
  
  /* Refined version of collision detection, for bots and circles. To call when a collision is most likely to happen. */
  bool collides(Bot const *bot, Circle const *circle, sf::Vector2f& hitPoint) const;
  
  /* Refined version of collision detection, for circles. */
  bool collides(Circle const *circle1, Circle const *circle2, sf::Vector2f& hitPoint) const;
  
  /* Refined version of collision detection, for bots. To call when a collision is most likely to happen. */
  bool collides(Bot const *bot1, Bot const *bot2, sf::Vector2f& hitPoint) const;
  
  /* Test intersection of a segment and a circle. 'P' is the point of intersection if any. */
  bool segment_circle_intersection(sf::Vector2f const& A, sf::Vector2f const& B, Circle const *circle, sf::Vector2f& P) const;
  
  /* Test intersection of two segments. 'P' is the point of intersection found if any. */
  bool segments_intersection(sf::Vector2f const& A, sf::Vector2f const& B, sf::Vector2f const& C, sf::Vector2f const& D, sf::Vector2f& P) const;
  
  /* Return a random value between 0 and 'bound' - 1. Uniform distribution. */
  unsigned int random_value(unsigned int bound) const;
};

#endif
