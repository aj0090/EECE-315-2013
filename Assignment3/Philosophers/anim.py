#!/usr/bin/env python2.7


import zmq
import subprocess

from math import pi, sin, cos
from direct.showbase.ShowBase import ShowBase
from direct.task import Task
from direct.actor.Actor import Actor
from panda3d.core import TextureStage, Texture

from get_constants import get_constants


ASSETS_DIR = "assets/"


class PhilosophicalDiners(ShowBase):

    def __init__(self):
        ShowBase.__init__(self)
        self.p_constants = get_constants()
        self.context, self.socket = self._create_socket_context()

        # Load texture
        self.wood_tex = self.loader.loadTexture(
            ASSETS_DIR + "derevo_mebel.jpg")

        self.models = {}
        # Load environment (really just a plane)
        self._load_model("house", scale=[25, 25, 25], pos=[0, 0, -10])
        # Load the "table"
        self._load_model("round_table", scale=[27, 27, 14], pos=[0, 0, 0])
        # Load and place forks
        self._load_forks()
        #
        self._load_chairs()

        # Apply textures to models

        self.models["round_table"].setTexture(self.wood_tex)
        self.models["round_table"].setTexScale(
            TextureStage.getDefault(), 0.005, 0.005)

        self.black_tex = self.loader.loadTexture(ASSETS_DIR + "black.png")
        self.models["house"].setTexture(self.black_tex)

        # Start sim
        self.rnum = 0
        self.philos = subprocess.Popen("./philos", shell=True)

        # Tasks
        self.taskMgr.add(self.spin_camera, "spin_camera")
        self.taskMgr.add(self._comm, "comm")

    def _comm(self, task):
        """Communicate with sim

        This is a blocking function
        """
        if True:#not int(task.time*100) % 2:
            self.socket.send("Hello")  # Send generic request
            # Get message, if wait longer than timeout, quit
            try:
                message = self.socket.recv()
            except zmq.error.Again:
                print("Looks like everyone is done!")
                exit(0)

            print "Received reply ", self.rnum, "[", message, "]"
            self._anim_fork(int(message[0]), int(message[2]))


            self.rnum += 1
        print task.time
        return Task.cont

    def _anim_fork(self, fork_num, fork_pos):
        curr_Z = self.forks[fork_num].getZ()
        self.forks[fork_num].setZ(curr_Z + 2 if fork_pos == 1 else curr_Z - 2)

    def _load_model(self, name, **kwargs):
        """Load name.egg as name as name in self.models

        Also sets scale, pos, and/or hpr as per kwargs
        """
        self.models[name] = self.loader.loadModel(ASSETS_DIR + name)
        self.models[name].reparentTo(self.render)
        if kwargs.get("scale"):
            assert len(kwargs.get("scale")) == 3
            assert isinstance(kwargs.get("scale"), list)
            self.models[name].setScale(*kwargs.get("scale"))
        if kwargs.get("pos"):
            assert len(kwargs.get("pos")) == 3
            assert isinstance(kwargs.get("pos"), list)
            self.models[name].setPos(*kwargs.get("pos"))
        if kwargs.get("hpr"):
            assert len(kwargs.get("hpr")) == 3
            assert isinstance(kwargs.get("hpr"), list)
            self.models[name].setHpr(*kwargs.get("hpr"))

    def _load_forks(self):
        """Load and place forks"""
        self.forks = [0 for i in xrange(self.p_constants["NPHILOSOPHERS"])]
        for i in xrange(self.p_constants["NPHILOSOPHERS"]):
            x, y, angle = self._get_fork_coord(i)
            self.forks[i] = self.loader.loadModel(ASSETS_DIR + "fork")
            self.forks[i].reparentTo(self.render)
            self.forks[i].setScale(3, 3, 3)
            self.forks[i].setPos(x, y, 2.3)
            self.forks[i].setH(self.forks[i], angle * 180 / pi + 90)

    def _load_chairs(self):
        """Load and place chairs"""
        self.chairs = [0 for i in xrange(self.p_constants["NPHILOSOPHERS"])]
        for i in xrange(self.p_constants["NPHILOSOPHERS"]):
            x, y, angle = self._get_chair_coord(i)
            self.chairs[i] = self.loader.loadModel(ASSETS_DIR + "chair")
            self.chairs[i].reparentTo(self.render)
            self.chairs[i].setScale(1, 1, 1)
            self.chairs[i].setPos(x, y, 2.3)
            self.chairs[i].setH(self.chairs[i], angle * 180 / pi + 90)
            self.chairs[i].setTexture(self.wood_tex)

    def _get_chair_coord(self, num):
        """Do mathemagics to get cartesian coordinates of a fork by its number
        on the table
        """
        diameter = pi * 5.6  # This number is a guess
                          # Take care to change this if table-size changes
        angle1 = num * 2 * pi / self.p_constants["NPHILOSOPHERS"]
        angle2 = (num+1) * 2 * pi / self.p_constants["NPHILOSOPHERS"]
        ang_diff = angle1-angle2
        angle = angle1 + ang_diff/2
        return (diameter / 2 * cos(angle), diameter / 2 * sin(angle), angle)

    def _get_fork_coord(self, num):
        """Do mathemagics to get cartesian coordinates of a fork by its number
        on the table
        """
        diameter = pi * 3.6  # This number is a guess
                          # Take care to change this if table-size changes
        angle = num * 2 * pi / self.p_constants["NPHILOSOPHERS"]
        return (diameter / 2 * cos(angle), diameter / 2 * sin(angle), angle)

    def spin_camera(self, task):
        """Spin the camera around the table"""
        angleDegrees = 0#task.time * 6.0
        angleRadians = angleDegrees * (pi / 180.0)
        self.camera.setPos(
            25 * sin(angleRadians), -25.0 * cos(angleRadians), 15)
        self.camera.setHpr(angleDegrees, -30, 0)
        return Task.cont

    def _create_socket_context(self):
        """Creates context, socket and returns them"""
        # Find upper bound on ACTIME from constants and set timeout to double
        # that
        timeout = int(2000 * self.p_constants["ACTIME_UPPER"])

        context = zmq.Context()  # Create Context
        socket = context.socket(zmq.REQ)  # Create socket
        # Connect to dining philosophers
        socket.connect(self.p_constants["SERV_ADDR"])
        socket.RCVTIMEO = timeout  # Set timeout

        return context, socket



if __name__ == '__main__':
    app = PhilosophicalDiners()
    app.run()
