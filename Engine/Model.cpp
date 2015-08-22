/*
 * PythonBot - A game by a developer for developers.
 * Copyright (C) 2015 Jean-Marie BARAN (jeanmarie.baran@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Refer to 'LICENSE.txt' for the full notice.
 */

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <algorithm>
#
#include "Model.h"

// Simple function to compare bots by name.
bool compare_bots(Object const *bot1, Object const *bot2)
{
  return ((Bot const*) bot1)->get_name() < ((Bot const*) bot2)->get_name();
}

Model::Model(unsigned int numBots, unsigned int botPort, unsigned int contactPort, unsigned int width, unsigned int height) : m_width(width), m_height(height)
{
  // Initialize seed for random numbers.
  srand(time(0));
  
  // Create random circles.
  unsigned int const radiusMin(10), radiusMax(60);
  unsigned int const numCircles(10 * width * height / 480000);
  for (unsigned int i(0); i < numCircles; i++)
  {
    unsigned int radius(random_value(radiusMax - radiusMin + 1) + radiusMin);
    unsigned int x(random_value(width));
    unsigned int y(random_value(height));
    m_circles.push_back(new Circle(x, y, radius));
  }
  
  // Initialize a listener for the incoming connections.
  sf::TcpListener tcpListener;
  if (tcpListener.listen(botPort) != sf::Socket::Done)
    std::cerr << "Error : Unable to bind the listener to port " << botPort << std::endl;
  
  // Send the bot socket port to the Python module...
  if (contactPort > 0) // ...if a port for this has been provided.
  {
    sf::sleep(sf::milliseconds(500));
    sf::TcpSocket contactSocket;
    if (contactSocket.connect(sf::IpAddress::LocalHost, contactPort) == sf::Socket::Done) // Connect to the port provided.
    {
      char data[6] = {0}; // Size is 6 because the maximum value is 65535 (5 digits plus '\0').
      int numCharacters(sprintf(data, "%u", tcpListener.getLocalPort())); // Do not use 'botPort' here as if it is zero (and it should be the case), a random port has been chosen.

      // Send the port the server will listen on.
      if (numCharacters > 0)
      {
	// Partial sends warning have been introduced in SFML 2.3. Up to 2.2, no need for using a third parameter for 'send()'.
	#if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR <= 2
	
	if (contactSocket.send(data, numCharacters) != sf::Socket::Done)
	  std::cerr << "Error : Sending bot socket port to the Python module failed" << std::endl;
	
	#else
	
	size_t sent(0);
	if (contactSocket.send(data, numCharacters, sent) != sf::Socket::Done) // We use 'sent' to get rid of the SFML warning "Partial sends not handled properly".
	  std::cerr << "Error : Sending bot socket port to the Python module failed" << std::endl;
	
	#endif
      }
      else
	std::cerr << "Error : Unable to write the bot socket port to a buffer" << std::endl;
      
      contactSocket.disconnect();
    }
    else
      std::cerr << "Error : Could not connect to the Python module to the specified port (" << contactPort << ")" << std::endl;
  }
  
  // Add bots entities.
  std::cout << "Waiting for " << numBots << " bots..." << std::endl;
  for (unsigned int i(0); i < numBots; i++)
    add_bot(tcpListener);
  
  // Sort the bots by name and give them their proper color.
  std::sort(m_aliveBots.begin(), m_aliveBots.end(), compare_bots);
  sf::Color colors[] = {sf::Color::Red, sf::Color::Green, sf::Color::Blue, sf::Color::Yellow,
                        sf::Color::Magenta, sf::Color::Cyan, sf::Color(255, 128, 0), sf::Color(128, 0, 255)}; // ... Orange, Purple.
  for (unsigned int i(0); i < numBots; i++)
    ((Bot*) m_aliveBots[i])->set_color(colors[i % 8]);
  
  // Close the listener (not needed anymore).
  tcpListener.close();
  
  // Finally, fill the vector of all bots.
  m_bots = m_aliveBots;
}

