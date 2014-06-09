#!/usr/bin/python

from psutil import Process
from argparse import ArgumentParser
# import cPickle as pickle
import seq_gen as sbh
from subprocess import Popen, PIPE
from time import sleep
import os
from time import time
from cStringIO import StringIO

DEVNULL = open(os.devnull, 'wb')


def process_with_stats(name, stdin):
    process = Popen(name, stdin=PIPE, stdout=PIPE, stderr=DEVNULL,
                    close_fds=True)
    process.stdin.write(stdin.getvalue())
    process.stdin.close()

    stats = Process(process.pid)

    memory_usage = stats.memory_info().rss
    while process.poll() is None:
        try:
            memory_usage = max(memory_usage, stats.memory_info().rss)
        except:
            memory_usage = 0
        sleep(1/500.0)

    execution_time = time() - stats.create_time()
    output = process.stdout.read()

    if memory_usage == 0:
        return process_with_stats(name, stdin)

    return {'execution': execution_time,
            'memory': memory_usage,
            'output': output,
            'status': process.returncode}


def compare_sequnce(a, b):
    def transform(s):
        return ''.join(ch for ch in s.upper() if ch.isalnum())

    return transform(a) == transform(b)


def spectrum_stream(input="data/ecoli.fa", random=False,
                    chip="alternating-ex", sample_length=5,
                    length=1000):
    spectrum = StringIO()

    sequence = sbh.generate_spectrum(input, random, chip, sample_length*2,
                                     sample_length, length, spectrum)

    return (spectrum, sequence)

if __name__ == "__main__":
    parser = ArgumentParser()

    args = parser.parse_args()

    spectrum, sequence = spectrum_stream(length=800, sample_length=9)

    sequenced = process_with_stats("bin/sbh", spectrum)

    if compare_sequnce(sequenced['output'], sequence):
        print "Correct sequence!"
        print sequence

    else:
        print "buuu"

    print "time: %.4fs, memory: %.2fMB, status: %d" % (
        sequenced['execution'],
        sequenced['memory']/2.0**20,
        sequenced['status'])
