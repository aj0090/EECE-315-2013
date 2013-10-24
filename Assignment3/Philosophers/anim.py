import zmq
import subprocess

from math import pi, sin, cos
from direct.showbase.ShowBase import ShowBase
from direct.task import Task
from direct.actor.Actor import Actor

# XXX obviously change this before submitting
ASSETS_DIR = "/home/hamza/Dropbox/_SELF/Panda3D/"

# TODO:
# * scale and pos hard-coded numbers to constants


class PhilosophicalDiners(ShowBase):

    def __init__(self):
        ShowBase.__init__(self)

        self.context, self.socket, self.num_phil = self._create_socket_context(
        )

        # Load environment (really just a plane)
        self.environ = self.loader.loadModel(ASSETS_DIR + "house")
        self.environ.reparentTo(self.render)
        self.environ.setScale(25, 25, 25)

        # Load the "table"
        self.table_model = self.loader.loadModel(ASSETS_DIR + "table")
        self.table_model.reparentTo(self.render)
        self.table_model.setScale(7, 7, 7)
        self.table_model.setPos(0, 0, 1)

        # Load and place forks
        self.forks = [0 for i in xrange(self.num_phil)]
        for i in xrange(self.num_phil):
            self.forks[i] = self.loader.loadModel(ASSETS_DIR + "fork")
            self.forks[i].reparentTo(self.render)
            x, y, angle = self._get_fork_coord(i)
            print angle
            self.forks[i].setScale(5, 5, 5)
            self.forks[i].setPos(x, y, 1.2)
            self.forks[i].setH(self.forks[i], angle * 180 / pi + 90)

        # Apply textures to models
        self.brown_tex = self.loader.loadTexture(ASSETS_DIR + "brown.png")
        self.table_model.setTexture(self.brown_tex)
        self.black_tex = self.loader.loadTexture(ASSETS_DIR + "black.png")
        self.environ.setTexture(self.black_tex)

        self.taskMgr.add(self.spin_camera, "spin_camera")

    def _get_fork_coord(self, num):
        """Do mathemagics to get cartesian coordinates of a fork by its number
        on the table
        """
        diameter = pi * 3.4  # This number is a guess
                          # Take care to change this if table-size changes
        angle = num * 2 * pi / self.num_phil
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
        with open("constants.h", "r") as f:
            found = 0
            lines = f.read().split("\n")
            for line in lines:
                if "ACTIME" in line:
                    timeout = int(2000 * float(line.split(" ")[-1].strip("s")))
                    found += 1
                elif "NPHILOSOPHERS" in line:
                    num_phil = int(line.strip().split(" ")[-1])
                    found += 1
                if found == 2:
                    break
            else:
                raise Exception(
                    "Could not find ACTIME and/or NPHILOSOPHERS bound in constants.h")

        context = zmq.Context()  # Create Context
        socket = context.socket(zmq.REQ)  # Create socket
        # Connect to dining philosophers
        socket.connect("tcp://127.0.0.101:9123")
        socket.RCVTIMEO = timeout  # Set timeout

        return context, socket, num_phil


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
