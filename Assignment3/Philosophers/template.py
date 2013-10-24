import subprocess
import sys


def new_constants():
    num_phil = int(raw_input("Number of philosophers? "))
    num_meals = int(raw_input("Number of meals? "))
    verbose = True if raw_input("VERBOSE? (y/n) ") == "y" else False

    constants_h = """
    #ifndef CONSTANTS_H
    #define CONSTANTS_H

    #define ACTIME (random() % 2000000 + 1000000) // 1s < ACTIME < 3s
    #define NPHILOSOPHERS {}
    #define NMEALS {}
    #define VERBOSE {}
    #define SERV_ADDR "tcp://127.0.0.101:9123"

    #endif
    """.format(
        num_phil,
        num_meals,
        "true" if verbose else "false"
    )

    with open("constants.h", "w+") as f:
        f.write(constants_h)


def main():
    try:
        if sys.argv[1] == "-n":
            new_constants()
    except IndexError:
        print("No new constants.h created.")

    subprocess.call("make", shell=True)


if __name__ == '__main__':
    main()
