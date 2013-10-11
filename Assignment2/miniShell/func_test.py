import os
import sys
import pexpect
import subprocess
from time import sleep
from tempfile import TemporaryFile

TMP_FILE = "miniShell_functest"


def main():
    shell = pexpect.spawn("./miniShell")
    shell.logfile = sys.stdout

    # Test echo
    shell.sendline("echo hello")
    shell.expect("hello.*", timeout=2)

    # Test pwd
    shell.sendline("pwd")
    shell.expect("{}.*".format(os.getcwd()), timeout=2)

    # Test cd
    t = TemporaryFile()
    subprocess.call(["ls", "-l", ".."], stdout=t)
    t.seek(0)
    shell.sendline("cd ..")
    shell.sendline("ls -l")
    shell.expect(".*" + "drwxrwxr" + ".*", timeout=2)

    # Test non-existent file
    shell.sendline("loldontexist")
    shell.expect(".*in the PATH.*", timeout=2)

    # Test foreground waiting
    shell.sendline("firefox")
    shell.expect(".*EECE315.*", timeout=5)

    # Test background running
    shell.sendline("firefox &")
    shell.expect(".*EECE315.*", timeout=2)

    # Test file redirection
    shell.sendline("cd miniShell")
    if os.path.exists(TMP_FILE):
        os.remove(TMP_FILE)
    shell.sendline("ls -l .. > {}".format(TMP_FILE))
    sleep(2)
    assert os.path.exists(TMP_FILE)
    with open(TMP_FILE, "r") as f:
        f_str = f.read().strip()
    t.seek(0)
    print t.read()
    t.seek(0)
    print f_str
    assert t.read().strip() == f_str

    # Test SIGINT handler
    shell.sendline(chr(3))

    # Test quit
    shell = pexpect.spawn("./miniShell")
    shell.logfile = sys.stdout
    shell.sendline("quit")


if __name__ == '__main__':
    main()
