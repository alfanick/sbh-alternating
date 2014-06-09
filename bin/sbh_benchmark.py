#!/usr/bin/python

# from psutil import Process
from argparse import ArgumentParser
# import cPickle as pickle
import seq_gen as sbh

if __name__ == "__main__":
    parser = ArgumentParser()

    args = parser.parse_args()

    print sbh.generate_spectrum(output=open("/dev/null", "a"), length=20)
