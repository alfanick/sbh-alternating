#!/usr/bin/python

import csv
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
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np

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

    if memory_usage == 0 and process.returncode == 0:
        return process_with_stats(name, stdin)

    return {'execution': execution_time,
            'memory': memory_usage,
            'output': output,
            'status': process.returncode}


def compare_sequnce(a, b):
    def transform(s):
        return ''.join(ch for ch in s.upper() if ch.isalnum())

    l = min(len(a), len(b))
    return transform(a[0:l]) == transform(b[0:l])


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

    sequences = map(lambda x: x.upper(), o.split("\n"))

    quality = best_quality(sequences, s)
    found = quality > 0.99999999
    outputs = len(sequences)

    return (found, quality, outputs)


def spectrum_stream(input="data/ecoli.fa", random=False,
                    chip="alternating-ex", sample_length=5,
                    length=1000, sequence=None):
    spectrum = StringIO()

    sequence = sbh.generate_spectrum(input, random, chip, sample_length*2,
                                     sample_length, length, spectrum, sequence)

    return (spectrum, sequence)


def plot3d(N, K, Z, i, name, zlabel=""):
    fig = plt.figure()
    ax = fig.gca(projection='3d')

    x = list(N)
    y = list(K)

    X, Y = np.meshgrid(x, y)

    zs = []
    for n in N:
        for k in K:
            zs.append(Z[n][k][i])
    Z = np.array(zs).reshape(X.shape)
    ax.set_xlabel('Sequence length')
    ax.set_ylabel('Sample length')
    ax.set_zlabel(zlabel)
    ax.set_xticks(np.arange(min(N), max(N)+1, (max(N)-min(N))/(len(N)-1)))
    ax.set_yticks(np.arange(min(K), max(K)+1, (max(K)-min(K))/(len(K)-1)))
    for item in ax.get_xticklabels() + ax.get_yticklabels() + ax.get_zticklabels():
        item.set_fontsize(8)
    ax.plot_surface(X, Y, Z, cmap=cm.summer, rstride=1, cstride=1,
                    linewidth=1, antialiased=True)

    plt.savefig(name)


def progress_text(text, content="", inline=True, newline=False):
    if inline:
        sys.stderr.write("\r" * 16)
    sys.stderr.write(text + ("%16s" % content) + ("\n" if newline else " "))

if __name__ == "__main__":
    parser = ArgumentParser(formatter_class=ArgumentDefaultsHelpFormatter)

    parser.add_argument("-l", "--min-length", type=int, default=30,
                        help="Minimum length of sequence")
    parser.add_argument("-ml", "--max-length", type=int, default=110,
                        help="Maximum length of sequence")
    parser.add_argument("-sl", "--step-length", type=int, default=10,
                        help="Step for length range")

    parser.add_argument("-k", "--min-sample-length", type=int, default=8,
                        help="Minimum number of known nucleotides")
    parser.add_argument("-mk", "--max-sample-length", type=int, default=12,
                        help="Maximum number of known nucleotides")
    parser.add_argument("-sk", "--step-sample-length", type=int, default=1,
                        help="Step for sample length range")

    parser.add_argument("-rs", "--runs", type=int, default=3,
                        help="Number of runs for each sample")

    parser.add_argument("-b", "--sbh-binary",
                        default=os.path.join(os.path.dirname(
                            os.path.realpath(__file__)), 'sbh'),
                        help="Path to SBH binary")
    parser.add_argument("-i", "--input",
                        default=os.path.join(os.path.dirname(
                            os.path.realpath(__file__)), '..',
                            'data', 'ecoli.fa'),
                        help="Input sequence in FASTA format")
    parser.add_argument("-c", "--chip", default="alternating-ex",
                        help="Chip type for SBH simulation")

    parser.add_argument("-r", "--random", action="store_true", default=False,
                        help="Random data instead of FASTA")
    parser.add_argument("-nr", "--not-related", action="store_true",
                        default=False,
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
        progress_text("Creating output directory\n  ", args.output,
                      newline=True)
    except:
        pass

    with open(os.path.join(args.output, 'params'), 'w') as file:
        file.write(' '.join(sys.argv[1:]))
        progress_text("Saved parameters", newline=True)

    if args.not_related:
        prepared = None
    else:
        _, prepared = spectrum_stream(length=args.max_length, input=args.input,
                                      sample_length=args.max_length,
                                      random=args.random)

        with open(os.path.join(args.output, 'input.seq'), 'w') as file:
            file.write(prepared)
            progress_text("Saved input sequenced", newline=True)

    table = []

    N = range(args.min_length, args.max_length+1, args.step_length)
    K = range(args.min_sample_length, args.max_sample_length+1,
              args.step_sample_length)

    for n in N:
        for k in K:
            progress_text("Sequencing: ", "n=%d, k=%d" % (n, k))
            spectrum, sequence = spectrum_stream(length=n, sample_length=k,
                                                 chip=args.chip,
                                                 input=args.input,
                                                 sequence=prepared)
            sequenced = process_with_stats(args.sbh_binary, spectrum,
                                           args.runs)

            found, quality, results = compare_sequences(sequenced['output'],
                                                        sequence)

            table.append([n, k, found, quality, results,
                          sequenced['memory'], sequenced['execution']])

    progress_text("Sequencing: ", "done", newline=True)

    with open(os.path.join(args.output, 'results.csv'), 'wb') as file:
        writer = csv.writer(file)

        writer.writerow(['length', 'sample', 'status', 'quality',
                         'outputs', 'memory', 'time'])
        for row in table:
            writer.writerow(row)

        progress_text("Saved CSV results", newline=True)

    if args.output_type == 'csv':
        with open(os.path.join(args.output, 'results.csv'), 'r') as file:
            if args.output_type == 'csv':
                print file.read()

    Z = {}

    with open(os.path.join(args.output, 'results.txt'), 'w') as file:
        out = PrettyTable(["length", "sample", "status", "quality",
                           "outputs", "memory", "time"])

        for (n, k, found, quality, results, memory, execution) in table:
            if n not in Z:
                Z[n] = {}
            Z[n][k] = (found,
                       round(quality, 2),
                       results,
                       round(memory/1024.0/1024.0, 2),
                       round(execution, 3))
            out.add_row([n, k, "OK" if found else "Error",
                        int(round(quality*100, 0)),
                        results,
                        round(memory/1024.0/1024.0, 2),
                        round(execution, 3)])

        file.write(out.get_string())

        progress_text("Saved table results", newline=True)

        if args.output_type == 'table':
            print out

    progress_text("Plotting: ", "status")
    plot3d(N, K, Z, 0,
           os.path.join(args.output, 'status_nk.pdf'),
           'Status')

    progress_text("Plotting: ", "quality")
    plot3d(N, K, Z, 1,
           os.path.join(args.output, 'quality_nk.pdf'),
           'Quality')

    progress_text("Plotting: ", "outputs")
    plot3d(N, K, Z, 2,
           os.path.join(args.output, 'outputs_nk.pdf'),
           'Number of sequences')

    progress_text("Plotting: ", "memory")
    plot3d(N, K, Z, 3,
           os.path.join(args.output, 'memory_nk.pdf'),
           'Memory usage [MB]')

    progress_text("Plotting: ", "time")
    plot3d(N, K, Z, 4,
           os.path.join(args.output, 'time_nk.pdf'),
           'Time [s]')

    progress_text("Plotting: ", "done", newline=True)
