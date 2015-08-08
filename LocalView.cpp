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
#include <sstream>
#include <algorithm>
#
#include "LocalView.h"

float const LocalView::edgesWidth;
float const LocalView::defaultMargin;

LocalView::LocalView(Model const& model, sf::Color edgesColor)
{
  // Retrieve a valid video mode.
  unsigned int const pannelWidth(400); // Bots info display zone.
  sf::VideoMode videoMode = sf::VideoMode::getDesktopMode();
  videoMode.width = model.get_width() + 2 * edgesWidth + pannelWidth + defaultMargin;
  videoMode.height = model.get_height() + 2 * edgesWidth;
  
  // Configure antialiasing.
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  
  // Set up the window.
  m_window.create(videoMode, "PythonBot", sf::Style::Default, settings);
  if (!m_window.isOpen())
    std::cerr << "Error : Window creation failed." << std::endl;
  m_window.setFramerateLimit(60);
  m_window.setKeyRepeatEnabled(false);
  
  // Configure world's edges.
  // Up.
  m_edges[0].setFillColor(edgesColor);
  m_edges[0].setSize(sf::Vector2f(model.get_width() + 2.0f * edgesWidth, edgesWidth));
  m_edges[0].setPosition(-edgesWidth, -edgesWidth);
  // Right.
  m_edges[1].setFillColor(edgesColor);
  m_edges[1].setSize(sf::Vector2f(edgesWidth, model.get_height() + 2.0f * edgesWidth));
  m_edges[1].setPosition((float) model.get_width(), -edgesWidth);
  // Bottom.
  m_edges[2].setFillColor(edgesColor);
  m_edges[2].setSize(sf::Vector2f(model.get_width() + 2.0f * edgesWidth, edgesWidth));
  m_edges[2].setPosition(-edgesWidth, (float) model.get_height());
  // Left.
  m_edges[3].setFillColor(edgesColor);
  m_edges[3].setSize(sf::Vector2f(edgesWidth, model.get_height() + 2.0f * edgesWidth));
  m_edges[3].setPosition(-edgesWidth, -edgesWidth);
  
  // Configure views.
  m_arena.setSize(m_window.getSize().x - (float) (pannelWidth) - defaultMargin, (float) m_window.getSize().y);
  m_arena.setCenter(m_arena.getSize() / 2.0f - sf::Vector2f(edgesWidth, edgesWidth));
  m_window.setView(m_arena);
  m_pannel.setSize(((float) pannelWidth), (float) m_window.getSize().y);
  m_pannel.setCenter(m_pannel.getSize() / 2.0f);
  resize(); // Initial resize useful on Windows where the content is not properly sized. A recalculation fix it.
  
  // Collect pointers to the bots.
  for (unsigned int i(0); i < model.get_bots().size(); i++)
    m_bots.push_back((Bot const*) model.get_bots()[i]);
  
  // Load font and initialize strings.
  const std::string fontName("arial.ttf");
  if (!m_font.loadFromFile(fontName))
    std::cerr << "Error : Unable to load " << fontName << std::endl;
  else
  {
    m_botName.setFont(m_font);
    m_numKills.setFont(m_font);
    m_pause.setFont(m_font);
    
    m_botName.setCharacterSize(15);
    m_numKills.setCharacterSize(15);
    m_pause.setCharacterSize(100);
    
    m_botName.setStyle(sf::Text::Bold);
    m_numKills.setStyle(sf::Text::Bold);
    m_pause.setStyle(sf::Text::Bold);
    
    m_pause.setColor(sf::Color::Red);
    m_pause.setString("Pause");
    m_pause.setPosition(m_arena.getCenter().x - m_pause.getLocalBounds().width / 2.0f, m_arena.getCenter().y - m_pause.getLocalBounds().height); // "Pause" fits better a little higher.
  }
}

LocalView::EventRequest
LocalView::read_events()
{
  sf::Event event;
  
  while (m_window.pollEvent(event))
  {
    if (event.type == sf::Event::Closed)
    {
      m_window.close();
      return REQUEST_QUIT;
    }
    else if (event.type == sf::Event::Resized)
      resize();
    else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
      return REQUEST_PAUSE;
  }
  
  return REQUEST_NONE;
}

void
LocalView::draw(std::vector<Object*> const& objects)
{
  // Add the objects to the scene.
  for (unsigned int i(0); i < objects.size(); i++)
    m_window.draw(objects[i]->get_SFML_shape());
}

void
LocalView::render()
{  
  // Draw explosions.
  draw_explosions();
  
  // Draw bots' information.
  draw_bots_info();
  
  m_window.display();
  m_window.clear(sf::Color::Black);
  
  // Draw world's edges.
  draw_edges();
}

void
LocalView::add_explosion(sf::Vector2f const& position, float radius)
{
  m_explosions.push_back(new sf::CircleShape(radius));
  m_explosions.back()->setOrigin(radius, radius);
  m_explosions.back()->setPosition(position);
  m_explosions.back()->setFillColor(sf::Color::Yellow);
  m_explosions.back()->setOutlineThickness(radius / 4.0f);
  m_explosions.back()->setOutlineColor(sf::Color::Red);
}

void
LocalView::add_bot_explosion(sf::Vector2f const& position, float radius)
{
  add_explosion(position, radius);
}

