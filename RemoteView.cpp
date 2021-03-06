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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#
#include "RemoteView.h"

RemoteView::RemoteView(Model const &model, unsigned int remotePort,
                       unsigned int contactPort)
    : m_model(model), m_clientDisconnected(false) {
  sf::TcpListener tcpListener;

  // Initialize listener for the incoming connections.
  if (tcpListener.listen(remotePort > 0 ? remotePort
                                        : (unsigned int)sf::Socket::AnyPort) !=
      sf::Socket::Done) {
    std::cerr << "Error : Unable to bind the listener to port " << remotePort
              << std::endl;
    // Say the client is disconnected to stop the game.
    m_clientDisconnected = true;
  }

  // Send the remote socket port to the peer...
  if (contactPort > 0) { // ...if a port for this has been provided.
    sf::TcpSocket contactSocket;
    sf::Socket::Status connectionStatus;
    unsigned int attemptsLeft(10);
    do {
      // Connect to the port provided.
      connectionStatus =
          contactSocket.connect(sf::IpAddress::LocalHost, contactPort);
      // If the connection fails, the peer is probably not ready yet.
      if (connectionStatus != sf::Socket::Done)
        sf::sleep(sf::milliseconds(10)); // Wait a little before retrying.
      attemptsLeft--;
    } while (connectionStatus != sf::Socket::Done && attemptsLeft > 0);

    if (connectionStatus == sf::Socket::Done) {
      // Size is 6 because the maximum value is 65535 (5 digits plus '\0').
      char data[6] = {0};
      /* Do not use 'remotePort' here as if it is zero (and it should be the
       * case), a random port has been chosen. */
      int numCharacters(sprintf(data, "%u", tcpListener.getLocalPort()));

      // Send the port the server will listen on.
      if (numCharacters > 0) {
        if (contactSocket.send(data, numCharacters) != sf::Socket::Done)
          std::cerr << "Error : Sending remote socket port to the peer failed"
                    << std::endl;
      } else
        std::cerr
            << "Error : Unable to write the remote socket port to a buffer"
            << std::endl;

      contactSocket.disconnect();
    } else
      std::cerr
          << "Error : Could not connect to the peer to the specified port ("
          << contactPort << ")" << std::endl;
  }

  // Accept connection from a client.
  std::cout << "Waiting for a client..." << std::endl;
  if (tcpListener.accept(m_tcpClient) == sf::Socket::Done) {
    if (!send_initial_state())
      // Say that the client is disconnected to stop the game.
      m_clientDisconnected = true;
    else
      std::cout << "Client connected" << std::endl;
  } else {
    std::cerr << "Error : Unable to connect to client" << std::endl;
    // Say the client is disconnected to stop the game.
    m_clientDisconnected = true;
  }

  // Close the listener (not needed anymore).
  tcpListener.close();
}

RemoteView::EventRequest RemoteView::read_events() {
  if (m_clientDisconnected)
    return REQUEST_QUIT;
  else
    return REQUEST_NONE;
}

/* As the parameter is unused, no need to name it. This remove the "Unused
 * parameter" warning. */
void RemoteView::draw(std::vector<Object *> const &) {
  // Nothing stored here, it is easier to register a reference to the model and
  // asking it at the moment of sending data to the remote client.
}

void RemoteView::render() {
  sf::sleep(sf::milliseconds(20));
  send_current_state();
}

/* As the last parameter is unused, no need to name it. This remove the "Unused
 * parameter" warning. */
void RemoteView::add_explosion(sf::Vector2f const &position, float) {
  // The radius will be choosen by the client.
  m_explosions.push_back(position);
}

/* As the last parameter is unused, no need to name it. This remove the "Unused
 * parameter" warning. */
void RemoteView::add_bot_explosion(sf::Vector2f const &position, float) {
  // The radius will be chosen by the client.
  m_botExplosions.push_back(position);
}

void RemoteView::draw_pause() {}

