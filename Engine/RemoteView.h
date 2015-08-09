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

#ifndef REMOTE_VIEW__HEADER
#define REMOTE_VIEW__HEADER

#include "View.h"

class RemoteView : public View
{
public:
  /* Constructor. */
  RemoteView(Model const& model);
  
  /* Destructor. */
  ~RemoteView();
  
  /* Deal with events. */
  EventRequest read_events();
  
  /* Set a list of objects to be drawn. */
  void draw(std::vector<Object*> const& objects);
  
  /* Display the scene. */
  void render();
  
  /* Add a little explosion to the scene. */
  void add_explosion(sf::Vector2f const& position, float radius);
  
  /* Add a bot explosion to the scene. */
  void add_bot_explosion(sf::Vector2f const& position, float radius);
  
  /* Draw "Pause" on top of the game. */
  void draw_pause();

private:
  bool m_clientDisconnected;
  sf::TcpSocket m_tcpClient;
  
  /* Send the initial data (positions and sizes of objects). Refer to the method definition for the frame format. Success : true, fail : false. */
  bool send_initial_state(Model const& model);
};

#endif
