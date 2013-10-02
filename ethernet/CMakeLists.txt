#SET(atmega8.build.f_cpu 8000000) # running on internal 8MHz RC oscillator
#SET(atmega8.build.f_cpu 16000000) # running on external 16MHz crystal

set(CMAKE_TOOLCHAIN_FILE ../../arduino-cmake/cmake/ArduinoToolchain.cmake)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(ethernet)

#print_board_list()

SET(AVR_DEVICE     atmega328)
#SET(AVRDUDE_DEVICE ${AVR_DEVICE})
SET(AVRDUDE_DEVICE atmega328p)
#SET(AVRDUDE_PORT   /dev/ttyACM0)   # diamex AVR von www.reichelt.de
SET(AVRDUDE_PORT   /dev/ttyUSB0)
#SET(AVRDUDE_PROGRAMMER   stk500v2) # diamex AVR von www.reichelt.de
#SET(AVRDUDE_PROGRAMMER   USBasp)   # www.fischl.de
SET(AVRDUDE_PROGRAMMER    arduino)  # Arduino

INCLUDE_DIRECTORIES(/usr/share/arduino/libraries/SPI)
INCLUDE_DIRECTORIES(ENC28J60)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_DEFINITIONS(-D __PROG_TYPES_COMPAT__) # for old pgm_char

generate_arduino_firmware(ethernet
    SKETCH ../ethernet
	SRCS	ethernet.ino 
		IR.cpp
		ENC28J60/EncEthernet.cpp
		ENC28J60/EncClient.cpp
		ENC28J60/EncServer.cpp
		ENC28J60/utility/socket.c
		ENC28J60/utility/enc28j60.c
		ENC28J60/utility/ip_arp_udp_tcp.c
	PROGRAMMER stk500v2
    PORT  ${AVRDUDE_PORT}
    BOARD ${AVR_DEVICE}
)

IF(${AVRDUDE_PROGRAMMER} STREQUAL "arduino")
	ADD_CUSTOM_TARGET(flash
				avrdude -P ${AVRDUDE_PORT} -p ${AVRDUDE_DEVICE} -c ${AVRDUDE_PROGRAMMER} -b 57600 -F -u -Uflash:w:ethernet.hex
		DEPENDS ethernet
	)
ELSE(AVRDUDE_PROGRAMMER == "arduino")
	ADD_CUSTOM_TARGET(flash
				avrdude -P ${AVRDUDE_PORT} -p ${AVRDUDE_DEVICE} -c ${AVRDUDE_PROGRAMMER} -Uflash:w:ethernet.hex
		DEPENDS ethernet
	)
ENDIF(AVRDUDE_PROGRAMMER == "arduino")

# for fuse settings see http://www.engbedded.com/fusecalc/
SET(LFUSE 0xbf) # ext crystal
#SET(LFUSE 0xa4) # internal RC osc 8Mhz -> bricked
SET(HFUSE 0xd9)
ADD_CUSTOM_TARGET(fuses
			avrdude -P ${AVRDUDE_PORT} -p ${AVRDUDE_DEVICE} -c stk500v2 -U lfuse:w:${LFUSE}:m -Uhfuse:w:${HFUSE}:m
)

ADD_CUSTOM_TARGET(readchip
			avrdude -P ${AVRDUDE_PORT} -p ${AVRDUDE_DEVICE} -c stk500v2 -U flash:r:dumpflash.hex:i -U eeprom:r:dumpee.hex:i -U lfuse:r:dumplfuse.hex:i -U hfuse:r:dumphfuse.hex:i
)


