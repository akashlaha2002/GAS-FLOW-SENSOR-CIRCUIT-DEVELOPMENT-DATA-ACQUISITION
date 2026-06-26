import serial
import time
from serial.tools import list_ports
import os
import struct
import binascii
from array import array
import math
import time
import sys


# -------- Create a new binary file --------
base_name = "data"
extension = ".dat"  # Use .bin to reflect binary content
i = 1
while os.path.exists("data/"+f"{base_name}_{i}{extension}"):
    i += 1
filename = "data/" + f"{base_name}_{i}{extension}"

# -------- Find the Arty7 serial port --------
def find_arty7_port():
    ports = list_ports.comports()
    for port in ports:
        if 'Digilent' in port.description or 'Arty' in port.description:
            return port.device
    return None

BAUD_RATE = 921600
SERIAL_PORT = find_arty7_port()

if SERIAL_PORT is None:
    print("No Arty7 FPGA serial device found.")
else:
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud.")
        print(f"Writing raw binary data to file: {filename}")

        with open(filename, "wb") as f:
            while True:
                if ser.in_waiting > 0:
                    data = ser.read(ser.in_waiting)  
                    f.write(data)                   
                    f.flush()                       
                    print("Received:", data)
                else:
                    time.sleep(0.01)

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except KeyboardInterrupt:
        print("Stopped by user.")

