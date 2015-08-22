# Python version 2.7 or 3.3+
try:
  # Python 3.3+
  from http.server import SimpleHTTPRequestHandler
  from socketserver import TCPServer
except ImportError:
  # Python 2.7
  from SimpleHTTPServer import SimpleHTTPRequestHandler
  from SocketServer import TCPServer

def start_server():
  port = 8000
  handler = SimpleHTTPRequestHandler
  httpd = TCPServer(('', port), handler)
  print('Serving HTTP on 0.0.0.0 port {}...'.format(port))
  httpd.serve_forever()

if __name__ == '__main__':
  try:
    start_server()
  except KeyboardInterrupt:
    print('\nExited')