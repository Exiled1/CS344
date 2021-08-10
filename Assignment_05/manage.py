import socket as sock

"""
DS in this: Prio queue, 
"""

HOST = ''       # Get a name from out host & init as blank
PORT = 55555    # Random ass port :shrug:
data_size = 1024
socket = sock.socket(sock.AF_INET, sock.SOCK_STREAM) # initialize a socket listener
socket.bind((HOST, PORT)) # Bind our server to the port.
socket.listen(1) # Wait for a connection.

# ^^ Server initalize.

# Accept requests.

(connection, addr) = socket.accept() # Accept any incomming connection.

print(f"Client IP: {addr}")
while True:
    client_data = connection.recv(data_size) # recieve in the data size from the socket and return it as the client data.
    if not client_data:
        break
    print(f"{client_data} {addr}")
    connection.sendall(client_data)
connection.close()
