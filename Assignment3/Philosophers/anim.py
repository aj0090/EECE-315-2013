#!/usr/bin/env python2.7


import zmq
import subprocess

from math import pi, sin, cos
from direct.showbase.ShowBase import ShowBase
from direct.task import Task
from direct.actor.Actor import Actor
from panda3d.core import TextureStage, Texture

from .get_constants import get_constants

# XXX obviously change this before submitting
ASSETS_DIR = "/home/hamza/Dropbox/_SELF/Panda3D/"

# TODO:
# * scale and pos hard-coded numbers to constants


class PhilosophicalDiners(ShowBase):

    def __init__(self):
        ShowBase.__init__(self)
        self.p_constants = get_constants()
        self.context, self.socket = self._create_socket_context()

        self.models = {}
        # Load environment (really just a plane)
        self._load_model("house", scale=[25, 25, 25], pos=[0, 0, -10])
        # Load the "table"
        self._load_model("round_table", scale=[27, 27, 14], pos=[0, 0, 0])
        # Load and place forks
        self._load_forks()

        # Apply textures to models
        self.wood_tex = self.loader.loadTexture(
            ASSETS_DIR + "derevo_mebel.jpg")
        self.models["round_table"].setTexture(self.wood_tex)
        self.models["round_table"].setTexScale(
            TextureStage.getDefault(), 0.005, 0.005)

        self.black_tex = self.loader.loadTexture(ASSETS_DIR + "black.png")
        self.models["house"].setTexture(self.black_tex)

        # Tasks
        self.taskMgr.add(self.spin_camera, "spin_camera")

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
        angleDegrees = task.time * 6.0
        angleRadians = angleDegrees * (pi / 180.0)
        self.camera.setPos(
            22 * sin(angleRadians), -22.0 * cos(angleRadians), 12)
        self.camera.setHpr(angleDegrees, -30, 0)
        # print self.camera.getHpr()
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


def main():
    context, socket = create_socket_context()
    philos = subprocess.Popen("./philos", shell=True)

    rnum = 0
    while True:
        socket.send("Hello")  # Send generic request
        # Get message, if wait longer than timeout, quit
        try:
            message = socket.recv()
        except zmq.error.Again:
            print("Looks like everyone is done!")
            break

        print "Received reply ", rnum, "[", message, "]"

        rnum += 1


if __name__ == '__main__':
    app = PhilosophicalDiners()
    app.run()
