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

#ifndef VIEW__HEADER
#define VIEW__HEADER

#include <vector>
#include <SFML/Graphics.hpp>
#
#include "Model.h"

class View
{
public:
  /* Constructor. */
  View(Model const& model, sf::Color edgesColor = sf::Color(128, 128, 128));
  
  /* Getters. */
  sf::RenderWindow& get_window();
  
  /* Set a list of objects to be drawn. */
  void draw(std::vector<Object*> const& objects);
  
  /* Display the scene. */
  void render();
  
  /* Add a little explosion to the scene. */
  void add_explosion(sf::Vector2f const& position, float radius = 1.0f);
  
  /* Add a bot explosion to the scene. */
  void add_bot_explosion(sf::Vector2f const& position, float radius = 5.0f);
  
  /* Re-compute the positions and sizes of the views. */
  void resize();
  
  /* Draw "Pause" on top of the game. */
  void draw_pause();

private:
  static float const edgesWidth = 10.0f;
  static float const defaultMargin = 50.0f; // Default margin between the arena and the pannel.
  
  sf::RenderWindow m_window;
  sf::View m_arena;
  sf::View m_pannel;
  sf::RectangleShape m_edges[4]; // World's boundaries.
  std::vector<sf::CircleShape*> m_explosions;
  sf::Font m_font; // Used for bots' names.
  sf::Text m_botName;
  sf::Text m_numKills; // Number of kills for the bot.
  sf::Text m_pause;
  std::vector<Bot const*> m_bots; // Pointers to the bots (just to draw their info).
  
  /* Draw world's edges and explosions. */
  void draw_edges();
  void draw_explosions();
  
  /* Display bot's information. */
  void draw_bots_info();
};

#endif
