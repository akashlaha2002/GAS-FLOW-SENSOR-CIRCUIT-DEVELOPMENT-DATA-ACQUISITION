import serial
import time
from serial.tools import list_ports

with open('data_2.dat', 'rb') as f:
    while frame := f.read(8):
        print('Raw bytes:', frame)
        print('Hex:', frame.hex())
