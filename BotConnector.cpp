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

#include <iostream>
#include <cstdio>
#include <string>
#
#include "BotConnector.h"

unsigned int const BotConnector::dataSize;

BotConnector::~BotConnector()
{
  m_tcpClient.disconnect();
}

void
BotConnector::connect_bot(sf::TcpListener& tcpListener, std::string& botName)
{
  // Accept connection from a bot.
  if (tcpListener.accept(m_tcpClient) == sf::Socket::Done)
  {
    // Receive the name of the bot.
    std::size_t received;
    if (m_tcpClient.receive(m_data, dataSize, received) == sf::Socket::Done)
    {
      m_data[received <= dataSize - 1 ? received : dataSize - 1] = '\0'; // Put a null character to the end.
      botName = m_data;
      answer_request(10, 10, 0);
    }
    else
      std::cerr << "Error : Unable to receive bot name" << std::endl;
    
    m_tcpClient.setBlocking(false);
  }
  else
    std::cerr << "Error : Unable to connect to client" << std::endl;
}

BotConnector::Request const&
BotConnector::get_request()
{
  std::size_t received;

  // Receive a string.
  if (m_tcpClient.receive(m_data, dataSize, received) == sf::Socket::Done)
  {
    m_data[received <= dataSize - 1 ? received : dataSize - 1] = '\0'; // Put a null character to the end.
    unsigned int objectsScanned(0);
    switch (m_data[0])
    {
      case '0':
        m_requestBuffer.type = SKIP;
        break;
      case '1':
        m_requestBuffer.type = MOVE;
        break;
      case '2':
        m_requestBuffer.type = ROTATE;
        m_requestBuffer.rotationDegrees = atoi(m_data + 2);
        break;
      case '3':
        m_requestBuffer.type = FIRE;
        break;
      case '4':
        m_requestBuffer.type = SCAN;
        sscanf(m_data + 2, "%d %u %u", &m_requestBuffer.scanDistance, &m_requestBuffer.scanRadius, &objectsScanned);
        m_requestBuffer.scanCircles = (objectsScanned & 1) != 0;
        m_requestBuffer.scanBots = (objectsScanned & 2) != 0;
        m_requestBuffer.scanBullets = (objectsScanned & 4) != 0;
        break;
      default:
        std::cerr << "Warning : Unknown request received : " << m_data[0] << std::endl;
        m_requestBuffer.type = NONE;
        break;
    }
  }
  else
    m_requestBuffer.type = NONE;
  
  return m_requestBuffer;
}

void
BotConnector::answer_request(unsigned int health, unsigned int numBulletsLeft, unsigned int numKills)
{
  int numCharacters(sprintf(m_data, "%u %u %u", health, numBulletsLeft, numKills));

  if (numCharacters > 0)
    send_data(numCharacters);
  else
    std::cerr << "Error : Unable to write the answer to a request" << std::endl;
}

void
BotConnector::answer_scan_request(unsigned int health, unsigned int numBulletsLeft, unsigned int numKills, unsigned int countCircles, unsigned int countBots, unsigned int countBullets)
{
  int numCharacters(sprintf(m_data, "%u %u %u %u %u %u", health, numBulletsLeft, numKills, countCircles, countBots, countBullets));

  if (numCharacters > 0)
    send_data(numCharacters);
  else
    std::cerr << "Error : Unable to write the answer to a scan request" << std::endl;
}

void
BotConnector::send_data(unsigned int numCharacters)
{
  /*
   * Partial sends occur on asynchronous mode. In case of partial sent, we are expected to send the part of the message which has not been sent.
   * But here we assume that partial sends will never occur because:
   * - The messages are very small
   * - The connection is on local host.
   * So the goal here is just to discard the warning message from SFML when using 'send()' without third argument in asynchronous mode.
   */
  
  // Partial sends warning have been introduced in SFML 2.3. Up to 2.2, no need for using a third parameter for 'send()'.
  #if SFML_VERSION_MAJOR == 2 && SFML_VERSION_MINOR <= 2
  
  if (m_tcpClient.send(m_data, numCharacters) != sf::Socket::Done)
    std::cerr << "Error : Sending data to a bot failed" << std::endl;
  
  #else
  
  size_t sent(0);
  if (m_tcpClient.send(m_data, numCharacters, sent) != sf::Socket::Done) // We use 'sent' to get rid of the SFML warning "Partial sends not handled properly".
    std::cerr << "Error : Sending data to a bot failed" << std::endl;
  
  #endif
}

