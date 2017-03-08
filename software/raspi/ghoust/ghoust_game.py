#!/usr/bin/env python

from ghoust_srv import Player, GHOUST, filter_clients
from threading import Timer, Thread
import time
from IPython import embed

class ghoust_game:
    
    def __init__(self):
        print "init"
        
        g = GHOUST(on_button = self._on_button, 
                on_accelerometer = self._on_accelerometer, 
                on_gestures = self._on_gestures, 
                on_conn = self._on_conn)

        self.players = g.clients

        self.ghoust_srv = g
        
        self.outvalue = 23
        
        self.gameTimer = None
        self.pregameTimer = None
        self.endTimer = None
        self.gamestatus = "init"


        # configs
        self.pregame_t = 5
        self.game_t = 10
        self.end_t = 2

    
    def check_win(self):
        # count alive
        living = filter_clients(self.players, status = "GO")
        if len(living) == 1:
            self.end_game(p = living[0])
        if len(living) == 0:
            print "todo all dead before checkwin"
            exit(-1)

    def setup(self):
        # not used (?)
        self.gamestatus = "setup"
        return

    def pre_game(self):
        print "############# pregame ##############"
        self.gamestatus = "pregame"
        self.endTimer = None

        # all clients in inactive mode
        for _,e in self.players.items():
            e.leave()

    def pre_game_timer(self):
        # configure start timer if 2 or more clients joined
        if len(filter_clients(self.players, status = "ACTIVE")) > 1 :
            self.start_timers(pregame=True)
        else:
            self.stop_timers(pregame=True)

    def start_game(self):
        print "############# game ##############"
        self.gamestatus = "game"
        self.pregameTimer = None
        # all joined clients in go mode
        for e in filter_clients(self.players, status="ACTIVE"):
            e.start()

        # set timer
        self.start_timers(game=True)

    def end_game(self, p=None, timeout=False):
        print "############# endgame ##############"
        self.gamestatus = "endgame"

        if   p != None:
            p.win()
        elif timeout != False:
            for e in filter_clients(self.players, status="GO"):
                e.timeout()
        else:
            for e in self.players:
                e.abort()
        self.stop_timers(game=True)
        self.start_timers(end=True)

    def start_timers(self, pregame=False, game=False, end=False):
        if self.pregameTimer == None and pregame:
            self.pregameTimer = Timer(self.pregame_t, self.start_game)
            self.pregameTimer.start()
        if self.gameTimer == None and game:
            self.gameTimer = Timer(self.game_t, self.end_game, kwargs={"timeout":True})
            self.gameTimer.start()
        if self.endTimer == None and end:
            self.endTimer = Timer(self.end_t, self.pre_game)
            self.endTimer.start()

    def stop_timers(self, pregame=False, game=False, end=False):
        if self.pregameTimer != None and pregame:
            self.pregameTimer.cancel()
            del self.pregameTimer
            self.pregameTimer = None
        if self.gameTimer != None and game:
            self.gameTimer.cancel()
            del self.gameTimer
            self.gameTimer = None
        if self.endTimer != None and end:
            self.endTimer.cancel()
            del self.endTimer
            self.endTimer = None

    ##### callback functions #####
    
    def _on_accelerometer(self, p, value):
        if self.gamestatus == "game" and p.status == "GO":
            if value < self.outvalue:
                p.warn()
            elif value > self.outvalue:
                p.out()
                self.check_win()

        return 0
    
    def _on_button(self, p, clicktype):
        # join/leave game
        if self.gamestatus == "pregame":
            if clicktype == "CLICK" and p.status == "INACTIVE":
                p.join()
            if clicktype == "LONGPRESS" and p.status == "ACTIVE":
                p.leave()
            self.pre_game_timer()

        return 0
    
    def _on_gestures(self, p):
        # not used
        return 0

    def _on_conn(self, p, state):
        if state == "DISCONNECTED":
            if self.gamestate == "game":
                self.check_win()
            elif self.gamestate == "pregame":
                self.pre_game_timer()
        print p.pid +": " + state
        return 0

    ###############################
    
    def main(self):
        self.setup()
        self.pre_game()
        try:
            self.ghoust_srv.run()
        except KeyboardInterrupt :
            self.stop_timers(True, True, True)
        return 0



if __name__ == "__main__":
    import ghoust_debug_clients
    debug = ghoust_debug_clients.ghoust_debug()

    g = ghoust_game()
    g.main()
    print "\n Cleaning up, stopping debug clients" 
    debug.stop()

        
