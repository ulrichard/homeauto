// This program is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE.  See the GNU General Public License for more details.
// Created by Richard Ulrich <richi@paraeasy.ch>
// Ethernet slave for the home simple automation
// With a Nokia display to display the state of the bitcoin miner

#include "IR.h"
#include "nokia3310lcd.h"
#include <EncEthernet.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

//  AtMega 328
//                      +-\/-+
//              RST -> 1|    |28 <-  SCL  -------------- C  I
//         uart RXD -> 2|    |27 <-> SDA  -------------- D  2
//         uart TXD <- 3|    |26                   +---- V  C
//  ENC28J60 INT -- D2 4|    |25                   |  +- G 
//      LED red  <- D3 5|    |24                   |  |   nokia 5110 LCD
//                  D4 6|    |23 -> backlight --+  |  |  +--------------+
//                VCC  7|    |22  GND ----------|--|--+--|        GND ->|
//                GND  8|    |21                +--|-----|  backlight ->|
//            crystal  9|    |20  VCC     ---------+-----|        VCC ->|
//            crystal 10|    |19 D13 -> SCK--------------|        SCK ->|  x-> ENC28J60 SCK
//                    11|    |18 D12 <- MISO  +----------|       MOSI ->|  x-> ENC28J60 SO
//                    12|    |17 D11 -> MOSI -+     +----|   cmd/data ->|  x-> ENC28J60 SI
// LCD chip select <- 13|    |16 D10 <- SS          | +--|chip enable ->|  x-> ENC28J60 CS
// LCD reset  |   +-<-14|    |15 D9 -> LCD cmd/data-+ |+-|      reset ->|
//            |   |     +----+                        || +--------------+                  
//            +---|-----------------------------------+|
//                +------------------------------------+  
//
// ENC28J60 Ethernet adapter -> connections from top:
// http://www.geeetech.com/wiki/index.php/Arduino_ENC28J60_Ethernet_Module
// 
// 	  CLK out  |  INT
//	      SOL  |  SO
//         SI  |  SCK
//         CS  |  reset
//        VCC  |  GND

// the mac and ip of the Arduino Ethernet shield
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte ip[]  = {192, 168, 2, 9};

// Enter the IP address of the web server
byte ulrichardIp[] = {192, 168, 2, 7}; // ulrichard.ch

// pin assignments
static const uint8_t PIN_status_LED = 3;
static const uint8_t PIN_IR_LED     = 9; // OC1A -> infrared transmitter LED.
static const uint8_t LCD_BACKLIGHT  = A0;

EncServer server(80); // Port 80 is http
EncClient ulrichardClient(ulrichardIp, 9332);
Nokia3310LCD  disp(9, 8, 7);

char linebuf[128];
uint16_t linepos;

void setup()
{
	Serial.begin(115200);

	EncEthernet.begin(mac, ip);
	server.begin();

	pinMode(LCD_BACKLIGHT, OUTPUT);
    digitalWrite(LCD_BACKLIGHT, LOW);

//	SPI.begin();
//	SPI.setClockDivider(SPI_CLOCK_DIV64); // 250 kHz

	disp.init();
    disp.LcdContrast(0x40);
	disp.LcdClear();
	disp.LcdGotoXYFont(1, 1);
	disp.LcdStr(Nokia3310LCD::FONT_1X, "hello world");
	disp.LcdUpdate();

//	IR::initialise(0); // IR receiver hardware is on pin2. -> conflicting with ethernet

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
/* -> pin conflicting with ethernet
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
*/

/* first testing the client part
	// Check if we received something over ethernet
	EncClient client = server.available();
	if(client)
	{
//		Serial.println("____");
//		Serial.println("Web: ");

		// an http request ends with a blank line
		boolean current_line_is_blank = true;
		while(client.connected())
		{
			disp.LcdClear();
			uint8_t x = 1, y = 1;

			if(client.available())
			{
				const char c = client.read();
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

//					Serial.println("");

					x = 1;
					++y;
				}
				else if(c != '\r')
				{
					// we've gotten a character on the current line
					current_line_is_blank = false;

//					Serial.print(c);
					disp.LcdGotoXYFont(x, y);
					disp.LcdChr(Nokia3310LCD::FONT_1X, c);
				}
			}
			disp.LcdUpdate();
		}
		// give the web browser time to receive the data
		delay(1);
		client.stop();
	}
*/

	// Check the state of the bitcoin miner
	// https://en.bitcoin.it/wiki/P2Pool#Web_interface
	linepos = 0;
	bool foundPrev = false, foundColon = false;
	uint32_t hashrate = 0;
	while(ulrichardClient.connected())
	{
		digitalWrite(PIN_status_LED, LOW);

		if(!ulrichardClient.available())
			continue;
		const char c = ulrichardClient.read();

		if('\n' == c)
		{
			linebuf[linepos++] = '\0';

			if(0 == hashrate)
			{
				disp.LcdGotoXYFont(1, 1);
				disp.LcdStr(Nokia3310LCD::FONT_1X, linebuf);
				disp.LcdUpdate();
			}

			if(foundColon)
			{
				hashrate = atoi(linebuf);         // in H/s
				hashrate /= (1024 * 1024 * 1024); // GH/s

				sprintf(linebuf, "%d GH/s", hashrate);

				disp.LcdGotoXYFont(1, 1);
				disp.LcdStr(Nokia3310LCD::FONT_1X, linebuf);
				disp.LcdUpdate();

				foundColon = foundPrev = false;
			}
			else if(strncmp("miner_hash_rates", linebuf, 16))
				foundPrev = true;


			linepos = 0;
		}
		else if('\r' == c)
			continue;
		else if(0 == linepos && (' ' == c || '*' == c || '\"' == c))
			continue; // trim left
		else if(foundPrev && !foundColon)
		{
			if(':' == c)
				foundColon = true;
			continue;
		}
		else if(linepos + 2 < sizeof(linebuf))
			linebuf[linepos++] = c;
	}
	ulrichardClient.stop();
	delay(10);
		 
	if(ulrichardClient.connect())
	{
		digitalWrite(PIN_status_LED, LOW);
//		Serial.println("ulrichard.ch reachable");

		ulrichardClient.println("GET /local_stats HTTP/1.1");
		ulrichardClient.println("Host: 192.168.2.7:9332");
		ulrichardClient.println();

		
	}
	else
	{
		digitalWrite(PIN_status_LED, HIGH);
//		Serial.println("no connection to ulrichard.ch");

		disp.LcdClear();
		disp.LcdGotoXYFont(1, 1);
		disp.LcdStr(Nokia3310LCD::FONT_1X, "could not");
		disp.LcdGotoXYFont(1, 2);
		disp.LcdStr(Nokia3310LCD::FONT_1X, "connect to");
		disp.LcdGotoXYFont(1, 3);
		disp.LcdStr(Nokia3310LCD::FONT_1X, "ulrichard.ch:9332");
		disp.LcdUpdate();
	}
	

}