bool RemoteView::send_initial_state() {
  // clang-format off
  /*
   * The format of the frame is as follow :
   *
   *  0               1               2               3               4               5               6
   *  0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * |             Width             |             Height            |Number of walls|Number of bots |
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * |      Wall x position (1)      |      Wall y position (1)      |Wall radius (1)| Color bot (2) | <-- Repeat (1) for each wall (repeat the entire block (1) each time).
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ <-- Exact position in frame become unknown (as walls vary in numbers).
   * |      Bot x position (2)       |      Bot y position (2)       |        Bot rotation (2)       |
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * |Name length (2)|                               Bot name (2)                                    | <-- Repeat (2) for each bot (repeat the entire block (2) each time).
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   *
   * Width, height : in pixels.
   * Positions, radius : in pixels.
   * Rotation : in degrees.
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
   */
  // clang-format on

  // Walls and bots are not yet taken into account here (see below). In bytes.
  unsigned int frameSize(2 + 2 + 1 + 1);

  // Compute size for the walls.
  // Positions and radius of the walls.
  frameSize += (2 + 2 + 1) * m_model.get_circles().size();

  // Get all bots' names, and compute the total frameSize.
  std::vector<std::string> botNames;
  for (std::vector<Object *>::const_iterator botIter(
           m_model.get_alive_bots().begin());
       botIter != m_model.get_alive_bots().end(); botIter++) {
    botNames.push_back(((Bot *)*botIter)->get_name());
    // Colors, positions, rotations and names of the bots.
    frameSize += 1 + 2 + 2 + 2 + 1 + botNames.back().size();
  }

  // Allocate the frame.
  char *initialState(0);
  initialState = (char *)malloc(frameSize);
  if (initialState == 0) {
    std::cerr
        << "Error : Could not allocate the memory for the initial state frame"
        << std::endl;
    return false;
  }

  // Fill the frame.
  unsigned int index(0); // Current position in the frame.
  initialState[index++] = m_model.get_width() >> 8;
  initialState[index++] = m_model.get_width();
  initialState[index++] = m_model.get_height() >> 8;
  initialState[index++] = m_model.get_height();
  initialState[index++] = m_model.get_circles().size();
  initialState[index++] = m_model.get_alive_bots().size();
  for (unsigned int i(0); i < m_model.get_circles().size(); i++) {
    // Get a direct pointer for convenience.
    Circle *wall((Circle *)m_model.get_circles()[i]);
    initialState[index++] = (int)wall->get_position().x >> 8;
    initialState[index++] = (int)wall->get_position().x;
    initialState[index++] = (int)wall->get_position().y >> 8;
    initialState[index++] = (int)wall->get_position().y;
    initialState[index++] = (int)wall->get_radius();
  }
  for (unsigned int i(0); i < m_model.get_alive_bots().size(); i++) {
    // Get a direct pointer for convenience.
    Bot *bot((Bot *)m_model.get_alive_bots()[i]);
    initialState[index++] = i; // The ID.
    initialState[index++] = (int)bot->get_position().x >> 8;
    initialState[index++] = (int)bot->get_position().x;
    initialState[index++] = (int)bot->get_position().y >> 8;
    initialState[index++] = (int)bot->get_position().y;
    initialState[index++] = (int)bot->get_SFML_shape().getRotation() >> 8;
    initialState[index++] = (int)bot->get_SFML_shape().getRotation();
    initialState[index++] = bot->get_name().length();
    bot->get_name().copy(initialState + index, bot->get_name().length());
    index += bot->get_name().length(); // Update index position.
  }

  // Finally check the data written is consistent with the expected length.
  if (frameSize != index)
    std::cerr << "Error : The expected length of the initial state frame is "
                 "wrong (expected "
              << frameSize << ", got " << index << "). Sending data even so"
              << std::endl;

  // Send the data.
  if (m_tcpClient.send(initialState, frameSize) != sf::Socket::Done) {
    std::cerr << "Error : Sending initial data to the client failed"
              << std::endl;
    free(initialState);
    return false;
  }

  free(initialState);
  return true;
}

