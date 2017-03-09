# coding: utf-8
from multiprocessing import Pool
import argparse
import os

parser = argparse.ArgumentParser()
parser.add_argument('-n', help='num of testing', type=int, required=True)
args = parser.parse_args()

def run(seed):
    print('start %d th testcase' % (seed))
    os.system('java -jar ./tester.jar -exec ./a.out -seed %d -novis > "tmp/%d"' % (seed, seed))

if __name__=='__main__':
    os.system('mkdir -p tmp')
    os.system('rm tmp/*')
    pool = Pool()

    for i in range(args.n):
        pool.apply_async(run, [1+i])

    pool.close()
    pool.join()
