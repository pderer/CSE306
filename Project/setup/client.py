# Send your source and Compile on the grading system !
import sys
import os
import socket

sock = socket.socket()
host = '10.150.21.56'
port = 12399

def to_bytes(num):
    result = bytearray()
    for i in range(4):
        result.append(num & 0xFF)
        num = num >> 8
    return result

# Usage
if len(sys.argv) != 2:
    print("Usage: python client.py YOUR_PROJ_FILENAME.C")
    sys.exit()

filename = sys.argv[1]

# Check if file exists
if not os.path.exists(filename):
    print("no such file existed")
    sys.exit()

# Connecting
print('Connecting to the server')
try:
    sock.connect((host, port))
except socket.error as e:
    print(str(e))
    sys.exit()

# Send the file
print('Sending the file')

filesize = os.path.getsize(filename)
sock.send(to_bytes(filesize))
transferred  = 0
with open(filename, 'rb') as f:
    try:
        data = f.read(1024)
        while data:
            transferred += sock.send(data)
            data = f.read(1024)
    except Exception as ex:
        print(ex)
    print("Transfer completed: %d Bytes" % transferred)

# Print the results
print('Waiting for the result. It may take a while.')
response = sock.recv(4096)
print(response.decode('utf-8')) # Count
response = sock.recv(4096)
print(response.decode('utf-8')) # Compiler message
sock.close()
