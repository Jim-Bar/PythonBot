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

#include <cstdlib>
#include <iostream>
#
#include "RemoteView.h"

RemoteView::RemoteView(Model const& model) : m_clientDisconnected(false)
{
  sf::TcpListener tcpListener;
  unsigned int port(6006);
  
  // Initialize listener for the incoming connections.
  if (tcpListener.listen(port) != sf::Socket::Done)
  {
    std::cerr << "Error : Unable to bind the listener to port " << port << std::endl;
    m_clientDisconnected = true; // Say the client is disconnected to stop the game.
  }
  
  // Accept connection from a bot.
  std::cout << "Waiting for a client..." << std::endl;
  if (tcpListener.accept(m_tcpClient) == sf::Socket::Done)
  {
    if (!send_initial_state(model))
      m_clientDisconnected = true; // Say the client is disconnected to stop the game.
    else
      m_tcpClient.setBlocking(false);
  }
  else
  {
    std::cerr << "Error : Unable to connect to client" << std::endl;
    m_clientDisconnected = true; // Say the client is disconnected to stop the game.
  }
  
  // Close the listener (not needed anymore).
  tcpListener.close();
}

RemoteView::~RemoteView()
{
  m_tcpClient.disconnect();
}

RemoteView::EventRequest
RemoteView::read_events()
{
  if (m_clientDisconnected || true)
    return REQUEST_QUIT;
  else
    return REQUEST_NONE;
}
  
void
RemoteView::draw(std::vector<Object*> const& objects)
{}
  
void
RemoteView::render()
{}
  
void
RemoteView::add_explosion(sf::Vector2f const& position, float radius)
{}
  
void
RemoteView::add_bot_explosion(sf::Vector2f const& position, float radius)
{}
  
void
RemoteView::draw_pause()
{}

bool
RemoteView::send_initial_state(Model const& model)
{
  /*
   * The format of the frame is as follow :
   *
   *  0                   1                   2                   3                   4
   *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * |                 Width                 |                 Height                |  Number of walls  |   Number of bots  |
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * |          Wall x position (1)          |          Wall y position (1)          |  Wall radius (1)  |  Color of bot (2) | <-- Repeat (1) for each wall (repeat the entire block (1) each time).
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ <-- Exact position in frame become unknown (as walls vary in numbers).
   * |          Bot x position (2)           |          Bot y position (2)           |            Bot rotation (2)           |
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * |  Name length (2)  |                                           Bot name (2)                                            | <-- Repeat (2) for each bot (repeat the entire block (2) each time).
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * 
   * Width, height : in pixels.
   * Radius : in pixels.
   * 
   * Colors are :
   * 0 : Red
   * 1 : Green
   * 2 : Blue
   * 3 : Yellow
   * 4 : Magenta
   * 5 : Cyan
   * 6 : Orange
   * 7 : Purple
   * 
   * Rotation : in degrees.
   */
  unsigned int frameSize(2 + 2 + 1 + 1); // Walls and bots are not yet taken into account here (see below).
  
  // Compute size for the walls.
  frameSize += (2 + 2 + 1) * model.get_circles().size(); // Positions and radius of the walls.
  
  // Get all bots' names, and compute the total frameSize.
  std::vector<std::string> botNames;
  for (std::vector<Object*>::const_iterator botIter(model.get_bots().begin()); botIter != model.get_bots().end(); botIter++)
  {
    botNames.push_back(((Bot*) *botIter)->get_name());
    frameSize += 1 + 2 + 2 + 2 + 1 + botNames.back().size(); // Colors, positions, rotations and names of the bots.
  }
  
  // Allocate the frame.
  char *initialState(0);
  initialState = (char*) malloc(frameSize);
  if (initialState == 0)
  {
    std::cerr << "Error : Could not allocate the memory for the initial state frame" << std::endl;
    return false;
  }
  
  // Fill the frame.
  unsigned int index(0); // Current position in the frame.
  initialState[index++] = model.get_width() >> 8;
  initialState[index++] = model.get_width();
  initialState[index++] = model.get_height() >> 8;
  initialState[index++] = model.get_height();
  initialState[index++] = model.get_circles().size();
  initialState[index++] = model.get_bots().size();
  for (unsigned int i(0); i < model.get_circles().size(); i++)
  {
    Circle *wall((Circle*) model.get_circles()[i]); // Get a direct pointer for convenience.
    initialState[index++] = (int) wall->get_position().x >> 8;
    initialState[index++] = (int) wall->get_position().x;
    initialState[index++] = (int) wall->get_position().y >> 8;
    initialState[index++] = (int) wall->get_position().y;
    initialState[index++] = (int) wall->get_radius();
  }
  for (unsigned int i(0); i < model.get_bots().size(); i++)
  {
    Bot *bot((Bot*) model.get_bots()[i]); // Get a direct pointer for convenience.
    initialState[index++] = i; // The color.
    initialState[index++] = (int) bot->get_position().x >> 8;
    initialState[index++] = (int) bot->get_position().x;
    initialState[index++] = (int) bot->get_position().y >> 8;
    initialState[index++] = (int) bot->get_position().y;
    initialState[index++] = (int) bot->get_SFML_shape().getRotation() >> 8;
    initialState[index++] = (int) bot->get_SFML_shape().getRotation();
    initialState[index++] = bot->get_name().length();
    bot->get_name().copy(initialState + index, bot->get_name().length());
    index += bot->get_name().length(); // Update index position.
  }
  
  // Finally check the data written is consistent with the expected length.
  if (frameSize != index)
    std::cerr << "Error : The expected length of the initial state frame is wrong (expected " << frameSize << ", got " << index << "). Sending data even so" << std::endl;
  
  // Send the data.
  if (m_tcpClient.send(initialState, frameSize) != sf::Socket::Done)
  {
    std::cerr << "Error : Sending initial data to the client failed" << std::endl;
    free(initialState);
    return false;
  }
  
  free(initialState);
  return true;
}
