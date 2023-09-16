import socket


sockfd = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sockfd.bind(('0.0.0.0', 54154))

print("My echo server is listening on port: 54154")

while True:
	payload, client_addr = sockfd.recvfrom(1024)
	sockfd.sendto(payload, client_addr)

# - **Step 1** There's an echo server program. 
# - **Step 2** Write a service unit file to manage the above executable.
# - **Step 3** Copy the executable in **/usr/local/bin/** directory.
# and the service unit file in the **/etc/systemd/system/**
# directory.
# - **Step 4** Start/stop the service using systemctl and enjoy.