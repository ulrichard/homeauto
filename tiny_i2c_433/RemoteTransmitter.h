/*
 * RemoteSwitch library v2.2.1 (20120314) made by Randy Simons http://randysimons.nl/
 * Stripped down and converted to C by Richard Ulrich
 *
 * License: GPLv3. See license.txt
 */

#ifndef RemoteTransmitter_h
#define RemoteTransmitter_h

#include <stdbool.h>


typedef struct
{
    unsigned short _pin;		// Transmitter output pin
    unsigned int _periodusec;	// Oscillator period in microseconds
    unsigned short _repeats;	// Number over repetitions of one telegram
} RemoteTransmitter;

/**
* ElroTransmitter simulates remotes of the Elro "Home Control" series
* see http://www.elro.eu/en/m/products/category/home_automation/home_control
* There are are many similar systems on the market. If your remote has setting for 5 address bits, and can control
* 4 devices on or off, then you can try to use the ElroTransmitter. Otherwise you may have luck with the
* ActionTransmitter, which is similar.
*/

/**
* Constructor
*
* @param pin		output pin on Arduino to which the transmitter is connected
* @param periodsec	Duration of one period, in microseconds. Default is 320usec
* @param repeats	[0..7] The 2log-Number of times the signal is repeated.
* @see RemoteSwitch
*/
void InitElroTransmitter(RemoteTransmitter* rt, unsigned short pin);

/**
* Send a on or off signal to a device.
*
* @param systemCode	5-bit addres (dip switches in remote). Range [0..31]
* @param device	Device to switch. Range: [A..D] (case sensitive!)
* @param on	True, to switch on. False to switch off,
*/
void ElroSendSignal(const RemoteTransmitter* rt, unsigned short systemCode, char device, bool on);

/**
* Generates the telegram (data) which can be used for RemoteSwitch::sendTelegram.
* See sendSignal for details on the parameters
*
* @return Encoded data, including repeats and period duration.
*/
unsigned long ElroGetTelegram(const RemoteTransmitter* rt, unsigned short systemCode, char device, bool on);


#endif
