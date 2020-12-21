import os
import argparse
import numpy as np
import matplotlib.pyplot as plt

MPIRUN = ("mpirun -n {num_process} {program}")
N = 50
start = 2
finish = 20

#   This program computes average time for executing the computational core of
#   reduce.cpp and 10_11.cpp programs.
#
#   Results are shown in the graph that is being saved in 'stats.png' file.

if __name__ == '__main__':
	K = finish - start + 1
	time_reduce = np.zeros(K)
	time_prog = np.zeros(K)
	accum = np.zeros(N)
		
	
	for i in range(K):
		cmd = MPIRUN.format(num_process=i+start, program="reduce.out")
		for j in range(N):
			os.system(cmd)
			time = []
			with open('time.txt') as f:
				time = [float(x) for x in f]
			accum[j] = time[0]
		time_reduce[i] = np.mean(accum);

	for i in range(K):
		cmd = MPIRUN.format(num_process=i+start, program="prog.out")
		for j in range(N):
			os.system(cmd)
			time = []
			with open('time.txt') as f:
				time = [float(x) for x in f]
			accum[j] = time[0]
		time_prog[i] = np.mean(accum);

	x = np.arange(start, finish+1)

	plt.plot(x, time_reduce, color='red', label='reduce')
	plt.plot(x, time_prog, color='blue', label='prog')
	plt.xlabel('processes number')
	plt.ylabel('time spent')
	plt.legend()
	plt.savefig('stats.png')
	
