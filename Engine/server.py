import os # To construct cross-compatible paths.
import socket
import pythonbot
from time import sleep

# Server configuration.
host = '127.0.0.1' # We are the host.
port = 5006 # Port to listen to.
bufferSize = 256 # Maximum data we will receive.

# Create a new TCP socket.
tcpListener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)    

try:
  tcpListener.bind((host, port)) # Bind the socket to the address (double parenthesis build a tuple).
  
  while True:
    tcpListener.listen(5) # Allow at most 5 queued connections.
    print 'Listening on 0.0.0.0 port {}...'.format(port)

    tcpSocket, address = tcpListener.accept() # Accept the connection.
    print 'A client just connected: {}'.format(address)
    botCode = ''
    botCode = tcpSocket.recv(bufferSize)
    if botCode == '':
      print 'Received empty string ! Closing down.'
    else:
      print 'Received:\n"\n{}\n"\nfrom {}'.format(botCode, address)
    tcpSocket.close()
    print 'Connection with {} closed'.format(address)
    
    try:
      os.remove(os.path.join('Bots', 'imported.pyc')) # Remove the previously compiled bot file.
    except OSError:
      pass # If the file does not exists, no need to delete it.
    botFile = open(os.path.join('Bots', 'imported.py'), 'w')
    botFile.write(botCode)
    botFile.close()
    pythonbot.launch_game()
except KeyboardInterrupt:
  tcpListener.close()
  print 'Exited'
