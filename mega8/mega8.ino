// This program is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.

// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU General Public License for more details.

// This program will serve as an interface between the
// the i2c bus and a simple 433MHz transmitter.
// It exposes commands to switch wall plugs
// Created by Richard Ulrich <richi@paraeasy.ch>
// Inspired by :


// ATMEL AtMega8
//                       +-\/-+
//                      1|    |8  Vcc
//                      2|    |7 <-  I2C_SCL
//              test -> 3|    |6  -> 433MHz transmitter
//               GND    4|    |5 <-> I2C_SDA
//                       +----+

#include <inttypes.h>
#include "usiTwiSlave.h"
#include "RemoteTransmitter.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

uint8_t  EEMEM i2cSlaveAddr = 0x11; // Default address

int main(void)
{
	_delay_ms(100); // give the master some time to grab the i2c bus
	sei(); 			// enable interrupts
	if(eeprom_read_byte(&i2cSlaveAddr) < 7 || eeprom_read_byte(&i2cSlaveAddr) > 77)
		eeprom_write_byte(&i2cSlaveAddr, 0x11);
	usiTwiSlaveInit(eeprom_read_byte(&i2cSlaveAddr)); // initialize i2c

	// Set Port B pin 1 as output
	DDRB = (1 << PB1);

	// Intantiate a new Elro remote, also use pin 11 (same transmitter!)
	RemoteTransmitter elroTransmitter;
	InitElroTransmitter(&elroTransmitter, 1 << PB1);

	uint8_t plugAddress = 21;
	uint8_t plugDevice = 'B';

	// the main loop
	while(1)
	{
		// check the test pin
		if(PINB & (1 << PB4))
		{
			ElroSendSignal(&elroTransmitter, plugAddress, 'B', true);
			_delay_ms(1000);
			ElroSendSignal(&elroTransmitter, plugAddress, 'B', false);
			_delay_ms(1000);
		}
	
		// I2C
		if(usiTwiDataInReceiveBuffer())
		{
			const uint8_t cmd = usiTwiReceiveByte();

			switch(cmd)
			{
				case 0xA1: // change the i2c address (sends 1 byte)
				{
					uint8_t recv = usiTwiReceiveByte();
					if(recv < 7 || recv > 77)
						continue;
					eeprom_write_byte(&i2cSlaveAddr, recv);
					usiTwiSlaveInit(recv); // initialize i2c
					break;
				}
				case 0xAA: // set address for switching
					plugAddress = usiTwiReceiveByte();
					break;
				case 0xB0: // switch off
					plugDevice = usiTwiReceiveByte();
                    ElroSendSignal(&elroTransmitter, plugAddress, plugDevice, false);
					break;
				case 0xB1: // switch on
					plugDevice = usiTwiReceiveByte();
					ElroSendSignal(&elroTransmitter, plugAddress, plugDevice, true);
					break;
			}; // switch
			continue;
		}

		_delay_ms(10);
	}

	return 1;
}
