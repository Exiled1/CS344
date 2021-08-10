import socket
import threading
import time

HOST = "localhost"
PORT = 55555
sock = socket.socket
data = "Empty"




def server_connect(HOST, PORT, data: bytes, n: list):
    with sock(socket.AF_INET, socket.SOCK_STREAM) as stream:
        stream.connect((HOST, PORT))
        stream.sendall(f"<Compute req> : {data}")
        data = stream.recv(1024)
        n.append = data.find("Request") # 

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

if __name__ == "__main__":
    n = 0
    compute_thread.run
    perfect_nums(10)
    compute_thread = threading.Thread(perfect_nums, n)
    monitor_thread = threading.Thread(server_connect, (HOST, PORT, data))