import zmq
import subprocess


def create_socket_context():
    """Creates context, socket and returns them"""
    # Find upper bound on ACTIME from constants and set timeout to double that
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
    socket.connect("tcp://127.0.0.101:9123")  # Connect to dining philosophers
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
    main()
