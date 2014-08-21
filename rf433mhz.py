#! /usr/bin/python
# script to send commands to the power wall plugs over 433mhz
# this script communicates over i2c with the atiny that is connected to the 433mhz transmitter

import smbus, time, argparse

class rf433mhz:
	def __init__(self, i2cSlaveAddr = 0x11, plugAddress = 21, i2cBusNbr = 1):
		self.i2cSlaveAddr = i2cSlaveAddr
		self.plugAddress = plugAddress
		self.i2c = smbus.SMBus(i2cBusNbr)
		# set the plugAddress
		self.i2c.write_byte(self.i2cSlaveAddr, 0xAA)
		self.i2c.write_byte(self.i2cSlaveAddr, self.plugAddress)

	def changeSlaveAddress(self, newAddr):
		self.i2c.write_byte(self.i2cSlaveAddr, 0xA1)
		self.i2c.write_byte(self.i2cSlaveAddr, newAddr)

	def switchOff(self, plugDevice): # plugDevice is one of : 'A' 'B' 'C' 'D'
		self.i2c.write_byte(self.i2cSlaveAddr, 0xB0)
		self.i2c.write_byte(self.i2cSlaveAddr, ord(plugDevice))

	def switchOn(self, plugDevice): # plugDevice is one of : 'A' 'B' 'C' 'D'
		self.i2c.write_byte(self.i2cSlaveAddr, 0xB1)
		self.i2c.write_byte(self.i2cSlaveAddr, ord(plugDevice))

	def __repr__(self):
		print "attiny interfacing to the 433mhz transmitter at i2c address %d" % self.i2cSlaveAddr



# Main program
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--bus', type=int, default=0, help='the i2c bus: 0 on the alix, 1 on the RaspberryPi')
    parser.add_argument('--addr', type=int, default=21, help='the address of the plug')
    parser.add_argument('--device', type=char, default='B', help='the device letter (A B C or D) of the plug')
    parser.add_argument('--on', type=bool, default=True, help='end on or off command')
    args = parser.parse_args()

    rf = rf433mhz(0x11, args.addr, args.bus)

    if args.on:
        rf.switchOn(args.device)
    else:
        rf.switchOff(args.device)


