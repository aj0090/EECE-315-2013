import zmq
import subprocess

from math import pi, sin, cos
from direct.showbase.ShowBase import ShowBase
from direct.task import Task
from direct.actor.Actor import Actor

ASSETS_DIR = "/home/hamza/Dropbox/_SELF/Panda3D/" # XXX obviously change this before submitting


class PhilosophicalDiners(ShowBase):

    def __init__(self):
        ShowBase.__init__(self)

        self.context, self.socket = self._create_socket_context()

        self.environ = self.loader.loadModel("models/environment")
        self.environ.reparentTo(self.render)
        self.environ.setScale(0.25, 0.25, 0.25)
        self.environ.setPos(-8, 42, 0)

        self.table_model = self.loader.loadModel(ASSETS_DIR + "table")
        self.table_model.reparentTo(self.render)
        self.table_model.setScale(5, 5, 5)
        self.table_model.setPos(0, 0, 10)

        # http://archive3d.net/?a=download&id=af4c5708
        self.fork_model = self.loader.loadModel(ASSETS_DIR + "fork")
        self.fork_model.reparentTo(self.render)
        self.fork_model.setScale(5, 5, 5)
        self.fork_model.setPos(0, 0, 10)


        self.brown_tex = self.loader.loadTexture(ASSETS_DIR + "brown.png")
        self.table_model.setTexture(self.brown_tex)
        # self.silver_tex = self.loader.loadTexture(ASSETS_DIR + "silver.png")
        # self.table_model.setTexture(self.silver_tex)

    def _create_socket_context(self):
        """Creates context, socket and returns them"""
        # Find upper bound on ACTIME from constants and set timeout to double
        # that
        with open("constants.h", "r") as f:
            lines = f.read().split("\n")
            for line in lines:
                if "ACTIME" in line:
                    timeout = int(2000 * float(line.split(" ")[-1].strip("s")))
                    break
            else:
                raise Exception("Did not find ACTIME bound in constants.h")

        context = zmq.Context()  # Create Context
        socket = context.socket(zmq.REQ)  # Create socket
        # Connect to dining philosophers
        socket.connect("tcp://127.0.0.101:9123")
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
