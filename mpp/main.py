from NetWrapper import NetWrapper
import time
import logging
import numpy as np
import matplotlib.pyplot as plt

filename = 'resources/vacation0.json'
N = 200

times = np.zeros((5,N))

for i in range(N):
	wrapper0 = NetWrapper(filename)
	t_start = time.time()
	wrapper0.run_consensus_reaching_process(0, exec_type="default", log_filename = "no")
	times[0,i] = (time.time() - t_start) * 1000
t0 = np.mean(times[0])
print("Consensus reaching process evaluated using python ended in appr ", t0)

for i in range(N):
	wrapper1 = NetWrapper(filename)
	t_start = time.time()
	wrapper1.run_consensus_reaching_process(0, exec_type="plain_c", log_filename = "no")
	times[1,i] = (time.time() - t_start) * 1000
t1 = np.mean(times[1])
print("Consensus reaching process evaluated using clear C ended in appr ", t1)

for i in range(N):
	wrapper2 = NetWrapper(filename)
	t_start = time.time()
	wrapper2.run_consensus_reaching_process(0, exec_type="simd", log_filename = "no")
	times[2,i] = (time.time() - t_start) * 1000
t2 = np.mean(times[2])
print("Consensus reaching process evaluated using SIMD instructions ended in appr ", t2)

for i in range(N):
	wrapper3 = NetWrapper(filename)
	t_start = time.time()
	wrapper3.run_consensus_reaching_process(0, exec_type="omp", log_filename = "no")
	times[3,i] = (time.time() - t_start) * 1000
t3 = np.mean(times[3])
print("Consensus reaching process evaluated using OpenMP pragmas ended in appr ", t3)

for i in range(N):
	wrapper4 = NetWrapper(filename)
	t_start = time.time()
	wrapper4.run_consensus_reaching_process(0, exec_type="vec_omp", log_filename = "no")
	times[4,i] = (time.time() - t_start) * 1000
t4 = np.mean(times[4])
print("Consensus reaching process evaluated using SIMD instructions and OpenMP pragmas ended in appr ", t4)

execs = ['python','C','SIMD','OpenMP','SIMD+OpenMP']
time_spent = [t0,t1,t2,t3,t4]
plt.bar(execs, time_spent)
plt.xlabel('')
plt.ylabel('time spent')
plt.savefig('stats.png')
