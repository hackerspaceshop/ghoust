#!/usr/bin/env python
import time

import paho.mqtt.client as mqtt
import importlib


class Player:
    def __init__(self, pid, mqtt_client, name="", game=None):
        self.pid = pid
	self.client = mqtt_client
        self.team = 0
        self.status = "SELECT_GAME"
	self.basestring = "GHOUST/clients/{0}".format( self.pid )
        self.game = game
        self.select_game(0)
        
    
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
        print self.pid + ": join game ",self.game.game_number
        self.status = "ACTIVE"
        # action?

    def leave(self):
        print self.pid + ": leave ",self.game.game_number
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
    
    def select_game(self, n):
        self.select_game_n = n
        #print "player ",self.pid,": select game, flash ",n
        # TODO flash gamenumber periodically

    def set_game(self, game_p):
        if self.game == game_p:
            return
        
        if self.game != None:
            self.game._leave(self.pid, self)
        
        self.game = game_p
        print self.pid," set game: ", str(game_p)
        if game_p != None:
            self.game._join(self.pid, self)
            self.status = "INACTIVE"
        else:
            self.status = "SELECT_GAME"
    
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

    def __init__(self, game_list):
        
        self.clients = dict()
        
        self.client = mqtt.Client("GHOUST_SRV", clean_session=False)
        self.client.will_set("GHOUST/server/status", "EXIT")
        self.client._on_connect = self._on_connect
        self.client._on_message = self._on_message
        
        # config parameters
        self.max_games = 4
        
        # game modules
        self.games = []
        for i, g in enumerate(game_list):
            m = importlib.import_module("games."+g)
            C = getattr(m, g)
            self.games.append(C(i))

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
            payload = str(msg.payload)
            if len(topic) < 4:
                    print("msg tree too short! debug: " +msg.topic + " " + payload)
                    return
            
            pid = topic[2]
            subtree = topic[3]
            if subtree == "status":
                    if payload == "CONNECTED":
                            
                            pobj = Player(pid, client)
                            self.clients.update({ pid : pobj })
                            if len(self.games) == 1 :
                                pobj.set_game(self.games[0])

                    elif payload == "DISCONNECTED" and self.clients.has_key(pid):
                            pobj = self.clients[pid]
                            self.clients.pop(pid)
                            pobj.set_game(None)

                            del pobj 

            elif subtree == "events":
                    # pass message to game engine callbacks
                    elem = topic[4]
                    pobj = self.clients[pid]
                    
                    if pobj.status == "SELECT_GAME":
                        if elem == "button":
                            if payload == "CLICK":
                                pobj.select_game((pobj.select_game_n + 1) % len(self.games)) # dirty...
                            elif payload == "LONGPRESS":
                                pobj.set_game( self.games[pobj.select_game_n] ) 
                    else:
                        if elem == "button":
                            if payload == "LONGPRESS" and len(self.games) > 1: # Leave game
                                pobj.set_game(None)
                            else:    # let game know of button press
                                pobj.game._on_button(pobj, payload)
                        elif elem == "accelerometer":
                            pobj.game._on_accelerometer(pobj, payload)
                        elif elem == "gestures":
                            pobj.game._on_gestures(pobj, payload)
            
    def stop(self):
        for g in self.games:
            g.stop()
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
    #import ghoust_debug_clients
    #debug = ghoust_debug_clients.ghoust_debug(num_clients=100)

    
    # TODO argparse
    #g = GHOUST(["ghoust_game", "ghoust_game"])
    g = GHOUST(["ghoust_game"])
    
    try:
        g.run()
    except KeyboardInterrupt:
        g.stop()


    #print "\n Cleaning up, stopping debug clients"
    #debug.stop()

