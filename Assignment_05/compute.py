import socket
import threading
import time

HOST = "localhost"
PORT = 55555
sock = socket.socket
data = "Empty"

COMPUTE_PACKET_SIZE = 256
#COMPUTE_QUERY_SIZE = 256
COMPUTE_PACKET_START = "<Compute begin>"
COMPUTE_PACKET_END = "<Computation Completed>"
REPORT_PACKET_START = "<Report Begin>"
REPORT_PACKET_END = "<Report Completed>" 
# ^ Globals for the packet 

def packet_create():

    pass

# Monitor for a signal
def monitor_socket():

    pass

def signal_handlers():

    pass

def server_connect(HOST, PORT, data: bytes, n: list):
    with sock(socket.AF_INET, socket.SOCK_STREAM) as stream:
        stream.connect((HOST, PORT))
        stream.sendall(f"Client Request : {data}") # send this data to the user.
        data = stream.recv(COMPUTE_PACKET_SIZE) # After data is recieved from the client
        
        n.append = data.find("Request") # Find 

# O(N)
def perfect_nums(a, b):
    perfect_num_list = []
    for i in range(a, b):
        sum = 0
        for num in range(1, i):
            if i % num == 0:
                sum += num
        if sum == i:
            perfect_num_list.append(i)
    return perfect_num_list

# Send the server the clock amount, 
def server_query():
    pass

if __name__ == "__main__":
    n = 0
    compute_thread.run
    perfect_nums(10)
    compute_thread = threading.Thread(perfect_nums, n)
    monitor_thread = threading.Thread(server_connect, (HOST, PORT, data))
    thread_list = [compute_thread, monitor_thread]
    # Add the compute and monitor threads into the thread list
    threading.Thread.join(*thread_list) 
    # Run all our threads. 
    