void
LocalView::resize()
{
  // Update the views to the new size of the window.
  // Arena.
  float const marginBorders((m_window.getSize().y - ((float) m_arena.getSize().y)) / 2.0f); // Margin to the window's borders.
  sf::FloatRect viewport;
  viewport.top = marginBorders / m_window.getSize().y;
  viewport.left = marginBorders / m_window.getSize().x;
  viewport.width = m_arena.getSize().x / m_window.getSize().x;
  viewport.height = m_arena.getSize().y / m_window.getSize().y;
  m_arena.setViewport(viewport);
  m_window.setView(m_arena);
  // Pannel.
  float const margin(marginBorders == 0.0f ? defaultMargin : 0.0f); // Margin between arena and pannel.
  m_pannel.setSize(m_window.getSize().x - m_arena.getSize().x - margin - 2.0f * marginBorders, m_pannel.getSize().y);
  viewport.left = (m_arena.getSize().x + 2.0f * marginBorders + margin) / m_window.getSize().x;
  viewport.width = m_pannel.getSize().x / m_window.getSize().x;
  m_pannel.setCenter(m_pannel.getSize() / 2.0f);
  m_pannel.setViewport(viewport);
}

void
LocalView::draw_pause()
{
  m_window.draw(m_pause);
}

void
LocalView::draw_edges()
{
  for (unsigned int i(0); i < 4; i++)
    m_window.draw(m_edges[i]);
}

void
LocalView::draw_explosions()
{
  for (std::vector<sf::CircleShape*>::iterator iter(m_explosions.begin()); iter != m_explosions.end(); /* Nothing (because of erasing elements). */)
  {
    // Draw then increase the radius. A standard explosion has a radius up to 4, a bot explosion from 5 to 8.
    m_window.draw(**iter);
    float radius((*iter)->getRadius());
    if (radius < 4.0f || (5.0f <= radius && radius < 8.0f))
    {
      (*iter)->setOrigin(radius + 0.4f, radius + 0.4f); // Reajust the origin.
      (*iter)->setRadius(radius + 0.4f);
      (*iter)->setOutlineThickness((*iter)->getRadius() / 4.0f);
      iter++;
    }
    else
    {
      delete *iter;
      iter = m_explosions.erase(iter);
    }
  }
}

void
LocalView::draw_bots_info()
{
  // Switch to pannel.
  m_window.setView(m_pannel);
  
  // Setup box.
  sf::RectangleShape box(sf::Vector2f(m_pannel.getSize().x / 2.0f - defaultMargin / 4.0f, 55.0f));
  box.setFillColor(sf::Color::Black);
  box.setOutlineThickness(-1.0f);
  
  // Setup health rectangle.
  float const healthBoxWidth((box.getSize().x - 10.0f - 1.0f) / 10.0f); // -10.0f, 10.0f : Maximum health of a bot (10).
  float const healthBoxHeight(std::min(healthBoxWidth, box.getSize().y / 3.0f));
  sf::RectangleShape healthBox(sf::Vector2f(healthBoxWidth, healthBoxHeight));
  
  // Setup bullet rectangle.
  float const bulletBoxHeight(box.getSize().y - healthBoxHeight - 25.0f);
  sf::RectangleShape bulletBox(sf::Vector2f(bulletBoxHeight / 3.0f, bulletBoxHeight));
  
  // Number of boxes vertically (maximum), and gap between them.
  unsigned int const numBoxVertically(10);
  float const gap((m_pannel.getSize().y - ((float) numBoxVertically) * box.getSize().y) / (((float) numBoxVertically) - 1.0f));
  
  float posX(0.0f);
  for (unsigned int i(0); i < m_bots.size(); i++)
  {
    // Fill the second column if the first one is full.
    if (i == numBoxVertically)
      posX = box.getSize().x + defaultMargin / 2.0f;
    
    // Draw box.
    box.setPosition(posX, (float) (i % numBoxVertically) * (box.getSize().y + gap));
    box.setOutlineColor(m_bots[i]->get_SFML_shape().getOutlineColor());
    m_window.draw(box);
    
    // Draw name.
    m_botName.setString(m_bots[i]->get_name());
    m_botName.setColor(box.getOutlineColor());
    m_botName.setPosition(box.getPosition() + sf::Vector2f(5.0f, 2.0f));
    m_window.draw(m_botName);
    
    // Draw number of kills.
    std::ostringstream numKills;
    numKills << m_bots[i]->get_num_kills();
    m_numKills.setString(numKills.str());
    m_numKills.setCharacterSize(15 + m_bots[i]->get_num_kills() / 2.0f); // Increase the size of the number of kills if it is high (can break the design if it is too high).
    m_numKills.setColor(box.getOutlineColor());
    m_numKills.setPosition(sf::Vector2f(box.getPosition().x + box.getSize().x - m_numKills.getLocalBounds().width - 5.0f, m_botName.getPosition().y));
    m_window.draw(m_numKills);
    
    // Draw health.
    healthBox.setFillColor(box.getOutlineColor());
    for (unsigned int j(0); j < m_bots[i]->get_health(); j++)
    {
      healthBox.setPosition(box.getPosition().x + ((float) j) * (healthBoxWidth + 1.0f) + 1.0f,
                            box.getPosition().y + box.getSize().y - 1.0f - healthBoxHeight);
      m_window.draw(healthBox);
    }
    
    // Draw bullets.
    bulletBox.setFillColor(box.getOutlineColor());
    for (unsigned int j(0); j < m_bots[i]->get_bullets_left(); j++)
    {
      bulletBox.setPosition(box.getPosition().x + ((float) j) * (bulletBox.getSize().x + 4.0f) + 4.0f,
                            box.getPosition().y + box.getSize().y - healthBoxHeight - bulletBox.getSize().y - 3.0f);
      m_window.draw(bulletBox);
    }
  }
  
  // Switch to arena.
  m_window.setView(m_arena);
}
