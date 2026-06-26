import serial
import time
from serial.tools import list_ports

   # Configure the serial port. Replace with your actual port and settings.
   
def find_arty7_port():
    ports = list_ports.comports()
    for port in ports:
        # Customize this based on your device description or manufacturer
        if 'Digilent' in port.description or 'Arty' in port.description:
            return port.device
    return None
SERIAL_PORT = find_arty7_port()


ser = serial.Serial(
      port=SERIAL_PORT,  # Example: '/dev/ttyACM0' on Linux, 'COM3' on Windows
      baudrate= 921600,
      timeout=1)

def send_data(data):
       ser.write(data.encode())  # Encode the string to bytes before sending
       print(f"Sent: {data}")

try:
     while True:
           message = input("Enter data to send: ")
           send_data(message)
           time.sleep(0.1)  # Optional delay
except KeyboardInterrupt:
       print("Exiting")
finally:
       ser.close() # Close the port when done
