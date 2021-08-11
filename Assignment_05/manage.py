import socket as Socket

"""
DS in this: Prio queue, 
"""





# Address Family: Ipv4 
# Socket stream



# ^^ Server initalize.

# Accept requests.


class CPacket:
    pass



"""
Server connection definition
    - Monitor connections, queue their results depending on a parsed request, pog.

"""
class Server:
    COMPUTE_SUBMIT_SIZE = 256
    COMPUTE_QUERY_SIZE = 182
    HOST = ''       # Get a name from out host & init as blank
    PORT = 55555    # Random ass port :shrug:
    data_size = 1024
    QUEUE_SIZE = 10
    # Constructor.
    def __init__(self):
        self.process_queue = []
        self.__server_connection()

    # Privatized server connection.
    def __server_connection(self):
        socket = Socket.socket(Socket.AF_INET, Socket.SOCK_STREAM) # initialize a socket listener
        socket.bind((self.HOST, self.PORT)) # Bind our server to the port.
        socket.listen(self.QUEUE_SIZE) # Wait for a connection.
        (connection, addr) = socket.accept() # Accept any incomming connection.
        print(f"Client Connection: {addr}")
        while True:
            client_data = connection.recv(self.data_size) # recieve in the data size from the socket and return it as the client data.
            if not client_data:
                break
            print(f"{client_data} {addr}")
            connection.sendall(client_data)
        connection.close()
    
    def monitor_con_packet(self):
        pass
    
    def is_con_query(self, query: str):
        pass




if __name__ == "__main__":
    s = Server()

    
    