Model::~Model()
{
  // Free circles.
  while (!m_circles.empty())
  {
    delete m_circles.back();
    m_circles.pop_back();
  }
  
  // Free bots.
  while (!m_aliveBots.empty())
  {
    delete m_aliveBots.back();
    m_aliveBots.pop_back();
  }
  while (!m_deadBots.empty())
  {
    delete m_deadBots.back();
    m_deadBots.pop_back();
  }
  
  // Free bullets.
  while (!m_bullets.empty())
  {
    delete m_bullets.back();
    m_bullets.pop_back();
  }
}

unsigned int
Model::get_width() const
{
  return m_width;
}

unsigned int
Model::get_height() const
{
  return m_height;
}

std::vector<Object*> const&
Model::get_circles() const
{
  return m_circles;
}

std::vector<Object*> const&
Model::get_alive_bots() const
{
  return m_aliveBots;
}

std::vector<Object*> const&
Model::get_dead_bots() const
{
  return m_deadBots;
}

std::vector<Object*> const&
Model::get_scans() const
{
  return m_scans;
}

std::vector<Object*> const&
Model::get_bullets() const
{
  return m_bullets;
}

std::vector<Object*> const&
Model::get_bots() const
{
  return m_bots;
}

bool
Model::collides(Bot const *bot, Bot **botCollision, sf::Vector2f& hitPoint) const
{
  // World's edge collision detection.
  if (!is_in_world(bot, hitPoint))
    return true;
  
  // Loop over all the circles.
  for (unsigned int i(0); i < m_circles.size(); i++)
    if (bot->distance(m_circles[i]) <= 2.0f / 3.0f * Bot::botLength + ((Circle*) m_circles[i])->get_radius())
      if (collides(bot, ((Circle*) m_circles[i]), hitPoint))
        return true;
  
  // Loop over all the bots.
  *botCollision = 0;
  for (unsigned int i(0); i < m_aliveBots.size(); i++)
    if (bot != m_aliveBots[i] && bot->distance(m_aliveBots[i]) <= 4.0f / 3.0f * Bot::botLength)
      if (collides(bot, ((Bot*) m_aliveBots[i]), hitPoint))
      {
	*botCollision = (Bot*) m_aliveBots[i];
        return true;
      }
  
  return false;
}

bool
Model::collides(Bullet const *bullet, sf::Vector2f& hitPoint) const
{
  // Check that the bullet is within world.
  if (!is_in_world(bullet, hitPoint))
    return true;

  // Check collision with circles.
  for (unsigned int i(0); i < m_circles.size(); i++)
    if (collides(bullet, (Circle*) m_circles[i], hitPoint))
      return true;

  return false;
}

bool
Model::collides(Bullet const *bullet, Bot **bot, sf::Vector2f& hitPoint) const
{
  // Loop over all the bots.
  for (unsigned int i(0); i < m_aliveBots.size(); i++)
    if (distance(bullet->get_position(), m_aliveBots[i]->get_position()) <= bullet->get_radius() + 2.0f / 3.0f * Bot::botLength)
      if (collides((Bot*) m_aliveBots[i], bullet, hitPoint))
      {
        *bot = (Bot*) m_aliveBots[i];
        return true;
      }

  return false;
}

void
Model::fire_bullet(Bot *bot)
{
  bot->fire_bullet();
  m_bullets.push_back(new Bullet(bot));
}

void
Model::erase_bullet(unsigned int bulletId)
{
  delete m_bullets[bulletId];
  m_bullets.erase(m_bullets.begin() + bulletId);
}

void
Model::inflict_damages(Bot *bot)
{
  bot->receive_damages();
  if (!bot->is_alive()) // Move the bot to the dead list if it is dead.
  {
    m_deadBots.push_back(bot);
    for (unsigned int i(0); i < m_aliveBots.size(); i++)
      if (bot == m_aliveBots[i])
        m_aliveBots.erase(m_aliveBots.begin() + i);
    for (unsigned int i(0); i < m_scans.size(); i++) // Remove the scan from the list.
      if (&bot->get_scan() == m_scans[i])
        m_scans.erase(m_scans.begin() + i);
  }
}

