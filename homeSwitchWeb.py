#! /usr/bin/python

import rf433mhz
from flask import Flask
app = Flask(__name__)

rf = rf433mhz.rf433mhz(0x11, 21, 0) # bus is 0 on the alix, and 1 on the raspberrypi

devices = {'A' : -1
		  ,'B' : -1
		  ,'C' : -1
		  }

def preparePage():
	page = "<table><tr>"
	for p,s in devices.items():
		page += "<tr><td>" + p + "</td><td><a href='/switch/" + p + "_1'>" 
		if s == 1:
			page += "<b><img src='/static/" + p + "_1.png' alt='" + p + " on' border='2'></b>"
		else:
			page += "<b><img src='/static/" + p + "_1.png' alt='" + p + " on'></b>"
		page += "</a></td><td><a href='/switch/" + p + "_0'>"
		if s == 0:
			page += "<b><img src='/static/" + p + "_0.png' alt='" + p + " off' border='2'></b>"
		else:
			page += "<b><img src='/static/" + p + "_0.png' alt='" + p + " off'></b>"
		page += "</a></td></tr>"
	page += "</table><br>Icons from:<br>http://www.iconarchive.com<br>http://www.pelfusion.com"
	return page


@app.route("/")
def hello():
	return preparePage()

@app.route('/switch/<name>')
def switchPwr(name=None):
	nn = name.split('_')[0]
	ss = name.split('_')[1]
	for p,s in devices.items():
		if p == nn:
			if ss == '1':
				rf.switchOn(nn)	
				devices[p] = 1
				print 'switched on ' + nn
			else:
				rf.switchOff(nn)
				devices[p] = 0
				print 'switched off ' + nn
	return preparePage()

if __name__ == "__main__":
#	app.debug = True # never enable debug in a production environment
	app.run(host='0.0.0.0')

