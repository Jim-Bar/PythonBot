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
  if (m_tcpClient.send(m_data, numCharacters) != sf::Socket::Done)
    std::cerr << "Error : Sending data to a bot failed" << std::endl;
}

