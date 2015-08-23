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

#include <algorithm> // For std::find().
#include <cstdlib> // For atoi().
#include <iostream>
#
#include "Parser.h"

Parser::Parser(int argc, char *argv[]) : m_begin(argv), m_end(argv + argc), m_parsingSucceeded(true), m_isServer(false), m_startPaused(false), m_numBots(0), m_botSocketPort(0), m_remoteSocketPort(0), m_contactSocketPort(0)
{
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
  
  // 'p' like 'Paused'. Will start paused.
  if (has_command_option("-p"))
    m_startPaused = true;
  
  int port(0);
  if (has_command_option("-c")) // 'c' like 'Contact' socket. Port to be used to send the port chosen for the bot socket back to the Python module.
  {
    port = atoi(get_command_option("-c"));
    if (port > 0)
      m_contactSocketPort = (unsigned int) port;
    else
    {
      print_usage_with_error("Could not read contact socket port");
      m_parsingSucceeded = false;
    }
    
    // 's' like 'Server'. Will run in remote view mode.
    if (has_command_option("-s"))
      m_isServer = true;
  }
  else if (has_command_option("-b")) // 'b' like 'Bot' socket. Port to be used by the bots to connect to the game.
  {
    port = atoi(get_command_option("-b"));
    if (port > 0)
      m_botSocketPort = (unsigned int) port;
    else
    {
      print_usage_with_error("Could not read bot socket port");
      m_parsingSucceeded = false;
    }
    
    // 's' like 'Server'. Will run in remote view mode.
    if (has_command_option("-s"))
    {
      m_isServer = true;
      if (has_command_option("-r")) // 'r' like 'Remote' socket. Port to be used by the clients to receive the game state.
      {
	port = atoi(get_command_option("-r"));
	if (port > 0)
	  m_remoteSocketPort = (unsigned int) port;
	else
	{
	  print_usage_with_error("Could not read remote socket port");
	  m_parsingSucceeded = false;
	}
      }
      else
      {
	print_usage_with_error("The remote socket port must be provided when in server mode and not using the contact socket");
	m_parsingSucceeded = false;
      }
    }
  }
  else
  {
    print_usage_with_error("Either the contact socket port or the bot socket port must be provided");
    m_parsingSucceeded = false;
  }
  
  // Print a warning if both bot socket and contact socket ports are provided. It makes no sense of providing a port, and then expecting to receive it.
  if (has_command_option("-c") && has_command_option("-b"))
    print_usage_with_warning("Both bot socket and contact socket ports are provided. Bot socket port ignored");
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
Parser::get_bot_socket_port() const
{
  return m_botSocketPort;
}

unsigned int
Parser::get_remote_socket_port() const
{
  return m_remoteSocketPort;
}

unsigned int
Parser::get_contact_socket_port() const
{
  return m_contactSocketPort;
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
Parser::print_usage_with_warning(char const *warning)
{
  print_usage_with_level("Warning", warning);
}

void
Parser::print_usage_with_error(char const *error)
{
  print_usage_with_level("Error", error);
}

void
Parser::print_usage_with_level(char const *level, char const *message)
{
  std::cerr << "Usage:" << std::endl;
  std::cerr << "  " << *m_begin << " -n {num_bots} [-p] [-c {contact_socket_port} [-s] | -b {bot_socket_port} [-s -r {remote_socket_port}]]" << std::endl;
  std::cerr << std::endl;
  std::cerr << *m_begin << ": " << level << " : " << message << std::endl;
}
