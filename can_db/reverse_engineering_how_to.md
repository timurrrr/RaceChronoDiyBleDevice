# HOW TO REVERSE ENGINEER YOR CAR CAN-BUS

## REQUIRED KNOWLEDGE

### Something about CAN-Bus
First thing you need to know is what it actually is the CAN-bus and how it does work. I suggest you to read [this article form Madox](http://www.madox.net/blog/2008/11/17/reverse-engineering-the-mazda-can-bus-part-1/)
Practically, you need to know that there are 2 CAN-bus main set of signals, the HS CAN-Bus and the MS CAN-Bus, and for each of these you need to know the baud rate, meaning number of data transmitted per second. In Mazda's case the baud rate for HS is 500000, MS is 125000. HS is usually where the most important data are communicated.
You also need to know that information on CAN-Bus is available in form of "messages" with an ID and a number of bytes (up to 8) conaining the actual data.

Bytes are 8 bits long and what you get is usually the hexadecimal value of each one of the 8 bytes. The usable value is then obtained combining one or more bytes and passing through a transfer function. Which byte, how many bytes and the transfer equation are all unknown.

Also you need to know where to access the CAN-Bus. Most obvious point is the OBD socket. I suggest to use this point of access for the reverse engineering, since it is easilly accessible and the position is well documented over the internet.

### Something about iron soldering
In order to follow this "how to" you may need to use to perform few iron solderings. It is not a big deal, YouTube can help a lot. Just a hint: if you are not great at iron soldering you may want to use wire containing lead, i.e. 60% tin, 40% lead. This kind of wire is not available everywhere, since it is been banned due to environmental concerns. Without lead you need to go up with temperature and the process is not as smooth as with lead, results may be not pretty and in general the experience can be frustrating.

### Something about Raspberry Pi and PiCan
In order to read the CAN-Bus of your car you need a device that can acquire current signals, process and interpret to bytes and then feed to a PC for human reading, logging etc.
Ther are many ways of doing this, basically you need a MCP2551 based board for the acquiring-processing part and a way to connect this board to a PC.

I've chosen to use Rasperry Pi and a PiCan2 hat due to:
-   Cost of the hardware (I already had a Pi 3B)
-   Ease of assembly
-   Availabilty of CAN tools for Linux
-   Community support

In any case, whatever will be your choice of hardware, you will need to know your way around a terminal screen, how to install and use command line tools, how to edit configurations file etc.

## HARDWARE
* Raspberry Pi 2 or 3 (I've used 3)
* PiCan 2. [I suggest to use the SMPS version](http://skpang.co.uk/catalog/pican2-canbus-board-for-raspberry-pi-23-with-smps-p-1476.html), so you can draw power directly from OBD socket. In this case you may need to run the Pi as headless system or to provide current to the monitor if you plan to use an external monitor.
* Optionally: an OBD connector with exposed color coded cables and known PIN to cable association. Avoid OBD to DB9 connectors cable, unless your sure the pins are all connectected in the right way, also if you use the screw connector you don't have to solder the bridges on the board.
* Optionally: a monitor, I had an official Raspberry 7" touch screen, I was able to run everything with only 12v from OBD socket, even if with a "low voltage" warning

Follow [the official instruction](http://skpang.co.uk/catalog/images/raspberrypi/pi_2/PICAN2SMPSUG13.pdf) and solder a 3 pin header connectoris to the 5v - GND if you plan to power devices from there, otherwise the powering of the Pi itself is done via the GPIO connections. You may need to solder 2 pins to use the jumper for the 120 Ω terminator. In my tests the terminator is not needed when you connect to the OBD port.

You then need to connect pins from OBD socket to the PiCan2 screw connector. You may do this via single wires or via OBD connector with exposed cables, just follow this pinout diagram:
                                  
![OBD pinout](https://upload.wikimedia.org/wikipedia/commons/c/c7/OBD_connector_shape.svg)

this showed here is the front view of the female connector, that is a perfect
match for the back of a male connector.
* Pin 4 and 5 (grey) are chassis ground and signal ground.
* Pin 6 (green, top) is CAN High
* Pin 14 (green, bottom) is CAN Low
* Pin 16 (red) is 12V power from battery

This is the connection scheme

| OBD Pin | PiCan2 Screw Connector label |
| ------ | --------- |
| 4 and 5 | GND |
| 6 | CAN_H |
| 14 | CAN_L |
| 16 | +12V |

Just remember that there is always power at pin 16 on OBD, even if the car is shut down with no key in. 



## SOFTWARE



## PROCEDURE
