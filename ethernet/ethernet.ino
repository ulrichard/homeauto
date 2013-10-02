// Ethernet slave for the home simple automation

#include "IR.h"
#include <EncEthernet.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// the mac and ip of the Arduino Ethernet shield
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[]  = {192, 168, 2, 9};

// Enter the IP address of the web server
byte ulrichardIp[] = {192, 168, 2, 7}; // ulrichard.ch

//                      +-\/-+
//              RST -> 1|    |28
//         uart RXD -> 2|    |27
//         uart TXD <- 3|    |26
//  ENC28J60 INT -- D2 4|    |25
//      LED red  <- D3 5|    |24
//                     6|    |23
//                VCC  7|    |22 GND
//                GND  8|    |21
//            crystal  9|    |20 VCC
//            crystal 10|    |19 D13 -> ENC28J60 SCK
//                    11|    |18 D12 -> ENC28J60 SO
//                    12|    |17 D11 -> ENC28J60 SI
//                    13|    |16 D10 -> ENC28J60 CS
//                    14|    |15  
//                      +----+                   

static const uint8_t PIN_status_LED = 3;

EncServer server(80); // Port 80 is http
EncClient ulrichardClient(ulrichardIp, 80);


void setup()
{
	Serial.begin(115200);

	EncEthernet.begin(mac, ip);
	server.begin();


	IR::initialise(0); // IR receiver hardware is on pin2.

	pinMode(4, OUTPUT);
	digitalWrite(4, LOW);
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
	// Check if we received some IR code
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

	// Check if we received something over ethernet
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

/*
	// Check if the web server is reachable
	if(ulrichardClient.connect())
	{
		digitalWrite(PIN_status_LED, LOW);
//		Serial.println("ulrichard.ch reachable");
	}
	else
	{
		digitalWrite(PIN_status_LED, HIGH);
//		Serial.println("no connection to ulrichard.ch");
	}
	ulrichardClient.stop();
*/
}

