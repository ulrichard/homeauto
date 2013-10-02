// Ethernet slave for the home simple automation

#include "IR.h"
#include <EncEthernet.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[]  = { 192, 168, 2, 9 };


static const uint8_t PIN_status_LED = 13;

EncServer server(80);

void setup()
{
	Serial.begin(115200);

	EncEthernet.begin(mac, ip);
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
			Serial.print("IR: ");
			Serial.println(code, DEC);
		}
		digitalWrite(PIN_status_LED, LOW);
	}

	EncClient client = server.available();
	if(client)
	{
		Serial.println("____");
		Serial.println("Web: ");

		// an http request ends with a blank line
		boolean current_line_is_blank = true;
		while(client.connected())
		{
			if(client.available())
			{
				char c = client.read();
				// if we've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so we can send a reply
				if(c == '\n' && current_line_is_blank)
				{
					// send a standard http response header
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println();

					// output the value of each analog input pin
					for(int i = 0; i < 6; i++)
					{
						client.print("analog input ");
						client.print(i);
						client.print(" is ");
						client.print(analogRead(i));
						client.println("");
					}
					break;
				}
				if(c == '\n')
				{
					// we're starting a new line
					current_line_is_blank = true;

					Serial.println("");
				}
				else if(c != '\r')
				{
					// we've gotten a character on the current line
					current_line_is_blank = false;

					Serial.print(c);
				}
			}
		}
		// give the web browser time to receive the data
		delay(1);
		client.stop();
	}
}

