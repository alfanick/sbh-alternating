#!/usr/bin/python

from collections import defaultdict
from pyfasta import Fasta
from random import choice, randint
import argparse


def random_sequence(file):
    fasta = Fasta(file)
    key = choice(fasta.keys())
    return (key, fasta[key])


def select_sequence(seq, length):
    start = randint(0, len(seq) - length)
    return seq[start:(start+length)]


def trim_values(mapping, max_distinctive=2):
    return {k: 'N' if v > max_distinctive else v for k, v in mapping.items()}


def map_invert(map):
    inv_map = {}
    for k, v in map.iteritems():
        inv_map[v] = inv_map.get(v, [])
        inv_map[v].append(k)
    return inv_map


def chop_sequence_alternating(seq, k):
    def remove_known(oligo):
        return "".join([('X' if i % 2 else k) for i, k in enumerate(oligo)])

    a, b = defaultdict(int), defaultdict(int)

    for i, n in enumerate(seq+" "):
        if i >= 2*k-1:
            a[remove_known(seq[i-(2*k-1):i])] += 1
        if i >= 2*k-2:
            oligo = seq[i-(2*k-2):i]
            b[remove_known(oligo[0:-1]) + oligo[-1]] += 1

    return (map_invert(trim_values(a)), map_invert(trim_values(b)))


def output_alternating(name, seq, args, chip):
    yield "; from %s" % name
    yield ";INFO|%s|%s" % (seq[0:args['start']], args['length'])
    yield ";ALTERNATING-O|%d" % args['sample_length']
    for n, oligos in chip[0].iteritems():
        yield ">%s" % str(n)
        for oligo in oligos:
            yield oligo
    yield ";ALTERNATING-E|%d" % args['sample_length']
    for n, oligos in chip[1].iteritems():
        yield ">%s" % str(n)
        for oligo in oligos:
            yield oligo

    yield "; orignal sequence"
    yield "; %s" % seq


def chop_sequence_binary(seq, k):
    wsMap = {'A': 'W', 'T': 'W', 'C': 'S', 'G': 'S'}
    ryMap = {'A': 'R', 'T': 'Y', 'C': 'Y', 'G': 'R'}

    def transform(oligo, tMap):
        return "".join([(tMap[k] if i < len(oligo) - 1 else k)
                        for i, k in enumerate(oligo)])

    a, b = defaultdict(int), defaultdict(int)

    for i, n in enumerate(seq+" "):
        if i >= k and i < len(seq):
            oligo = seq[i-k:i+1]
            a[transform(oligo, wsMap)] += 1
            b[transform(oligo, ryMap)] += 1

    return (map_invert(trim_values(a, 1)), map_invert(trim_values(b, 1)))


def output_binary(name, seq, args, chip):
    yield "; from %s" % name
    yield ";INFO|%s|%s" % (seq[0:args['start']], args['length'])
    yield ";BINARY-WS|%d" % args['sample_length']
    for n, oligos in chip[0].iteritems():
        yield ">%s" % str(n)
        for oligo in oligos:
            yield oligo
    yield ";BINARY-RY|%d" % args['sample_length']
    for n, oligos in chip[1].iteritems():
        yield ">%s" % str(n)
        for oligo in oligos:
            yield oligo

    yield "; orignal sequence"
    yield "; %s" % seq


def generate_spectrum(input="data/ecoli.fa", random=False,
                      chip="alternating-ex", start=10, sample_length=5,
                      length=1000):
    seq = ""

    if random:
        seq_name = "random"
        seq = ''.join(choice(['A', 'T', 'G', 'C']) for _ in range(length))
    else:
        while len(seq) < length:
            seq_name, seq = random_sequence(input)

    seq = select_sequence(seq, length)

    if chip in CHIPS:
        chip_v = CHIPS[chip][0](seq, sample_length)
        for line in CHIPS[chip][1](seq_name, seq, locals(), chip_v):
            print line
    else:
        print "unsupported chip type"

    return seq

CHIPS = {"alternating-ex": (chop_sequence_alternating, output_alternating),
         "binary": (chop_sequence_binary, output_binary)}

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", default="data/ecoli.fa",
                        help="input sequence in FASTA format")
    parser.add_argument("-r", "--random", action="store_true", default=False,
                        help="use random sequence instead of FASTA")
    parser.add_argument("-c", "--chip", default="alternating-ex",
                        help="chip type for SBH simulation (%s)" %
                        ', '.join(CHIPS.keys()))
    parser.add_argument("-s", "--start", type=int, default=10,
                        help="number of known nucleotides")
    parser.add_argument("-k", "--sample-length", type=int, default=5,
                        help="sample length")
    parser.add_argument("-l", "--length", type=int, default=1000,
                        help="target sequence length")

    args = parser.parse_args()

    generate_spectrum(**vars(args))
