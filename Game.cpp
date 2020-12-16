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

/*
 * Standalone or online version.
 * 
 * 'LocalView' is the regular version, in standalone mode. It will open a window on the system, and draw the game.
 * 'RemoteView' is the online version. It will open a socket to a client, and send the graphical data to be drawn.
 *
 * If an extra port number is given, the online version will start. Otherwise the standalone version will be used.
 */

#include "Model.h"      // M
#include "LocalView.h"  // V
#include "RemoteView.h" // V
#include "Controller.h" // C
#
#include "Game.h"

Game::Game(int argc, char *argv[]) : m_parser(argc, argv)
{}

bool
Game::start() const
{
  if (m_parser.parsing_succeeded())
  {
    if (m_parser.is_server()) // Local view.
    {
      Model model(m_parser.get_num_bots(), m_parser.get_bot_socket_port(), m_parser.get_contact_socket_port());
      RemoteView view(model, m_parser.get_remote_socket_port(), m_parser.get_contact_socket_port());
      Controller controller(model, view, m_parser.start_paused());
      
      controller.loop();
    }
    else // Remote view.
    {
      Model model(m_parser.get_num_bots(), m_parser.get_bot_socket_port(), m_parser.get_contact_socket_port());
      LocalView view(model);
      Controller controller(model, view, m_parser.start_paused());
      
      controller.loop();
    }
    
    return true;
  }
  else
    return false;
}
