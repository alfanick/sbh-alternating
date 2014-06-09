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
from prettytable import PrettyTable

DEVNULL = open(os.devnull, 'wb')


def process_with_stats(name, stdin, times=1):
    if times > 1:
        results = []
        for _ in xrange(0, times):
            results.append(process_with_stats(name, stdin, 1))

        result = {'execution': reduce(lambda a, c:
                                      a+c['execution']/float(times),
                                      results, 0),
                  'memory': reduce(lambda a, c:
                                   max(a, c['memory']), results, 0),
                  'output': results[0]['output'],
                  'status': results[0]['status']}

        return result

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
                    length=1000, sequence=None):
    spectrum = StringIO()

    sequence = sbh.generate_spectrum(input, random, chip, sample_length*2,
                                     sample_length, length, spectrum, sequence)

    return (spectrum, sequence)

if __name__ == "__main__":
    parser = ArgumentParser()

    args = parser.parse_args()

    _, prepared = spectrum_stream(length=4000, sample_length=4000)

    table = PrettyTable(["length", "sample", "status", "memory", "time"])
    for n in xrange(20, 80, 10):
        for k in xrange(5, 8):
            spectrum, sequence = spectrum_stream(length=n, sample_length=k,
                                                 sequence=prepared)
            sequenced = process_with_stats("bin/sbh", spectrum, 3)
            status = compare_sequnce(sequenced['output'], sequence)

            table.add_row([n, k, "OK" if status else "Error",
                           round(sequenced['memory']/1024.0/1024.0, 2),
                           round(sequenced['execution'], 3)])
        print sequenced['output']

    print table
