SET(atmega8.build.f_cpu 12000000)

set(CMAKE_TOOLCHAIN_FILE ../../arduino-cmake/cmake/ArduinoToolchain.cmake)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(mega8_i2c_433)

SET(AVR_DEVICE     atmega8)
SET(AVRDUDE_PORT   /dev/ttyACM0)

generate_arduino_firmware(mega8_i2c_433
	SKETCH ../mega8
	SRCS mega8.ino RemoteTransmitter.cpp #usiTwiSlave.c
	PROGRAMMER stk500v2
    PORT  ${AVRDUDE_PORT}
    BOARD ${AVR_DEVICE}
)

ADD_CUSTOM_TARGET(flash
			avrdude -P ${AVRDUDE_PORT} -p ${AVR_DEVICE} -c stk500v2 -U flash:w:mega8_i2c_433.hex
	DEPENDS mega8_i2c_433
)

# for fuse settings see http://www.engbedded.com/fusecalc/
SET(LFUSE 0xbf)
SET(HFUSE 0xd9)
ADD_CUSTOM_TARGET(fuses
			avrdude -P ${AVRDUDE_PORT} -p ${AVR_DEVICE} -c stk500v2 -U lfuse:w:${LFUSE}:m -Uhfuse:w:${HFUSE}:m
)





