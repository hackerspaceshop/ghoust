#!/usr/bin/env python
import time

import paho.mqtt.client as mqtt
import importlib

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
        self._config("vibro") 
	self._config("led")

    def out(self):
        print self.pid + ": out"
        # vibrate hard, light red, set inactive
        self.status = "INACTIVE"
	self._config("vibro")
	self._config("led")
    
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
	self._config("vibro")
	self._config("led")

    def win(self):
        print self.pid + ": win"
        # vibrate partily, light green
	self._config("vibro")
	self._config("led")

    ############# raw functions for low level access ##############
    # buzzer, vibro val: [0-1023, 0-1023], [duration (ms), frequency (hz)]
    # led val: [0-1023, 0-1023, 0-10123], [r, g, b]
    # parameter: ["vibro", "buzzer", "led"]
    def _config(self, parameter, val=None, preset=None):
        if parameter not in ["vibro", "buzzer", "led"]:
            print "parameter not valid"
            return
        topic = self.basestring +"/config/{}".format(parameter)
    
        if preset != None:
            if not (0 <= int(preset) <= 9):
                print "vibrate preset not in range"
            self.client.publish(topic, "PRESET:"+ preset)
        
        if val != None:
            if (not (0 <= val[0] <= 1023) or 
                not (0 <= val[1] <= 1023) or
                (parameter == "led" and not(0 <= val[2] <= 1023))): 
                print "config values not in range"
            fstring = "RAW:{:04},{:04}"
            if parameter == "led":
                fstring = "RAW:{:04},{:04},{:04}"
            self.client.publish(topic, fstring.format(*val))


class GHOUST:

    def __init__(self, games):
        
        self.clients = dict()
        
        self.client = mqtt.Client("GHOUST_SRV", clean_session=False)
        self.client.will_set("GHOUST/server/status", "EXIT")
        self.client._on_connect = self._on_connect
        self.client._on_message = self._on_message
        
        # config parameters
        self.max_games = 4
        
        # game modules
        self.games = []
        for g in games:
            m = importlib.import_module("games."+g)
            C = getattr(m, g)
            self.games.append(C(self.clients))

    #### game functions ####
    
    # buzzer, vibro val: [0-1023, 0-1023], [duration (ms), frequency (hz)]
    # led val: [0-1023, 0-1023, 0-10123], [r, g, b]
    # parameter: ["vibro", "buzzer", "led"]

    def _game_config(self, game, parameter, val=None, preset=None):
        if not (0 <= game <= 4) or parameter not in ["vibro", "buzzer", "led"]:
            print "game number or parameter not valid"
            return
        topic = "GHOUST/game/{}/{}".format(game, parameter)
    
        if preset != None:
            if not (0 <= int(preset) <= 9):
                print "vibrate preset not in range"
            self.client.publish(topic, "PRESET:"+ preset)
        
        if val != None:
            if (not (0 <= val[0] <= 1023) or 
                not (0 <= val[1] <= 1023) or
                (parameter == "led" and not(0 <= val[2] <= 1023))): 
                print "vibrate values not in range"
            fstring = "RAW:{:04},{:04}"
            if parameter == "led":
                fstring = "RAW:{:04},{:04},{:04}"
            self.client.publish(topic, fstring.format(*val))

    #### mqtt callbacks ####

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
                            for g in self.games:
                                g._on_conn(pobj, "CONNECTED")
                    elif str(msg.payload) == "DISCONNECTED" and self.clients.has_key(pid):
                            pobj = self.clients[pid]
                            self.clients.pop(pid)
                            for g in self.games:
                                g._on_conn(pid, pobj, "DISCONNECTED")
                            del pobj 

            elif foo == "events":
                    # pass message to game engine callbacks
                    elem = topic[4]
                    pobj = self.clients[pid]

                    if   elem == "button":
                        for g in self.games:
                            g._on_button(pobj, str(msg.payload))
                    elif elem == "accelerometer":
                        for g in self.games:
                            g._on_accelerometer(pobj, str(msg.payload))
                    elif elem == "gestures":
                        for g in self.games:
                            g._on_gestures(pobj, str(msg.payload))
            
    def stop(self):
        self.client.loop_stop()

    def run(self):
        self.client.connect("localhost", 1883, 60)
        self.client.publish("GHOUST/server/status", "ACTIVE")
        
        for g in self.games:
            g.setup()
        
        self.client.loop_forever()



#############################

def filter_clients(c, status=""):
    if status != "":
        return [e for _,e in c.items() if e.status == status]
    return []

if __name__ == "__main__":
    import ghoust_debug_clients
    debug = ghoust_debug_clients.ghoust_debug()

    
    # TODO argparse
    g = GHOUST(["ghoust_game"])
    
    try:
        g.run()
    except KeyboardInterrupt:
        for game in g.games:
            game.stop()
        g.stop()


    print "\n Cleaning up, stopping debug clients"
    debug.stop()

