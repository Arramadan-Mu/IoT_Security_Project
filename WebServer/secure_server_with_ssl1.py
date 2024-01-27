import http.server
import ssl
import base64
import socket
import logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s [%(levelname)s] - %(message)s',
    handlers=[
        logging.FileHandler('server.log'),  # Log to a file
        logging.StreamHandler()              # Log to the console
    ]
)
class TimedHTTPHandler(http.server.SimpleHTTPRequestHandler):
    def do_GET(self):
        logging.info(f'Received GET request: {self.path}')
        auth_header = self.headers.get('Authorization')
        if auth_header is None or not auth_header.startswith('Basic '):
            logging.warning("No Authorization header received")
            self.send_response(401)
            self.send_header('WWW-Authenticate', 'Basic realm="Secure Area"')
            self.end_headers()
            return
        credentials = base64.b64decode(auth_header.split(' ')[1]).decode('utf-8')
        username, password = credentials.split(':', 1)

        logging.info(f"Received credentials - Username: {username}, Password: {password}")
        if username == 'root' and password == '1234543210':
            self.connection.settimeout(10)
            print("Authorization Accepted for Root User ...")
            print(f"Account {username} accepted")
            self.send_response(200)
            self.send_header('Content-Security-Policy', "default-src 'self'")
            self.end_headers()
            super().do_GET()
        elif username == 'mo' and password == '1234543210' and self.path == '/report.csv':
            self.connection.settimeout(10)
            print("Authorization Accepted for Downloading only ...")
            print(f"Account {username} accepted")
            self.send_response(200)
            self.send_header('Content-Security-Policy', "default-src 'self'")
            self.end_headers()
            super().do_GET()
        else:
            logging.warning("Authorization not accepted")
            self.send_response(401)
            self.send_header('WWW-Authenticate', 'Basic realm="Secure Area"')
            self.end_headers()
    def handle_request(self):
        try:
            super().handle_request()
        except socket.timeout:
            logging.warning("Session closed due to timeout")
wifi_ip = socket.gethostbyname(socket.gethostname())
server_address = (wifi_ip, 8443)

# Specify the paths to your CA certificate, server certificate, and private key files
ssl_cafile = 'ca_cert.pem'
ssl_certfile = 'server_cert.pem'
ssl_keyfile = 'server_key.pem'

# Create an SSL context with your certificates and private key
ssl_context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
ssl_context.load_verify_locations(cafile=ssl_cafile)
ssl_context.load_cert_chain(certfile=ssl_certfile, keyfile=ssl_keyfile)
ssl_context.verify_mode = ssl.CERT_OPTIONAL  # or ssl.CERT_REQUIRED

if __name__ == '__main__':
    httpd = http.server.HTTPServer(server_address, TimedHTTPHandler)
    httpd.socket = ssl_context.wrap_socket(httpd.socket, server_side=True)

    logging.info(f'Starting secure server on {server_address[0]}:{server_address[1]}...')
    httpd.serve_forever()