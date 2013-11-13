GO!
===

Type `make` to compile the program.

## Usage

This program requires the first argument be a file that contains the contents of "addresses.txt".
The user can also run a threaded version of the program by supplied an additional argument, `--threaded`.
```
./vmm addresses.txt
./vmm addresses.txt --threaded
```

## Redirection

Redirection can be used to view the contents of the output. The following examples redirects the program's output to a text file, "output.txt".
```
./vmm addresses.txt > output.txt
./vmm addresses.txt --threaded > output.txt
```
