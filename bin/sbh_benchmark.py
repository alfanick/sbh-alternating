#!/usr/bin/python

from psutil import Process
from argparse import ArgumentParser
# import cPickle as pickle
import seq_gen as sbh
from subprocess import Popen, PIPE
from time import sleep
import os
from time import time

DEVNULL = open(os.devnull, 'wb')

def process_stats(name):
    process = Popen([name, "data/in5.seq"], stdout=DEVNULL, stderr=DEVNULL)
    stats = Process(process.pid)

    memory_usage = -1
    while process.poll() == None:
        memory_usage = max(memory_usage, stats.memory_info().rss)
        sleep(1/100.0)

    execution_time = time() - stats.create_time()

    return {'execution_time': execution_time,
            'memory_usage': memory_usage}

if __name__ == "__main__":
    parser = ArgumentParser()

    args = parser.parse_args()
    process_stats("bin/sbh")
    print sbh.generate_spectrum(output=open("/dev/null", "a"), length=20)
