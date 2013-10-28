#!/usr/bin/env python2.7


def get_constants():
    """Get needed constants from constants.h and return as a dict"""
    res = {}

    with open("constants.h", "r") as f:
        lines = map(str.strip, f.read().split("\n"))
    lines = [line.split(" ") for line in lines]
    for line in lines:
        if "ACTIME" in line:
            res["ACTIME_LOWER"] = float(line[-5].strip("s"))
            res["ACTIME_UPPER"] = float(line[-1].strip("s"))
        elif any(term in line for term in ["NPHILOSOPHERS", "NMEALS"]):
            res[line[1]] = int(line[-1])
        elif "VERBOSE" in line:
            res[line[1]] = True if line[2] == "true" else False
        elif "SERV_ADDR" in line:
            res[line[1]] = line[2].strip('"')

    return res


if __name__ == '__main__':
    print get_constants()
