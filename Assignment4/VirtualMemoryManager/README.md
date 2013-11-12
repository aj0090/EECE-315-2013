To compile the program, type 'make' into terminal.


This program VMM requires that the first argument be a file that contains the contents of "addresses.txt". The user can run a threaded version of the program by supplied a third argument, "--threaded".


Usage:

./vmm addresses.txt

./vmm addresses.txt --threaded


Redirection may also be used, in order to view the contents of the output. The following example redirects the programs output to a text file, "output.txt".


Usage:

./vmm addresses.txt > output.txt

./vmm addresses.txt --threaded > output.txt


