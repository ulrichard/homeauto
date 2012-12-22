homeauto
========

Remote control power switches from a simple web application.

All you need is :
* Remote switched power plugs that operate at 433MHz
* AtTiny45 http://www.conrad.ch/ce/de/product/154219/ATMEL-AVR-RISC-Microcontroller-Atmel-ATTINY45-20PU-Gehaeuseart-DIL-8-Clock-Speed-20-MHz-Flash-Speicher-4-kB-Speicher-RA/SHOP_AREA_29142&promotionareaSearchDetail=005
* 433MHz transmitter http://dx.com/p/433mhz-wireless-transmitter-module-superregeneration-for-arduino-green-149254
* A small linux device that has ic2 available and can run python. 
  -> Best if it runs debian. 
  -> For example: 
	* I use an alix http://pcengines.ch
    * Also tested with a RaspberryPi http://raspberrypi.org
	* Bifferboard http://bifferos.net


First flash the code in the directory tiny_i2c_433 onto an AtTiny. The main file RemoteSwitch.c documents how to connect it to the transmitter and the i2c bus.
Once you connected it to the i2c bus of your small linux device, run the python web app homeSwitchWeb.py. It will run on port 5000.
You will have to adjust some settings such as i2c bus address, and rf adressing to match your devices. But it's very simple.
