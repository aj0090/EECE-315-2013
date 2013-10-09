import pyperclip
from time import sleep

commands = """echo hello
pwd
cd ..
ls -l
loldontexist
ls -l


quit
"""

for cmd in commands.split("\n"):
    print "Copied {}".format(cmd)
    pyperclip.copy(cmd)
    sleep(5)
