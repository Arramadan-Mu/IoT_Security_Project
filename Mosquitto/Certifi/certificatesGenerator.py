import socket
import subprocess
import shutil

def get_ipv4_address(interface='Wi-Fi'):
    try:
        # Get the IPv4 address of the specified network interface (default is Wi-Fi)
        ip_address = socket.gethostbyname(socket.gethostname())
        return ip_address
    except socket.error as e:
        print(f"Error getting IPv4 address: {e}")
        return None

def adjust_last_octet(ip_address):
    # Adjust the last octet of the IP address for the client's Common Name
    octets = ip_address.split('.')
    octets[-1] = str(int(octets[-1]) + 1)  # Increment the last octet
    return '.'.join(octets)

def generate_certificates(ip_address, common_name):
    try:
        print(f"Generating certificates for IP address: {ip_address}")

        # Generate CA Key and Certificate
        subprocess.run(["openssl", "ecparam", "-name", "prime256v1", "-genkey", "-noout", "-out", "ca.key"])
        subprocess.run(["openssl", "req", "-new", "-x509", "-sha256", "-days", "365", "-key", "ca.key", "-out", "ca.crt",
                        "-subj", f"/C=IT/ST=Calabria/L=Rende/O=Unical/OU=Dimes/CN={common_name}/emailAddress=muhammedarrmadan@gmail.com"])

        # Generate Server Key and CSR
        subprocess.run(["openssl", "ecparam", "-name", "prime256v1", "-genkey", "-noout", "-out", "server_mqtt.key"])
        subprocess.run(["openssl", "req", "-new", "-sha256", "-key", "server_mqtt.key", "-out", "server_mqtt.csr",
                        "-subj", f"/C=IT/ST=Calabria/L=Rende/O=Unical/OU=Dimes/CN={common_name}/emailAddress=muhammedarrmadan@gmail.com"])

        # Sign Server CSR with CA to Generate Server Certificate
        subprocess.run(["openssl", "x509", "-req", "-in", "server_mqtt.csr", "-CA", "ca.crt", "-CAkey", "ca.key",
                        "-CAcreateserial", "-out", "server.crt", "-days", "1000", "-sha256"])

        # Adjust the last octet of the IP address for the client's Common Name
        client_common_name = adjust_last_octet(ip_address)

        # Generate Client Key and CSR
        subprocess.run(["openssl", "ecparam", "-name", "prime256v1", "-genkey", "-noout", "-out", "client_mqtt.key"])
        subprocess.run(["openssl", "req", "-new", "-sha256", "-key", "client_mqtt.key", "-out", "client_mqtt.csr",
                        "-subj", f"/C=IT/ST=Calabria/L=Rende/O=Unical/OU=Dimes/CN={client_common_name}/emailAddress=muhammedarrmadan@gmail.com"])

        # Sign Client CSR with CA to Generate Client Certificate
        subprocess.run(["openssl", "x509", "-req", "-in", "client_mqtt.csr", "-CA", "ca.crt", "-CAkey", "ca.key",
                        "-CAcreateserial", "-out", "client_mqtt.crt", "-days", "1000", "-sha256"])

        # Export CA and Client Certificates in PEM format
        subprocess.run(["openssl", "x509", "-in", "ca.crt", "-out", "ca_cert.pem", "-outform", "PEM"])
        subprocess.run(["openssl", "x509", "-in", "client_mqtt.crt", "-out", "client_mqtt_cert.pem", "-outform", "PEM"])

        # Copy Client Key to PEM file using shutil.copy
        shutil.copy("client_mqtt.key", "client_mqtt_key.pem")

        print("Certificates generated successfully.")

    except subprocess.CalledProcessError as e:
        print(f"Error during certificate generation: {e}")

if __name__ == "__main__":
    wifi_interface = 'Wi-Fi'  # Adjust this if needed
    ipv4_address = get_ipv4_address(wifi_interface)

    if ipv4_address:
        generate_certificates(ipv4_address, ipv4_address)
    else:
        print("Failed to retrieve IPv4 address.")
