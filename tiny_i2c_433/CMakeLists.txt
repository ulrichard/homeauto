

set(CMAKE_TOOLCHAIN_FILE avr_crosscompile.cmake)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(tiny_i2c_433)

INCLUDE(avr_crosscompile.cmake)
IF(NOT CMAKE_CROSSCOMPILING)
#	ERROR()
ENDIF()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(tiny_i2c_433.elf
	RemoteSwitch.c
	RemoteTransmitter.c
	usiTwiSlave.c
)

ADD_CUSTOM_COMMAND(TARGET tiny_i2c_433.elf POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom tiny_i2c_433.elf tiny_i2c_433.hex
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# for fuse settings see http://www.engbedded.com/fusecalc/
SET(LFUSE 0xe1)
SET(HFUSE 0xdd)
SET(EFUSE 0xff)

ADD_CUSTOM_TARGET(flash
			avrdude -P ${AVRDUDE_PORT} -p ${AVRDUDE_DEVICE} -c stk500v2 -Uflash:w:tiny_i2c_433.hex -Ulfuse:w:${LFUSE}:m -Uhfuse:w:${HFUSE}:m -Uefuse:w:${EFUSE}:m
	DEPENDS tiny_i2c_433.elf
)

ADD_CUSTOM_TARGET(simavr
			run_avr -m ${AVR_DEVICE} -f ${AVR_FREQUENCY} tiny_i2c_433.hex
	DEPENDS tiny_i2c_433.elf
)

