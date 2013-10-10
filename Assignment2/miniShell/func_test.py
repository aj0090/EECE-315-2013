import os
import pexpect
import subprocess
from tempfile import TemporaryFile

# commands = """echo hello
# pwd
# cd ..
# ls
# loldontexist
# ls


# quit
# """

shell = pexpect.spawn("./miniShell")

shell.sendline("echo hello")
shell.expect("hello.*", timeout=2)

shell.sendline("pwd")
shell.expect("{}.*".format(os.getcwd()), timeout=2)

shell.sendline("cd ..")
shell.sendline("ls")
t = TemporaryFile()
subprocess.call(["ls", ".."], stdout=t)
shell.expect(t.read() + ".*", timeout=2)

shell.sendline("loldontexist")
shell.expect(".*in the PATH.*", timeout=2)



# for cmd in commands.split("\n"):
#     shell.sendline(cmd)
#     # print "Copied {}".format(cmd)
#     # pyperclip.copy(cmd)
#     # sleep(5)
