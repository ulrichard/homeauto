// Ethernet slave for the home simple automation

#include "IR.h"
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[]  = { 192, 168, 2, 9 };


static const uint8_t PIN_status_LED = 13;

EthernetServer server(80);

void setup()
{
	Ethernet.begin(mac, ip);
	server.begin();


	IR::initialise(0); // IR receiver hardware is on pin2.


	// blink to show we're alive
	pinMode(PIN_status_LED, OUTPUT);
	digitalWrite(PIN_status_LED, HIGH);
	delay(500);
	digitalWrite(PIN_status_LED, LOW);
	delay(500);
	digitalWrite(PIN_status_LED, HIGH);
	delay(500);
	digitalWrite(PIN_status_LED, LOW);
}

void loop()
{
	if(!IR::queueIsEmpty())
	{
		digitalWrite(PIN_status_LED, HIGH);
		Serial.println("____");

		IR_COMMAND_TYPE code;
		while(IR::queueRead(code))
		{
			Serial.println(code, DEC);
		}
		digitalWrite(PIN_status_LED, LOW);
	}
}

