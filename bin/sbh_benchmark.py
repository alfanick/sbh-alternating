#!/usr/bin/python

from psutil import Process
from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
import seq_gen as sbh
from subprocess import Popen, PIPE
from time import sleep
import os
import sys
from time import time, strftime
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


def compare_sequences(o, s):
    def best_quality(sequences, s):
        quality = 0
        for sequence in sequences:
            same = 0
            for i in xrange(0, min(len(s), len(sequence))):
                if sequence[i] == s[i]:
                    same += 1
            quality = max(quality, same / float(len(s)))
        return quality

    sequences = o.split("\n")

    found = not not filter(lambda x: compare_sequnce(x, s), sequences)
    quality = best_quality(sequences, s) if not found else 1
    outputs = len(sequences)

    return (found, quality, outputs)


def spectrum_stream(input="data/ecoli.fa", random=False,
                    chip="alternating-ex", sample_length=5,
                    length=1000, sequence=None):
    spectrum = StringIO()

    sequence = sbh.generate_spectrum(input, random, chip, sample_length*2,
                                     sample_length, length, spectrum, sequence)

    return (spectrum, sequence)

if __name__ == "__main__":
    parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)

    parser.add_argument("-l", "--min-length", type=int, default=30,
                        help="Minimum length of sequence")
    parser.add_argument("-ml", "--max-length", type=int, default=30,
                        help="Maximum length of sequence")
    parser.add_argument("-sl", "--step-length", type=int, default=10,
                        help="Step for length range")

    parser.add_argument("-k", "--min-sample-length", type=int, default=5,
                        help="Minimum number of known nucleotides")
    parser.add_argument("-mk", "--max-sample-length", type=int, default=5,
                        help="Maximum number of known nucleotides")
    parser.add_argument("-sk", "--step-sample-length", type=int, default=1,
                        help="Step for sample length range")

    parser.add_argument("-rs", "--runs", type=int, default=3,
                        help="Number of runs for each sample")

    parser.add_argument("-b", "--sbh-binary",
                        default=os.path.join(os.path.dirname(os.path.realpath(__file__)), 'sbh'),
                        help="Path to SBH binary")
    parser.add_argument("-i", "--input",
                        default=os.path.join(os.path.dirname(os.path.realpath(__file__)), '..', 'data', 'ecoli.fa'),
                        help="Input sequence in FASTA format")
    parser.add_argument("-c", "--chip", default="alternating-ex",
                        help="Chip type for SBH simulation")

    parser.add_argument("-r", "--random", action="store_true", default=False,
                        help="Random data instead of FASTA")
    parser.add_argument("-nr", "--not-related", action="store_true", default=False,
                        help="Do not use the same sequence for every test")

    oname = "sbh-%s" % strftime('%Y%m%d%H%M%S')
    parser.add_argument("-o", "--output", default="docs/generated/%s" % oname,
                        help="Directory for output files (reports and graphs)")
    parser.add_argument("-ot", "--output-type", default="table",
                        help="Output type - table or csv")

    args = parser.parse_args()

    if args.min_length > args.max_length:
        args.max_length = args.min_length

    if args.min_sample_length > args.max_sample_length:
        args.max_sample_length = args.min_sample_length

    try:
        os.makedirs(args.output)
    except:
        pass

    with open(os.path.join(args.output, 'params'), 'w') as file:
        file.write(' '.join(sys.argv[1:]))

    if args.not_related:
        prepared = None
    else:
        _, prepared = spectrum_stream(length=args.max_length, input=args.input,
                                      sample_length=args.max_length, random=args.random)

        with open(os.path.join(args.output, 'input.seq'), 'w') as file:
            file.write(prepared)

    table = PrettyTable(["length", "sample", "status", "quality", "outputs", "memory", "time"])
    for n in xrange(args.min_length, args.max_length+1, args.step_length):
        for k in xrange(args.min_sample_length, args.max_sample_length+1,
                        args.step_sample_length):
            spectrum, sequence = spectrum_stream(length=n, sample_length=k, chip=args.chip,
                                                 input=args.input, sequence=prepared)
            sequenced = process_with_stats(args.sbh_binary, spectrum,
                                           args.runs)

            found, quality, results = compare_sequences(sequenced['output'], sequence)

            table.add_row([n, k, "OK" if found else "Error",
                           round(quality*100, 0),
                           results,
                           round(sequenced['memory']/1024.0/1024.0, 2),
                           round(sequenced['execution'], 3)])
#        print sequenced['output']

    with open(os.path.join(args.output, 'results.txt'), 'w') as file:
        file.write(table.get_string())

    print table
