#include <algorithm> // For std::find().
#include <cstdlib> // For atoi().
#include <iostream>
#
#include "Parser.h"

Parser::Parser(int argc, char *argv[]) : m_begin(argv), m_end(argv + argc), m_parsingSucceeded(true), m_isServer(false), m_startPaused(false), m_numBots(0), m_botSocketPort(0), m_contactSocketPort(0)
{
  // 's' like 'Server'. Will run in remote view mode.
  if (has_command_option("-s"))
    m_isServer = true;
  
  // 'p' like 'Paused'. Will start paused.
  if (has_command_option("-p"))
    m_startPaused = true;
  
  // 'n' like 'Number' of bots.
  if (has_command_option("-n"))
  {
    int numBots(atoi(get_command_option("-n")));
    if (numBots > 0)
      m_numBots = (unsigned int) numBots;
    else
    {
      print_usage_with_error("Invalid number of bots");
      m_parsingSucceeded = false;
    }
  }
  
  int port(0);
  if (has_command_option("-b")) // 'b' like 'Bot' socket. Port to be used by the bots to connect to the game.
  {
    port = atoi(get_command_option("-b"));
    if (port > 0)
      m_botSocketPort = (unsigned int) port;
  }
  else if (has_command_option("-c")) // 'c' like 'Contact' socket. Port to be used to send the port chosen for the bot socket back to the Python module.
  {
    port = atoi(get_command_option("-c"));
    if (port > 0)
      m_contactSocketPort = (unsigned int) port;
  }
  
  // We need at least (and probably at most) a port: if we have no port assigned, and no possibility to chose one, then the game cannot work.
  if (m_botSocketPort == 0 && m_contactSocketPort == 0)
  {
    print_usage_with_error("The bot socket port or the contact socket port must be provided");
    m_parsingSucceeded = false;
  }
}

bool
Parser::parsing_succeeded() const
{
  return m_parsingSucceeded;
}

bool
Parser::is_server() const
{
  return m_isServer;
}

bool
Parser::start_paused() const
{
  return m_startPaused;
}

unsigned int
Parser::get_num_bots() const
{
  return m_numBots;
}

unsigned int
Parser::get_contact_socket_port() const
{
  return m_contactSocketPort;
}

unsigned int
Parser::get_bot_socket_port() const
{
  return m_botSocketPort;
}

bool
Parser::has_command_option(std::string const& option)
{
  return std::find(m_begin, m_end, option) != m_end;
}

char*
Parser::get_command_option(std::string const& option)
{
  char ** itr = std::find(m_begin, m_end, option);
  if (itr != m_end && ++itr != m_end)
    return *itr;
  
  return 0;
}

void
Parser::print_usage_with_error(char const *error)
{
  std::cerr << "Usage:" << std::endl;
  std::cerr << "  " << *m_begin << " [-p] [-s] [-b bot_socket_port | -c contact_socket_port]" << std::endl;
  std::cerr << std::endl;
  std::cerr << *m_begin << ": Error: " << error << std::endl;
}
