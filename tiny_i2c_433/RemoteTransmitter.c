/*
 * RemoteSwitch library v2.2.1 (20120314) made by Randy Simons http://randysimons.nl/
 * See RemoteTransmitter.h for details.
 *
 * License: GPLv3. See license.txt
 */

#include "RemoteTransmitter.h"
#include <avr/io.h>
#include <util/delay.h>

/************
* RemoteTransmitter
************/
void sendCode(unsigned short pin, unsigned long code, unsigned int periodusec, unsigned short repeats);

unsigned long encodeTelegram(const RemoteTransmitter* rt, unsigned short trits[])
{
	unsigned long data = 0;

	// Encode data
	for(unsigned short i=0; i<12; i++)
	{
		data*=3;
		data+=trits[i];
	}

	// Encode period duration
	data |= (unsigned long)rt->_periodusec << 23;

	// Encode repeats
	data |= (unsigned long)rt->_repeats << 20;

	return data;
}

/**
* Format data:
* pppppppp|prrrdddd|dddddddd|dddddddd (32 bit)
* p = perioud (9 bit unsigned int
* r = repeats as 2log. Thus, if r = 3, then signal is sent 2^3=8 times
* d = data
*/
void doSendTelegram(unsigned long data, unsigned short pin)
{
	unsigned int periodusec = (unsigned long)data >> 23;
	unsigned short repeats = ((unsigned long)data >> 20) & 0x07;

	sendCode(pin, data, periodusec, repeats);
}

void sendTelegram(const RemoteTransmitter* rt, unsigned short trits[])
{
	doSendTelegram(encodeTelegram(rt, trits), rt->_pin);
}

void delayMicrosecond(unsigned long val)
{
	for(; val > 100; val -= 100)
		_delay_us(100);
	for(; val > 10; val -= 10)
		_delay_us(100);
	for(; val > 1; val -= 1)
		_delay_us(1);
}

void sendCode(unsigned short pin, unsigned long code, unsigned int periodusec, unsigned short repeats)
{
	code &= 0xfffff; // Truncate to 20 bit ;
	// Convert the base3-code to base4, to avoid lengthy calculations when transmitting.. Messes op timings.
	unsigned long dataBase4 = 0;

	for(unsigned short i=0; i<12; i++)
	{
		dataBase4<<=2;
		dataBase4|=(code%3);
		code/=3;
	}

	repeats = 1 << (repeats & 0x07); // repeats := 2^repeats;

	for(unsigned short int j=0; j<repeats; j++)
	{
		// Sent one telegram

		// Recycle code as working var to save memory
		code=dataBase4;
		for(unsigned short i=0; i<12; i++)
		{
			switch(code & 0x03)
			{
            case 0:
                PORTB |= pin;
                delayMicrosecond(periodusec);
                PORTB &= ~pin;
                delayMicrosecond(periodusec*3);
                PORTB |= pin;
                delayMicrosecond(periodusec);
                PORTB &= ~pin;
                delayMicrosecond(periodusec*3);
                break;
            case 1:
                PORTB |= pin;
                delayMicrosecond(periodusec*3);
                PORTB &= ~pin;
                delayMicrosecond(periodusec);
                PORTB |= pin;
                delayMicrosecond(periodusec*3);
                PORTB &= ~pin;
                delayMicrosecond(periodusec);
                break;
            case 2: // KA: X or float
                PORTB |= pin;
                delayMicrosecond(periodusec);
                PORTB &= ~pin;
                delayMicrosecond(periodusec*3);
                PORTB |= pin;
                delayMicrosecond(periodusec*3);
                PORTB &= ~pin;
                delayMicrosecond(periodusec);
                break;
			}
			// Next trit
			code>>=2;
		}

		// Send termination/synchronisation-signal. Total length: 32 periods
		PORTB |= pin;
		delayMicrosecond(periodusec);
		PORTB &= ~pin;
		delayMicrosecond(periodusec*31);
	}
}

bool isSameCode(unsigned long encodedTelegram, unsigned long receivedData)
{
	return (receivedData==(encodedTelegram & 0xFFFFF)); // ompare the 20 LSB's
}


/************
* ElroTransmitter
************/

void InitElroTransmitter(RemoteTransmitter* rt, unsigned short pin)
{
    rt->_pin        = pin;
    rt->_periodusec = 320;
    rt->_repeats    = 4;
}

void ElroSendSignal(const RemoteTransmitter* rt, unsigned short systemCode, char device, bool on)
{
	doSendTelegram(ElroGetTelegram(rt, systemCode, device, on), rt->_pin);
}

unsigned long ElroGetTelegram(const RemoteTransmitter* rt, unsigned short systemCode, char device, bool on)
{
	unsigned short trits[12];

	device-=65;

	for (unsigned short i=0; i<5; i++)
	{
		//trits 0-4 contain address (2^5=32 addresses)
		trits[i]=(systemCode & 1)?0:2;
		systemCode>>=1;

		//trits 5-9 contain device. Only one trit has value 0, others have 2 (float)!
		trits[i+5]=(i==device?0:2);
    }

	//switch on or off
	trits[10]=(on?0:2);
	trits[11]=(!on?0:2);

	return encodeTelegram(rt, trits);
}
