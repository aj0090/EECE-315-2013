Quick Start
===========
* Run ```./install.sh``` to install ZeroMQ dependencies (Debian-based OSes only).
* Install the Panda3D SDK for your respective OS.
* Run ```./make.py -n```. This generates ```constants.h``` and ```make```s the threaded dining philosophers simulation.
* Run ```./anim.py```. This will start the simulation as well as the display.

Dependencies
============
* ZeroMQ (C and Python bindings; czmq, pyzmq)
    * ```$ ./install.sh```
* Panda3D SDK (Grab the appropriate installer [from here](https://www.panda3d.org/download.php?platform=ubuntu&version=1.8.1&sdk))