void
Model::scan(Bot *bot, bool scanCircles, unsigned int& numCircles, bool scanBots, unsigned int& numBots, bool scanBullets, unsigned int& numBullets)
{
  sf::Vector2f hitPoint;
  numCircles = 0;
  numBots = 0;
  numBullets = 0;
  
  // Scan circles and world's edges.
  if (scanCircles)
  {
    if (!is_in_world(&bot->get_scan(), hitPoint))
      numCircles++; // World's edges count for one.
    
    // Loop over all the circles.
    for (unsigned int i(0); i < m_circles.size(); i++)
      if (collides(&bot->get_scan(), (Circle const*) m_circles[i], hitPoint))
        numCircles++;
  }
  
  // Scan bots.
  if (scanBots)
    for (unsigned int i(0); i < m_aliveBots.size(); i++)
      if (bot != m_aliveBots[i] && collides((Bot const*) m_aliveBots[i], &bot->get_scan(), hitPoint))
        numBots++;
  
  // Scan bullets.
  if (scanBullets)
    for (unsigned int i(0); i < m_bullets.size(); i++)
      if (bot != ((Bullet*) m_bullets[i])->get_bot_origin() && collides(&bot->get_scan(), (Bullet const*) m_bullets[i], hitPoint))
        numBullets++;

  // Collect pointer to the newly activated scan.
  if (!bot->get_scan().get_active())
  {
    bot->get_scan().set_active();
    m_scans.push_back(&bot->get_scan());
  }
}

