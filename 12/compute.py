import os
import argparse
import numpy as np
import matplotlib.pyplot as plt

MPIRUN = ("mpirun -n {num_process} matrix.out {dim} time_mode")
N = 50
start = 3
finish = 20
dim_fix = 20
proc_fix = 4

#   This program computes average time for executing the computational core of
#   12.cpp programs and its dependancy on matrix dimension and number of processes.
#
#   Results are shown in the graph that is being saved in 'proc_time.png' 
#   and 'dim_time.png' files.

if __name__ == '__main__':
	K = finish - start + 1
	time_dim = np.zeros(K)
	time_proc = np.zeros(K)
	accum = np.zeros(N)
		
	
	for i in range(K):
		cmd = MPIRUN.format(num_process=i+start, dim=dim_fix)
		print('proc = ', i+start, '  dim =', dim_fix)
		for j in range(N):
			os.system(cmd)
			time = []
			with open('time.txt') as f:
				time = [float(x) for x in f]
			accum[j] = time[0]
		time_proc[i] = np.mean(accum);

	for i in range(K-1):
		cmd = MPIRUN.format(num_process=proc_fix, dim=i+start+1)
		print('proc = ', i+start+1, '  dim =', dim_fix)
		for j in range(N):
			os.system(cmd)
			time = []
			with open('time.txt') as f:
				time = [float(x) for x in f]
			accum[j] = time[0]
		time_dim[i] = np.mean(accum);

	x = np.arange(start, finish+1)
	fig1, ax1 = plt.subplots()
	ax1.plot(x, time_proc, color='red')
	ax1.set_xlabel('processes number')
	ax1.set_ylabel('time spent')
	fig1.savefig('proc_time.png')

	fig2, ax2 = plt.subplots()
	ax2.plot(x, time_dim, color='blue')
	ax2.set_xlabel('matrix dimension')
	ax2.set_ylabel('time spent')
	fig2.savefig('dim_time.png')
