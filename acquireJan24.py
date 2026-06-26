import socket
import struct
import binascii
from array import array
import math
import time
import sys
import codecs

#localIP   = "10.203.0.200"
localIP   = "192.168.1.20"
localPort = 4134
Buffer    = 400
serverAddressPort   = ("192.168.1.10", 1622)

UDPsocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
UDPsocket.bind((localIP, localPort))

#cmdfile = open('resetCommand.txt', 'r')
##########Rest Trigger No. #############
MESSAGE1=b'MUCHLVaABCDEFGHIJKLMNOPQRSTV12345142567722ABCDf%\r' 
MESSAGE2=b'MUCHLVaABCDEFGHIJKLMNOPQRSTV12345142567722PQRSE%\r' 
#MESSAGE2=b'MUCHLV1ABCDEFGHIJKLMNOPQRSTUV%\r'     
##########Rest Trigger No. Ends #############
    
UDPsocket.sendto( MESSAGE1 , serverAddressPort)
time.sleep(0.01)
UDPsocket.sendto( MESSAGE2 , serverAddressPort)


readfile = open("data/runNo", 'r')
xx=1
xx=readfile.read()
print(xx)
yy = eval(xx)
yy = yy + 1
print(yy)
readfile.close()
readfile = open("data/runNo", 'w')
readfile.write(str(yy))
readfile.close()
datfile = "data/data"  + str(yy) + ".txt"
#f1 = open("data/data1.txt", "wb")
f1 = open(datfile, "wb")
packet = 0
marker = "packet"
while(True):
    f1.write(b'\xff\xff\xff\xff')
    data,address = UDPsocket.recvfrom(400)
    packet = packet + 1
    #print("Message : ", data)
    if(packet%10 == 0):
        print("Packet : ", packet, "  Run no.: ",yy)
    f1.write(data)
