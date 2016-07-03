#!/bin/python
import json
import sys
from os.path import exists

def print_usage(msg=None):
    if msg:
        print(msg)
    print("json2h.py [sprite.json]")

if __name__ == "__main__":
    arg_count = len(sys.argv)
    if arg_count != 2:
        print_usage("invalid number of arguments")
        exit(0)

    if not exists("../res/" + sys.argv[1]):
        print_usage("not a real json file")
        exit(0)

    print("got here")

