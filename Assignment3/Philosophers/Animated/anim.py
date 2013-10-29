#!/usr/bin/env python2.7


import zmq
import subprocess

from math import pi, sin, cos
from direct.showbase.ShowBase import ShowBase
from direct.task import Task
from direct.actor.Actor import Actor
from panda3d.core import TextureStage, Texture, WindowProperties
from direct.gui.OnscreenText import OnscreenText

from get_constants import get_constants


ASSETS_DIR = "assets/"


class PhilosophicalDiners(ShowBase):

    def __init__(self):
        ShowBase.__init__(self)

        #
        self.wp = WindowProperties()
        self.wp.setSize(1280, 720)
        base.win.requestProperties(self.wp)
        self.fork_height = 2.3
        self.frame_time = 0.0
        self.textObject = OnscreenText(text='',
                                       pos=(-1.0, 0.9), scale = 0.10, fg=(255, 255, 255, 200))

        # create sockets, get h constants
        self.p_constants = get_constants()
        self.context, self.socket = self._create_socket_context()

        # Load textures
        self.bowl_tex = self.loader.loadTexture(
            ASSETS_DIR + "marble.jpg")
        self.wood_tex = self.loader.loadTexture(
            ASSETS_DIR + "derevo_mebel.jpg")
        self.chair_tex = self.loader.loadTexture(
            ASSETS_DIR + "02_1.bmp")
        self.black_tex = self.loader.loadTexture(ASSETS_DIR + "black.png")

        # Load models
        # Load environment (really just a plane)
        self.environ = self._load_model(
            "house", scale=[25, 25, 25], pos=[0, 0, -10])
        # Load the "table"
        self.table = self._load_model(
            "round_table", scale=[27, 27, 14], pos=[0, 0, 0])
        # Load and place forks, chairs
        self._load_forks()
        self._load_chairs()
        self._load_bowls()

        # Apply textures to models
        self.environ.setTexture(self.black_tex)
        self.table.setTexture(self.wood_tex)
        self.table.setTexScale(
            TextureStage.getDefault(), 0.005, 0.005)

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
        if task.time - self.frame_time > 0.35:
            self.socket.send("Hello")  # Send generic request
            # Get message, if wait longer than timeout, quit
            try:
                message = self.socket.recv()
            except zmq.error.Again:
                for fork in self.forks:
                    fork.setZ(self.fork_height)
                raw_input(
                    "Looks like everyone is done! Press enter to exit . . .")
                exit(0)

            if self.p_constants["VERBOSE"]:
                print "Received reply ", self.rnum, "[", message, "]"
            self._anim_fork(int(message[0]), int(message[2]))
            self._anim_bowl(int(message[1]), int(message[2]))

            self.rnum += 1
            self.frame_time = task.time

        return Task.cont

    def _anim_bowl(self, bowl_num, fork_pos):

        bowl_num = (bowl_num + 1) % self.p_constants["NPHILOSOPHERS"]

        self.textObject.destroy()
        ost = "\n".join(["{} has eaten {} meals".format(
            n, self.bowls[n]["meals"]) for n in xrange(self.p_constants["NPHILOSOPHERS"])])
        self.textObject = OnscreenText(text=ost,
                                       pos=(-1.26, 0.9), scale=0.08, fg=(130, 130, 130, 125))

        if fork_pos:
            self.bowls[bowl_num]["num_up"] += 1
        else:
            self.bowls[bowl_num]["num_up"] -= 1

        if self.bowls[bowl_num]["num_up"] == 2:
            self.bowls[bowl_num]["meals"] += 1
            self.bowls[bowl_num]["bowl"].setPos(*self.bowls[bowl_num]["pos"])
        else:
            self.bowls[bowl_num]["bowl"].setPos(*[100, 100, 100])

    def _anim_fork(self, fork_num, fork_pos):
        curr_Z = self.forks[fork_num].getZ()
        self.forks[fork_num].setZ(curr_Z + 1 if fork_pos else curr_Z - 1)

    def _load_model(self, name, **kwargs):
        """Load name.egg as name as name in self.models

        Also sets scale, pos, and/or hpr as per kwargs
        """
        model = self.loader.loadModel(ASSETS_DIR + name)
        model.reparentTo(self.render)
        if kwargs.get("scale"):
            assert len(kwargs.get("scale")) == 3
            assert isinstance(kwargs.get("scale"), list)
            model.setScale(*kwargs.get("scale"))
        if kwargs.get("pos"):
            assert len(kwargs.get("pos")) == 3
            assert isinstance(kwargs.get("pos"), list)
            model.setPos(*kwargs.get("pos"))
        if kwargs.get("hpr"):
            assert len(kwargs.get("hpr")) == 3
            assert isinstance(kwargs.get("hpr"), list)
            model.setHpr(*kwargs.get("hpr"))
        if kwargs.get("H"):
            assert isinstance(kwargs.get("H"), float)
            model.setH(model, kwargs.get("H"))

        return model

    def _load_forks(self):
        """Load and place forks"""
        self.forks = self.p_constants["NPHILOSOPHERS"] * [0]
        for i in xrange(self.p_constants["NPHILOSOPHERS"]):
            x, y, angle = self._get_fork_coord(i)
            self.forks[i] = self._load_model(
                "fork", scale=[3, 3, 3], pos=[x, y, self.fork_height], H=rad2deg(angle) + 90)

    def _load_chairs(self):
        """Load and place chairs"""
        self.chairs = self.p_constants["NPHILOSOPHERS"] * [0]
        for i in xrange(self.p_constants["NPHILOSOPHERS"]):
            x, y, angle = self._get_chair_coord(i, 5.0)
            self.chairs[i] = self._load_model(
                "chair1", scale=[7, 7, 7], pos=[x, y - 1, 0], H=rad2deg(angle) + 15)
            self.chairs[i].setTexture(self.chair_tex)
            self.chairs[i].setTexScale(
                TextureStage.getDefault(), 0.005, 0.005)

    def _load_bowls(self):
        """Load and place bowls"""
        self.bowls = [{} for x in xrange(self.p_constants["NPHILOSOPHERS"])]
        for i in xrange(self.p_constants["NPHILOSOPHERS"]):
            x, y, angle = self._get_chair_coord(i, 3.6)
            self.bowls[i]["bowl"] = self._load_model(
                "bowl", scale=[2, 2, 2], pos=[100, 100, 100], H=rad2deg(angle))
            self.bowls[i]["pos"] = [x, y - 1, self.fork_height]
            self.bowls[i]["num_up"] = 0
            self.bowls[i]["meals"] = 0
            self.bowls[i]["bowl"].setTexture(self.bowl_tex)
            self.bowls[i]["bowl"].setTexScale(
                TextureStage.getDefault(), 0.005, 0.005)

    def _get_chair_coord(self, num, diam):
        """Do mathemagics to get cartesian coordinates of a fork by its number
        on the table
        """
        circumf = pi * diam
        angle1 = num * 2 * pi / self.p_constants["NPHILOSOPHERS"]
        angle2 = (num + 1) * 2 * pi / self.p_constants["NPHILOSOPHERS"]
        ang_diff = angle1 - angle2
        angle = angle1 + ang_diff / 2
        return (circumf / 2 * cos(angle), circumf / 2 * sin(angle), angle)

    def _get_fork_coord(self, num):
        """Do mathemagics to get cartesian coordinates of a fork by its number
        on the table
        """
        circumf = pi * 3.6
        angle = num * 2 * pi / self.p_constants["NPHILOSOPHERS"]
        return (circumf / 2 * cos(angle), circumf / 2 * sin(angle), angle)

    def spin_camera(self, task):
        """Spin the camera around the table"""
        angleDegrees = 0  # task.time * 6.0
        angleRadians = angleDegrees * (pi / 180.0)
        self.camera.setPos(
            35 * sin(angleRadians) - 3.3, -27.5 * cos(angleRadians), 16)
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


def rad2deg(angle):
    return angle * 180 / pi


if __name__ == '__main__':
    app = PhilosophicalDiners()
    app.run()