void RemoteView::send_current_state() {
  // clang-format off
  /*
   * The format of the frame is as follow :
   *
   *  0               1               2               3               4               5               6               7
   *  0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7 0 1 2 3 4 5 6 7
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * |P|  Bot ID   |  Bot rotation   |A|S| Life  | Num kills |Bullets|        Bot x position         |        Bot y position         |
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   * |        Scan x position        |        Scan y position        |          Scan radius          |Num of bullets |       Bullet x  <-- Repeat the entire block (beginning to scan y position OR bot y position, see below) for each bot.
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ <-- Exact position in frame become unknown (as bots vary in numbers).
   *  position (1)   |     Bullet y position (1)     |Num explosions |   Explosion x position (2)    |   Explosion y position (2)    | <-- Repeat (1) for each bullet (repeat the entire block (1) each time).
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+ <-- Repeat (2) for each explosion (repeat the entire block (2) each time).
   * |Num EXPLOSIONS |    Explosion x position (3)   |    Explosion y position (3)   |                                                 <-- Repeat (3) for each bot explosion (repeat the entire block (3) each time).  
   * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   *
   *
   * Caption:
   * - 'P': 1 if paused, 0 otherwise. Note that this bit is actually repeated
   *   for each bot, but only the first one describe the pause status. All
   *   others are unused.
   * - 'Bot ID': ID of the bot. Max is 63.
   * - 'Bot rotation': rotation in degrees. /!\ This field is 9 bits /!\. Max is
   *   511.
   * - 'A': 1 if the bot is alive, 0 otherwise.
   * - 'S': 1 if the scan is active (the bot has scanned at least once and is
   *   alive), 0 otherwise.
   * - 'Life': remaining life of the bot. Max is 15.
   * - 'Num kills': number of kills made by the bot. Max is 63.
   * - 'Bullets': number of remaining bullets of the bot. Max is 15.
   * - ... (obvious fields)...
   * - 'Num explosions': are the little explosions. Max is 255.
   * - 'Num EXPLOSIONS': are the big explosions. Max is 255.
   *
   * Notes:
   * - The scan position/radius blocks are sent only if the bot has scanned at
   *   least once (bit 'S' to 1), and if it is alive.
   * - Only the initial explosions are sent. The client must then manage their
   *   lifetime.
   */
  // clang-format on

  unsigned int frameSize(0); // In bytes.

  // Compute size for the bots.
  for (unsigned int i(0); i < m_model.get_bots().size(); i++) {
    // The scan is sent only if the bot is alive.
    frameSize += 4 + 2 + 2 +
                 (((Bot *)m_model.get_bots()[i])->get_scan().get_active() &&
                          ((Bot *)m_model.get_bots()[i])->is_alive()
                      ? 2 + 2 + 2
                      : 0);
  }

  // Compute size for the bullets and the explosions.
  frameSize += 1 + (2 + 2) * m_model.get_bullets().size();
  frameSize += 1 + (2 + 2) * m_explosions.size();
  frameSize += 1 + (2 + 2) * m_botExplosions.size();

  // Fill the frame.
  unsigned int index(0); // Current position in the frame.
  unsigned int const maxFrameSize(2048);
  // Static allocation is better than allocating / deallocating continuously.
  unsigned char currentState[maxFrameSize] = {
      0 /*false ? (char) 1 << 7 : 0*/ /* The pause */};
  for (unsigned int i(0); i < m_model.get_bots().size();
       i++) { // Loop over alive and dead bots all alike.
    // Get a direct pointer for convenience, taking from the correct list.
    Bot *bot((Bot *)m_model.get_bots()[i]);
    /* Color. Do not remove '|', don't forget the pause! Do not replace '|=' by
     * '+=' as the type is char and not unsigned char! */
    currentState[index] |= i << 1;
    // High order first bit of the rotation.
    currentState[index++] |= (int)bot->get_SFML_shape().getRotation() >> 8;
    // Truncated automatically.
    currentState[index++] = (int)bot->get_SFML_shape().getRotation();
    currentState[index] = (bot->is_alive() ? 1 : 0) << 7;
    // Mark also the scan as inactive if the bot is dead.
    currentState[index] |=
        (bot->get_scan().get_active() && bot->is_alive() ? 1 : 0) << 6;
    currentState[index] |= bot->get_health() << 2;
    currentState[index++] |= bot->get_num_kills() >> 4;
    currentState[index] = bot->get_num_kills() << 4;
    currentState[index++] |= bot->get_bullets_left();
    currentState[index++] = (int)bot->get_position().x >> 8;
    currentState[index++] = (int)bot->get_position().x;
    currentState[index++] = (int)bot->get_position().y >> 8;
    currentState[index++] = (int)bot->get_position().y;
    if (bot->get_scan().get_active() &&
        bot->is_alive()) { // The scan is sent only if the bot is alive.
      currentState[index++] = (int)bot->get_scan().get_position().x >> 8;
      currentState[index++] = (int)bot->get_scan().get_position().x;
      currentState[index++] = (int)bot->get_scan().get_position().y >> 8;
      currentState[index++] = (int)bot->get_scan().get_position().y;
      currentState[index++] = (int)bot->get_scan().get_radius() >> 8;
      currentState[index++] = (int)bot->get_scan().get_radius();
    }
  }
  currentState[index++] = m_model.get_bullets().size();
  for (unsigned int i(0); i < m_model.get_bullets().size(); i++) {
    // Get a direct pointer for convenience.
    Bullet *bullet((Bullet *)m_model.get_bullets()[i]);
    currentState[index++] = (int)bullet->get_position().x >> 8;
    currentState[index++] = (int)bullet->get_position().x;
    currentState[index++] = (int)bullet->get_position().y >> 8;
    currentState[index++] = (int)bullet->get_position().y;
  }
  currentState[index++] = m_explosions.size();
  for (unsigned int i(0); i < m_explosions.size(); i++) {
    currentState[index++] = (int)m_explosions[i].x >> 8;
    currentState[index++] = (int)m_explosions[i].x;
    currentState[index++] = (int)m_explosions[i].y >> 8;
    currentState[index++] = (int)m_explosions[i].y;
  }
  currentState[index++] = m_botExplosions.size();
  for (unsigned int i(0); i < m_botExplosions.size(); i++) {
    currentState[index++] = (int)m_botExplosions[i].x >> 8;
    currentState[index++] = (int)m_botExplosions[i].x;
    currentState[index++] = (int)m_botExplosions[i].y >> 8;
    currentState[index++] = (int)m_botExplosions[i].y;
  }

  // Clear the explosions for the next iteration.
  m_explosions.clear();
  m_botExplosions.clear();

  // Finally check the data written is consistent with the expected length.
  if (frameSize != index)
    std::cerr << "Error : The expected length of the current state frame is "
                 "wrong (expected "
              << frameSize << ", got " << index << "). Sending data even so"
              << std::endl;

  if (frameSize > maxFrameSize) {
    // Exit the game, as the view will fail on the client side.
    std::cerr << "Error : The frame reached the maximum size allowed ("
              << maxFrameSize << " bytes). Consider raising this value"
              << std::endl;
    m_clientDisconnected = true;
  } else {
    // Send the data.
    sf::Socket::Status status(m_tcpClient.send(currentState, frameSize));
    if (status != sf::Socket::Done) {
      std::cerr << "Error : Sending current state to the client failed"
                << std::endl;
      if (status == sf::Socket::Disconnected) {
        /* Exit the game only if the client disconnected. Otherwise, try
         * continuing. */
        m_tcpClient.disconnect();
        m_clientDisconnected = true;
      }
    }
  }
}
