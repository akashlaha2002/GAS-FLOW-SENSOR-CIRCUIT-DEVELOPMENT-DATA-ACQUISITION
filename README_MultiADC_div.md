1. Summary:
   
 **Overview**

The MultiADC\_Div VHDL project is designed to collect real-time data from two independent external ADCs interfaced with the Arty A7-35T FPGA board. Each ADC acquires data from a different analog source. The digitized signals are processed by the FPGA and then streamed to a host PC for real-time data visualization. The project implements VHDL-based control and communication logic, and uses UART for data transmission.

**Objective**

* Interface two independent ADC modules with the FPGA using SPI protocol.
* Implement a FIFO buffer to store acquired samples.
* Control data flow using UART and AXI4 stream interfaces.
* Stream real-time data from the FPGA to a PC monitor for acquisition analysis.

 **FPGA Target**

* Board: Digilent Arty A7-35T
* Family: Xilinx Artix-7
* Tool Used: Xilinx Vivado (simulation and synthesis)





 2. Block diagram of data flow:



  +-----------------+     SPI    +-----------------+
  | ADC Read Block 1| <--------> |  External ADC 1 |
  +-----------------+            +-----------------+
         |
         v
  +------------------+     SPI     +-----------------+
  | ADC Read Block 2 | <-------->  |  External ADC 2 |
  +------------------+             +-----------------+
         |   
         v
   +------------------+     +-------------------+
   | ADC Process Ctrl | --> |  GENERIC FIFO     |
   +------------------+     +-------------------+
                                    |
                                    v
                             +--------------+
                             | UART Module  |
                             +--------------+
                                    |
                                    v
                           +------------------+
                           |    PC Monitor    |
                           +------------------+

=> ADC Read Modules fetch data via SPI from ADC1 and ADC2. Data from each ADC is pushed into a generic FIFO. A UART interface reads FIFO data and sends it to a PC. The Top Module synchronizes all submodules.





3. Architecture:



**TOP Module**: this module coordinates the full design. It: Instantiates the UartCommand module for PC communication, Interfaces with ADC drivers, Controls read/write operations for continuous streaming.

1. *ADC Control (Control\_process):* This process alternate between ADC1 and ADC2, reading from one ADC at a time:

		When data\_ready is high, it checks which ADC is active using stateActive.

		Captures DataTime, DeviceID, DataType, and TransmitData.

		Asserts readActive to trigger transmission.

2. *Deglitching Inputs (deglitch):*

	Synchronizes asynchronous inputs like reset and UART RX to the system clock to 	prevent metastability.

3. *Timestamp Generator (timestampProcess):*

	*Generates a time counter (TimeCounter) using a prescaler (ReduceSpeed) to scale down the 100 MHz clock to 1 Hz. The counter is used to timestamp ADC data.*

4. *Receiving Process (ReceiveProcess): *Handles USB UART RX data:*

		Decodes commands (x"31", x"32", x"33").

		Routes data to transmit FIFOs for each path.

		Dual channel support with toggling signals (sendLogic, sendLogic1).

5. *Transmit Process (transmitProcess): Sequentially transmits the data frame through UART:*

		Waits for readActive to start.

		Sends fields like DataType, DataTime, DeviceID, TransmitData byte-by-byte.

		Ends with a newline character.


**UartCommand Module:** This module instantiates the uart and GENERIC\_FIFO modules and create a loopback process: 

	Reset : Clears all control and data signals on reset.

	UART Receive → FIFO Store: If uart\_data\_out\_stb = '1' and FIFO is not full, the received UART byte is stored in FIFO.

	UART Transmit Acknowledge: Clears the uart\_data\_in\_stb flag when transmission is acknowledged (uart\_data\_in\_ack = '1').

	FIFO Read → UART Send: If FIFO is not empty and UART is ready, a byte is read from FIFO and passed to UART for transmission.

**FIFO Module:** 

	1. Reset: On reset, write\_pointer and read\_pointer are set to 0, fifo\_memory is cleared, fifo\_empty is '1', and fifo\_full is '0'.

	2. Write Operation: When write\_en = '1' and the FIFO is not full, data is written to the memory at write\_pointer, and write\_pointer advances. If write\_pointer reaches FIFO				    \_DEPTH - 1, it wraps around to 0.

	3. Read Operation: When read\_en = '1' and the FIFO is not empty, read\_pointer advances, and the data at read\_pointer is output via read\_data. If read\_pointer reaches FIFO				   \_DEPTH - 1, it wraps around to 0.

	4. Status Updates: The FIFO\_FLAGS process continuously monitors write\_pointer and read\_pointer to update full, empty, and level. The FIFO is considered full when level = FIFO			   \_DEPTH - 1 to avoid ambiguity with the empty state (both pointers equal).

**ADC module:** The adc\_read module implements a serial communication protocol to read 16-bit data from an ADC:

	• Idle State: When startConv = '0', the module waits with cs = '1', data\_ready = '0', and count = 16.		

	• Start Conversion: When startConv = '1', cs\_int is set to '0', activating the ADC. The module counts down from 16 to 0 over 16 clock cycles, shifting in one bit of sdata per 			    falling edge into shift\_reg.

	• Data Completion: When count reaches -1, the 16-bit data is complete in shift\_reg. cs\_int is set to '1', data\_ready is set to '1', and out\_data is assigned shift\_reg. count 			   is set to 17 to allow a brief pause before the next conversion.

	• Reset: A reset clears all signals and returns the module to the idle state.




 4. PMOD\\PIN Configuration:



* **Pmod Header JA**

*Signal Name	Pin	Description*

sclk2		D13	Serial Clock 2

sdata2		B18 	Serial Data 2

cs2		A18 	Chip Select 2

* **Pmod Header JB**

*Signal Name	Pin	Description*

tx\_1		E15	UART Transmit 1

rx\_1		E16	UART Receive 1

rx\_2		J17	UART Receive 2

tx\_2		J18	UART Transmit 2

* **Pmod Header JC**

*Signal Name	Pin	Description*

sclk1		U14	Serial Clock 1

sdata1		V14	Serial Data 1

cs1		T13	Chip Select 1

* **USB-UART Interface (Onboard)**

*Signal Name	Pin	Description*

usb\_rs232\_txd	D10	Transmit from FPGA to PC

usb\_rs232\_rxd	A9	Receive from PC to FPGA



 5. Reference 

Digilent Arty A7-35T Reference Manual



Xilinx Vivado Design Suite Documentation



UART and SPI Communication Protocols



VHDL 2008 Language Reference



