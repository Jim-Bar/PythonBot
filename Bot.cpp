/*
 * PythonBot - A game by a developer for developers.
 * Copyright (C) 2015-2021 Jean-Marie BARAN (jeanmarie.baran@gmail.com)
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

#include <cmath>
#
#include "Bot.h"

float const Bot::botLength;
float const Bot::botWidth = 2.0f * botLength / 3.5f;
float const Bot::botSpeed;

/* Initial health, number of bullets left, and number of kills are hard-coded
 * here and in the constructor of BotConnector too. */
Bot::Bot(sf::TcpListener &tcpListener, sf::Color color)
    : m_health(10), m_bulletsLeft(10), m_numKills(0), m_scan(color) {
  // Connect to the bot.
  m_connector.connect_bot(tcpListener, m_name);

  // Define shape.
  m_SFMLShape.setPointCount(3);
  m_SFMLShape.setPoint(0, sf::Vector2f(botWidth / 2.0f, 0.0f));
  m_SFMLShape.setPoint(1, sf::Vector2f(0.0f, botLength));
  m_SFMLShape.setPoint(2, sf::Vector2f(botWidth, botLength));

  // Define outline.
  m_SFMLShape.setOutlineThickness(-1);
  m_SFMLShape.setOutlineColor(color);

  // Define common properties.
  m_SFMLShape.setOrigin(botWidth / 2.0f, 2.0f / 3.0f * botLength);
  m_SFMLShape.setPosition(0.0f, 0.0f);
  m_SFMLShape.setFillColor(sf::Color::Transparent);
}

sf::Vector2f const &Bot::get_position() const {
  return m_SFMLShape.getPosition();
}

sf::Vector2f const &Bot::get_v1() const { return m_v1; }

sf::Vector2f const &Bot::get_v2() const { return m_v2; }

sf::Vector2f const &Bot::get_v3() const { return m_v3; }

unsigned int Bot::get_health() const { return m_health; }

unsigned int Bot::get_bullets_left() const { return m_bulletsLeft; }

unsigned int Bot::get_num_kills() const { return m_numKills; }

std::string const &Bot::get_name() const { return m_name; }

sf::Vector2f const &Bot::get_direction() const { return m_direction; }

sf::Shape const &Bot::get_SFML_shape() const {
  if (is_alive())
    return m_SFMLShape;
  else
    return m_SFMLDeadShape;
}

Scan &Bot::get_scan() { return m_scan; }

BotConnector &Bot::get_connector() { return m_connector; }

void Bot::set_color(sf::Color color) {
  m_SFMLShape.setOutlineColor(color);
  m_scan.set_color(color);
}

void Bot::set_position(float x, float y) {
  m_SFMLShape.setPosition(x, y);
  update_vertices();
}

void Bot::set_scan(int distance, unsigned int radius) {
  m_scan.set_position(get_position() + ((float)distance) * get_direction());
  m_scan.set_radius((float)radius);
}

void Bot::fire_bullet() {
  if (m_bulletsLeft > 0) {
    if (m_bulletsLeft == 10) {
      // To avoid to shoot 11 times when it was long since the bot fired.
      m_clockLastBulletLoaded.restart();
    }
    m_bulletsLeft--;
  }
}

void Bot::load_bullet() {
  if (m_bulletsLeft < 10) {
    sf::Time elapsedTime(m_clockLastBulletLoaded.getElapsedTime());
    if (m_clockLastBulletLoaded.getElapsedTime().asMilliseconds() > 500) {
      m_clockLastBulletLoaded.restart();
      m_bulletsLeft++;
    }
  }
}

void Bot::add_kill() { m_numKills++; }

bool Bot::is_alive() const { return m_health > 0; }

void Bot::move() {
  m_SFMLShape.move(botSpeed * get_direction().x, botSpeed * get_direction().y);
  update_vertices();
}

void Bot::rotate(int degrees) {
  m_SFMLShape.rotate(degrees);
  update_vertices();
  update_direction();
}

void Bot::receive_damages() {
  if (m_health > 0) {
    m_health--;
    if (m_health == 0) { // In the case of death, load dead shape.
      m_SFMLDeadShape = m_SFMLShape;
      m_SFMLDeadShape.setOutlineColor(sf::Color(64, 64, 64));
    }
  }
}

void Bot::update_vertices() {
  sf::Transform transform(get_SFML_shape().getTransform());
  m_v1 = transform.transformPoint(get_SFML_shape().getPoint(0));
  m_v2 = transform.transformPoint(get_SFML_shape().getPoint(1));
  m_v3 = transform.transformPoint(get_SFML_shape().getPoint(2));
}

void Bot::update_direction() {
  m_direction.x = sin(m_SFMLShape.getRotation() * M_PI / 180.0f);
  m_direction.y = -cos(m_SFMLShape.getRotation() * M_PI / 180.0f);
}
