import socket
import struct
import binascii
from array import array
import math
import time
import sys

localIP   = "192.168.1.20"
localPort = 4134
Buffer    = 400

UDPsocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
UDPsocket.bind((localIP, localPort))

f1 = open("data/data3.txt", "wb")

while(True):
    data,address = UDPsocket.recvfrom(400)
    print("Message : ", data)
    print("Address : ", address)
    length = len(data)
    data = data[2:length-1]
    print("Message : ", data)
    f1.write(data)
