#! /usr/bin/python
# script to send commands to the power wall plugs over 433mhz
# this script communicates over i2c with the atiny that is connected to the 433mhz transmitter

import smbus, time

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
	rf = rf433mhz(0x11, 21, 1) # bus is 0 on the alix, and 1 on the raspberrypi

	for i in range(3):
		rf.switchOn('B')
		time.sleep(2.0)
		rf.switchOff('B')
		time.sleep(2.0)


