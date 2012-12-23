homeauto
========

Remote control power switches from a simple web application.

All you need is :
* Remote switched power plugs that operate at 433MHz
* AtTiny45 http://www.conrad.ch/ce/de/product/154219/
* 433MHz transmitter http://dx.com/p/433mhz-wireless-transmitter-module-superregeneration-for-arduino-green-149254
* A small linux (preferrably debian) device that has ic2 available and can run python. For example: 
		* I use an alix http://pcengines.ch
    	* Also tested with a RaspberryPi http://raspberrypi.org
		* Bifferboard http://bifferos.co.uk/


First flash the code in the directory tiny_i2c_433 onto an AtTiny. The main file RemoteSwitch.c documents how to connect it to the transmitter and the i2c bus.
Once you connected it to the i2c bus of your small linux device, run the python web app homeSwitchWeb.py. It will run on port 5000.
You will have to adjust some settings such as i2c bus address, and rf adressing to match your devices. But it's very simple.
