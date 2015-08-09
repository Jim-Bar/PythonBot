import SimpleHTTPServer
import SocketServer

def start_server():
  port = 8000
  handler = SimpleHTTPServer.SimpleHTTPRequestHandler
  httpd = SocketServer.TCPServer(('', port), handler)
  print 'Serving HTTP on 0.0.0.0 port {}...'.format(port)
  httpd.serve_forever()

if __name__ == '__main__':
  try:
    start_server()
  except KeyboardInterrupt:
    print '\nExited'