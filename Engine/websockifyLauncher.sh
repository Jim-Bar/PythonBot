# PythonBot - A game by a developer for developers.
# Copyright (C) 2015 Jean-Marie BARAN (jeanmarie.baran@gmail.com)

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

# Refer to 'LICENSE.txt' for the full notice.


#!/bin/sh

if [ $# -ne 1 ]; then
  echo "Error: usage is $0 target_port" 1>&2
  echo 0 # Output 0 as port.
  exit 1
fi

netstat -tlpn 2> /dev/null | grep python > openPortsBefore # '2> /dev/null' hide warning (not running as root).
websockify --timeout=5 --idle-timeout=5 --run-once 0 127.0.0.1:$1 > /dev/null & # Start Websockify on a random port, with timeouts in case of fail.
sleep 0.1 # Wait for Websockify to start listening on a port.
netstat -tlpn 2> /dev/null | grep python > openPortsAfter # '2> /dev/null' hide warning (not running as root).
netStatLine=$(diff openPortsBefore openPortsAfter) # Select lines (hopefully only one) which as showed up.
port=$(echo $netStatLine | sed -r 's/.*[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}:([0-9]+).*/\1/') # Grab the port. This regex basically matches *xxx.xxx.xxx.xxx:xxx* where x are figures.

rm openPortsBefore openPortsAfter # Remove the files that are not needed anymore.

if [ -z $port ]; then
  echo "Error: could not retrieve the port on which Websockify is listening" 1>&2
  echo 0 # Output 0 as port.
  exit 1
fi

echo $port # Output the port.
exit 0
