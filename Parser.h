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

#ifndef PARSER__HEADER
#define PARSER__HEADER

#include <string>

class Parser
{
public:
  /* Constructor. */
  Parser(int argc, char *argv[]);

  /* Getters. */
  bool parsing_succeeded() const; // Return 'true' if the parsing succeeded. This value MUST be checked before doing anything else.
  bool is_server() const;
  bool start_paused() const;
  unsigned int get_num_bots() const;
  unsigned int get_bot_socket_port() const;
  unsigned int get_remote_socket_port() const;
  unsigned int get_contact_socket_port() const;

private:
  char **m_begin;
  char **m_end;
  
  bool m_parsingSucceeded;
  bool m_isServer; // 'true' means the game use the remote view instead of the local view.
  bool m_startPaused; // 'true' means the game should be paused on the beginning.
  unsigned int m_numBots;
  unsigned int m_botSocketPort;
  unsigned int m_remoteSocketPort;
  unsigned int m_contactSocketPort;
  
  bool has_command_option(std::string const& option);
  char* get_command_option(std::string const& option);
  
  void print_usage_with_warning(char const *warning);
  void print_usage_with_error(char const *error);
  void print_usage_with_level(char const *level, char const *message);
};

#endif
