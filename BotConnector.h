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