float 
Model::distance(sf::Vector2f const& p1, sf::Vector2f const& p2) const
{
  return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

void
Model::add_bot(sf::TcpListener& tcpListener)
{
  sf::Vector2f hitPoint;
  Bot *bot(new Bot(tcpListener)), *botBuffer(0); // 'botBuffer' not used, just to be able to call collides().
  do // Choose a free random position and orientation.
  {
    bot->set_position((float) random_value(m_width), (float) random_value(m_height));
    bot->rotate(random_value(360));
  } while (collides(bot, &botBuffer, hitPoint));  
  
  m_aliveBots.push_back(bot);
}

bool
Model::is_in_world(Bot const *bot, sf::Vector2f& hitPoint) const
{
  hitPoint = bot->get_position(); // To improve (think to transform get_vx() in get_vertices()...).
  if (bot->get_v1().x < 0.0f || bot->get_v1().x > m_width || bot->get_v1().y < 0.0f || bot->get_v1().y > m_height)
    return false;
  if (bot->get_v2().x < 0.0f || bot->get_v2().x > m_width || bot->get_v2().y < 0.0f || bot->get_v2().y > m_height)
    return false;
  if (bot->get_v3().x < 0.0f || bot->get_v3().x > m_width || bot->get_v3().y < 0.0f || bot->get_v3().y > m_height)
    return false;
  
  return true;
}

bool
Model::is_in_world(Circle const *circle, sf::Vector2f& hitPoint) const
{
  hitPoint = circle->get_position();
  if (circle->get_position().x - circle->get_radius() < 0.0f)
  {
    hitPoint.x = 0.0f;
    return false;
  }
  if (circle->get_position().y - circle->get_radius() < 0.0f)
  {
    hitPoint.y = 0.0f;
    return false;
  }
  if (circle->get_position().x + circle->get_radius() > m_width)
  {
    hitPoint.x = m_width - 1.0f;
    return false;
  }
  if (circle->get_position().y + circle->get_radius() > m_height)
  {
    hitPoint.y = m_height - 1.0f;
    return false;
  }
  
  return true;
}

bool
Model::collides(Bot const *bot, Circle const *circle, sf::Vector2f& hitPoint) const
{
  if (segment_circle_intersection(bot->get_v1(), bot->get_v2(), circle, hitPoint))
    return true;
  if (segment_circle_intersection(bot->get_v2(), bot->get_v3(), circle, hitPoint))
    return true;
  if (segment_circle_intersection(bot->get_v3(), bot->get_v1(), circle, hitPoint))
    return true;

  return false;
}

bool
Model::collides(Circle const *circle1, Circle const *circle2, sf::Vector2f& hitPoint) const
{
  if (distance(circle1->get_position(), circle2->get_position()) < circle1->get_radius() + circle2->get_radius())
  {
    // Compute hit point position.
    hitPoint = (circle2->get_position() - circle2->get_position()) / distance(hitPoint);
    hitPoint *= circle1->get_radius();
    hitPoint += circle1->get_position();
    return true;
  }
  
  return false;
}

#define SEG_1A bot1->get_v1(), bot1->get_v2()
#define SEG_1B bot1->get_v2(), bot1->get_v3()
#define SEG_1C bot1->get_v3(), bot1->get_v1()
#define SEG_2A bot2->get_v1(), bot2->get_v2()
#define SEG_2B bot2->get_v2(), bot2->get_v3()
#define SEG_2C bot2->get_v3(), bot2->get_v1()

bool
Model::collides(Bot const *bot1, Bot const *bot2, sf::Vector2f& hitPoint) const
{
  if (segments_intersection(SEG_1A, SEG_2A, hitPoint))
    return true;
  if (segments_intersection(SEG_1A, SEG_2B, hitPoint))
    return true;
  if (segments_intersection(SEG_1A, SEG_2C, hitPoint))
    return true;
  
  if (segments_intersection(SEG_1B, SEG_2A, hitPoint))
    return true;
  if (segments_intersection(SEG_1B, SEG_2B, hitPoint))
    return true;
  if (segments_intersection(SEG_1B, SEG_2C, hitPoint))
    return true;
  
  if (segments_intersection(SEG_1C, SEG_2A, hitPoint))
    return true;
  if (segments_intersection(SEG_1C, SEG_2B, hitPoint))
    return true;
  if (segments_intersection(SEG_1C, SEG_2C, hitPoint))
    return true;
  
  return false;
}

bool
Model::segment_circle_intersection(sf::Vector2f const& A, sf::Vector2f const& B, Circle const *circle, sf::Vector2f& P) const
{
  // http://doswa.com/2009/07/13/circle-segment-intersectioncollision.html
  sf::Vector2f segment(B - A), AToCenter(circle->get_position() - A);
  float segmentLength(distance(segment)), projectionLength((AToCenter.x * segment.x + AToCenter.y * segment.y) / segmentLength);
  
  if (projectionLength < 0.0f)
    P = A;
  else if (projectionLength > segmentLength)
    P = B;
  else
    P = A + projectionLength * segment / segmentLength;
  
  return distance(circle->get_position(), P) < circle->get_radius();
}

bool
Model::segments_intersection(sf::Vector2f const& A, sf::Vector2f const& B, sf::Vector2f const& C, sf::Vector2f const& D, sf::Vector2f& P) const
{
  // http://www.faqs.org/faqs/graphics/algorithms-faq/
  // http://mathforum.org/dr.math/faq/formulas/faq.ag2.html
  float det((B.x - A.x) * (C.y - D.y) - (C.x - D.x) * (B.y - A.y));
  
  if (det == 0.0f)
    return false;
  
  float r(((B.x - A.x) * (C.y - A.y) - (C.x - A.x) * (B.y - A.y)) / det);
  float s(((C.x - A.x) * (C.y - D.y) - (C.x - D.x) * (C.y - A.y)) / det);
  
  // Point of intersection.
  P = A + r * (B - A);

  return 0 <= r && r <= 1 && 0 <= s && s <= 1;
}

unsigned int
Model::random_value(unsigned int bound) const
{
  unsigned int x(0);
  
  do // Evict unwanted values for an uniform distribution.
  {
    x = rand();
  } while (x >= RAND_MAX - RAND_MAX % bound);
  
  return x % bound;
}
