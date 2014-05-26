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
    return { k: 'N' if v > max_distinctive else v for k,v in mapping.items() }

def map_invert(map):
    inv_map = {}
    for k, v in map.iteritems():
        inv_map[v] = inv_map.get(v, [])
        inv_map[v].append(k)
    return inv_map

def chop_sequence_alternating(seq, k):
    a, b = defaultdict(int), defaultdict(int)

    for i, n in enumerate(seq+" "):
        if i < k:
            continue
        oligo = seq[i-k:i]

        a["X".join(oligo)] += 1
        b["X".join(oligo[0:-1]) + oligo[-1]] += 1

    return (map_invert(trim_values(a)), map_invert(trim_values(b)))

def print_alternating(name, start, args, chip):
    print "; from %s" % name
    print ";INFO|%s|%s" % (start, args.length)
    print ";ALTERNATING-O|%d" % args.sample_length
    for n, oligos in chip[0].iteritems():
        print ">%s" % str(n)
        for oligo in oligos:
            print oligo
    print ";ALTERNATING-K|%d" % args.sample_length
    for n, oligos in chip[1].iteritems():
        print ">%s" % str(n)
        for oligo in oligos:
            print oligo

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", default="data/ecoli.fa", help="input sequence in FASTA format")
    parser.add_argument("-c", "--chip", default="alternating-ex", help="chip type for SBH simulation")
    parser.add_argument("-s", "--start", type=int, default=2, help="number of known nucleotides")
    parser.add_argument("-k", "--sample-length", type=int, default=5, help="sample length")
    parser.add_argument("-l", "--length", type=int, default=1000, help="target sequence length")

    args = parser.parse_args()

    seq = ""
    while len(seq) < args.length:
        seq_name, seq = random_sequence(args.input)

    seq = select_sequence(seq, args.length)

    if args.chip == "alternating-ex":
        chip = chop_sequence_alternating(seq, args.sample_length)
        print_alternating(seq_name, seq[0:args.start], args, chip)
    else:
        print "unsupported chip type"
