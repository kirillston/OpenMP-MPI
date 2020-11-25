#include <omp.h>
#include <iostream>
#include <vector>
#include <random>

#include <stdio.h>
#include <time.h>

//#include "stdafx.h"

#define MAX_NUM_THREADS 4
#define AVG_GROUP_NUM 8
#define DEV_GROUP_NUM 5
#define MIN_MARK 3
#define MAX_MARK 5

int main(int argc, char** argv)
{
	int max_threads = omp_get_max_threads();
	int num_threads = max_threads;
	int num_groups = 0;
	
	int tail_addition = 0;
	
	if (argc == 3) {
		try {
			num_threads = atoi(argv[1]);
			num_groups = atoi(argv[2]);
		}
		catch (int exception)
		{
			std::cerr << "Number of threads and number of groups should be integers.\n";
			return 1;
		}
		
		std::cout << "Input number of threads: " << num_threads
				<< "\nInput number of groups: " << num_groups << "\n";
		
		if (num_threads <= max_threads) {
			if (num_threads > num_groups) {
				std::cout << "Setting number of threads to number of groups.\n";
				num_threads = num_groups;
			}
		} else {
			if (num_groups > max_threads) {
				std::cerr << "Max number of threads is " << max_threads 
						<< " and less than specified number of threads.\n"
						<< "So setting number of threads to maximum.\n";
				num_threads = max_threads;
			} else {
				std::cerr << "Max number of threads is " << max_threads 
						<< " and less than specified number of threads.\n"
						<< "However number of groups even smaller.\n"
						<< "So Setting number of threads to number of groups.\n";	
				num_threads = num_groups;
			}						
		}
	} else {
		std::cerr << "Number of threads and number of groups should be specfied as arguments.\n";
		return 2;
	}
	
	std::cout << "Set number of threads: " << num_threads
			<< "\nSet number of groups: " << num_groups << "\n";
	
	omp_set_num_threads(num_threads);
	
	if (num_groups > num_threads)
		tail_addition = num_threads - num_groups % num_threads;
	
	std::vector<std::vector<int> > marks(num_groups + tail_addition);	
	int iterations_num = marks.size() / num_threads;
	
	std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> group_size_dist(AVG_GROUP_NUM - DEV_GROUP_NUM, AVG_GROUP_NUM + DEV_GROUP_NUM);

	//std::cout << "Iter: " << iterations_num << "\n";
	
	//std::cout << "Parallel loop of group size initialization:\n";
	#pragma omp parallel
	{
		for (size_t i = 0; i < iterations_num; i++)
		{
			int index = i * num_threads + omp_get_thread_num();
			marks[index] = std::vector<int>(group_size_dist(mt));
			//printf("index: %d size: %d thread: %d\n", index, marks[index].size(), omp_get_thread_num());
		}
	}
	/*
	for (size_t i = 0; i < marks.size(); i++) {
		std::cout << i << " " << marks[i].size() << "\n";
	}*/

	std::uniform_int_distribution<int> mark_dist(MIN_MARK, MAX_MARK);
	std::cout << "Parallel loop of marks matrix initialization:\n";
	
	#pragma omp parallel
	{
		for (size_t i = 0; i < iterations_num; i++)
		{
			int index = i * num_threads + omp_get_thread_num();
			if (index < num_groups) {
				size_t group_size = marks[index].size();
				for (size_t j = 0; j < group_size; j++)
				{
					marks[index][j] = mark_dist(mt);
				}
			}
		}
	}
	
	for (size_t i = 0; i < num_groups; i++) {
		for (size_t j = 0; j < marks[i].size(); j++) {
			std::cout << marks[i][j] << " ";
		}
		std::cout << "\n";
	}
	
	double avg_marks[num_groups] = {0};
	
	std::cout << "Evaluating average group marks:\n";
	
	int total_sum = 0;
	int total_group_size = 0;
	
	#pragma omp parallel reduction(+: total_sum) reduction(+: total_group_size)
	{
		for (size_t i = 0; i < iterations_num; i++)
		{
			int index = i * num_threads + omp_get_thread_num();
			if (index < num_groups) {
				size_t group_size = marks[index].size();
				int sum = 0;
				for (size_t j = 0; j < group_size; j++)
				{
					sum += marks[index][j];
				}
				avg_marks[index] = (double)sum / group_size;
				total_sum += sum;
				total_group_size += (int) group_size;
			}
		}
	}
	
	for (size_t i = 0; i < num_groups; i++) {
		std::cout << i << ": " << avg_marks[i] << "\n";
	}
	
	std::cout << "Average mark among all the students: " << (double)total_sum / total_group_size << "\n";
	
    return 0;
}

