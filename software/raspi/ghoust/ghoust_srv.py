#!/usr/bin/env python
import time

import paho.mqtt.client as mqtt

from IPython import embed


class Player:
    def __init__(self, pid, mqtt_client, name=""):
        self.pid = pid
	self.client = mqtt_client
        self.team = 0
        self.status = "INACTIVE"
	self.basestring = "GHOUST/clients/{0}".format( self.pid )
        
    def name(self, name):
        self.name = name

    def team(self, n):
        self.team = n

    def warn(self):
        print self.pid + ": warn"
        # vibrate or light a little
	# TODO
        self._vibrate() 
	self._light()

    def out(self):
        print self.pid + ": out"
        # vibrate hard, light red, set inactive
        self.status = "INACTIVE"
	self._vibrate()
	self._light()
    
    def timeout(self):
        print self.pid + ": timeout"
        self.status = "INACTIVE"
        # TODO timeout action
    
    def abort(self):
        print self.pid + ": abort"
        self.status = "INACTIVE"
        # TODO abort action

    def join(self):
        print self.pid + ": join"
        self.status = "ACTIVE"
        # action?

    def leave(self):
        print self.pid + ": leave"
        self.status = "INACTIVE"
        # action ?
        
    def start(self):
        print self.pid + ": start"
        self.status = "GO"
	self._vibrate()
	self._light()

    def win(self):
        print self.pid + ": win"
        # vibrate partily, light green
	self._vibrate()
	self._light()

    ############# raw functions for low level access ##############

    def _vibrate(self, val=None, preset=None): # val: [0-1023, 0-1023], [duration (ms), frequency (hz)]
        if preset != None:
            if not (0 <= int(preset) <= 9):
                print "vibrate preset not in range"
            self.client.publish(self.basestring + "/config/vibro", "PRESET:"+ preset)
        if val != None:
            if (not (0 <= val[0] <= 1023) or 
                not (0 <= val[1] <= 1023)): 
                print "vibrate values not in range"
            self.client.publish(self.basestring + "/config/vibro", "RAW:{:04},{:04}".format(*val))
    
    def _buzzer(self, val=None, preset=None): # val: [0-1023, 0-1023], [duration (ms), frequency (hz)]
        if preset != None:
            if not (0 <= int(preset) <= 9):
                print "buzzer preset not in range"
            self.client.publish(self.basestring + "/config/buzzer", "PRESET:"+ preset)
        if val != None:
            if (not (0 <= val[0] <= 1023) or 
                not (0 <= val[1] <= 1023)): 
                print "buzzer values not in range"
            self.client.publish(self.basestring + "/config/buzzer", "RAW:{:04},{:04}".format(*val))

    def _light(self, val=None, preset=None): # val: [0-1023, 0-1023, 0-10123], [r, g, b]
        if preset != None:
            if not (0 <= int(preset) <= 9):
                print "light preset not in range"
            self.client.publish(self.basestring + "/config/led", "PRESET:"+ preset)
        if val != None:
            if (not (0 <= val[0] <= 1023) or 
                not (0 <= val[1] <= 1023) or 
                not (0 <= val[2] <= 1023)): 
                print "light values not in range"
            self.client.publish(self.basestring + "/config/led", "RAW:{:04},{:04},{:04}".format(*val))


class GHOUST:
    def _dummy_cb(*args):
        print "dummy callback"
    
    def __init__(self, accel_threshold = [20,20,20], on_button = _dummy_cb, on_accelerometer = _dummy_cb, on_gestures = _dummy_cb, on_conn = _dummy_cb):
        
        self.clients = dict()
        
        self.client = mqtt.Client("GHOUST_SRV", clean_session=False)
        self.client.will_set("GHOUST/server/status", "EXIT")
        self.client._on_connect = self._on_connect
        self.client._on_message = self._on_message
        
        self.accel_threshold = accel_threshold
        # TODO accel_threshold to clients

        # game callbacks 
	self.on_button =  on_button 
        self.on_accelerometer = on_accelerometer
        self.on_gestures = on_gestures
        self.on_conn = on_conn

    #### mqtt callbacks ###

    def _on_connect(self, client, userdata, flags, rc):
        print("Connected with result code " + str(rc))

        client.subscribe("GHOUST/clients/+/status")
        client.subscribe("GHOUST/clients/+/events/button")
        client.subscribe("GHOUST/clients/+/events/accelerometer")
        client.subscribe("GHOUST/clients/+/events/gestures")

    def _on_message(self, client, userdata, msg):
            topic = msg.topic.split("/")
            if len(topic) < 4:
                    print("msg tree too short! debug: " +msg.topic + " " + str(msg.payload))
                    return
            
            pid = topic[2]
            foo = topic[3]
            if foo == "status":
                    if str(msg.payload) == "CONNECTED":
                            pobj = Player(pid, client)
                            self.clients.update({ pid : pobj })
                            self.on_conn(pobj, "CONNECTED")
                    elif str(msg.payload) == "DISCONNECTED" and self.clients.has_key(pid):
                            self.clients.pop(pid)
                            pobj = self.clients[pid]
                            self.on_conn(pid, pobj, "DISCONNECTED")
                

            elif foo == "events":
                    # pass message to game engine callbacks
                    elem = topic[4]
                    pobj = self.clients[pid]

                    if   elem == "button":
                        self.on_button(pobj, str(msg.payload))
                    elif elem == "accelerometer":
                        self.on_accelerometer(pobj, str(msg.payload))
                    elif elem == "gestures":
                        self.on_gestures(pobj, str(msg.payload))

                    #print pid, ": ", msg.topic, "; "+ msg.payload
            
    def stop(self):
        self.client.loop_stop()

    def run(self):
        self.client.connect("localhost", 1883, 60)
        self.client.publish("GHOUST/server/status", "ACTIVE")
        
        self.client.loop_forever()



#############################

def filter_clients(c, status=""):
    if status != "":
        return [e for _,e in c.items() if e.status == status]
    return []

if __name__ == "__main__":
    g = GHOUST()
    g.run()
