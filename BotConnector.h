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

#ifndef BOT_CONNECTOR__HEADER
#define BOT_CONNECTOR__HEADER

#include <SFML/Network.hpp>

class BotConnector
{
public:
  enum RequestType
  {
    NONE,
    SKIP,
    MOVE,
    ROTATE,
    FIRE,
    SCAN
  };
  
  struct Request
  {
    RequestType type;
    
    // Rotation.
    int rotationDegrees;
    
    // Scan.
    int scanDistance;
    unsigned int scanRadius;
    bool scanCircles;
    bool scanBots;
    bool scanBullets;
  };
  
  /* Destructor. */
  ~BotConnector();
  
  /* Wait for a bot to connect. Put the bot's name in 'botName'. */
  void connect_bot(sf::TcpListener& tcpListener, std::string& botName);
  
  /* Get the request from the bot. Must be called only once per frame since it establishes a connection each call. */
  Request const& get_request();
  
  /* Send answer for a simple request. Communicate bot state. */
  void answer_request(unsigned int health, unsigned int numBulletsLeft, unsigned int numKills);
  
  /* Send answer for a scan request. Communicate also bot state. */
  void answer_scan_request(unsigned int health, unsigned int numBulletsLeft, unsigned int numKills, unsigned int countCircles, unsigned int countBots, unsigned int countBullets);

private:
  static unsigned int const dataSize = 100; // Size of the buffer.
  
  char m_data[dataSize];
  Request m_requestBuffer;
  sf::TcpSocket m_tcpClient;
  
  /* Send the first 'numCharacters' in the buffer 'm_data' to the bot. */
  void send_data(unsigned int numCharacters);
};

#endif
