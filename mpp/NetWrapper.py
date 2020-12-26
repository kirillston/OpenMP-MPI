from SocialNetwork import SocialNetwork
from DecisionMaker import DecisionMaker
from time import gmtime, strftime
import numpy as np
import logging
import json
import time


class NetWrapper:
    	def __init__(self, json_name):
        	with open(json_name, "r") as read_file:
            		data = json.load(read_file)

        	decision_makers_dict = data['decision_makers']
        	decision_makers = []
        	for record in decision_makers_dict:
            		decision_makers.append(DecisionMaker(record["name"],
                                                 np.array(record['preference_matrix'], dtype=np.double)))
        	first_network = SocialNetwork(decision_makers)
        	first_network.set_trust_mat(np.array(data['trust_matrix']))
        	self.networks = [first_network]

    	def get_network(self, i):
        	return self.networks[i]

    	def run_consensus_reaching_process(self, network_num, epsilon=0.01, iterations_max_num=15, exec_type="default", log_filename="log"):
        	counter = 0
        	network = self.networks[network_num]
		log = True
		if log_filename == "log":
        		filename = "./logs/" + "net" + str(network_num) + "_" + strftime("%Y_%m_%d_%H_%M_%S", gmtime()) + ".log"
		elif log_filename == "no":
			log = False
		else:
			filename = "./logs/" + log_filename
		if log:
			self.logger = logging.getLogger(log_filename)
			self.logger.addHandler(logging.FileHandler(filename))
			self.logger.setLevel("INFO")
			self.logger.info('Iteration ' + str(counter))
        	coll_opinion = network.get_collective_opinion()
        	coll_pref = coll_opinion.eval_best_alternative_num()
        	if log:
			self.logger.info('Collective opinion: ' + str(coll_pref))
        		for dm in network.get_decision_makers():
            			self.logger.info(dm.get_name() + ' opinion: ' + str(dm.eval_best_alternative_num()))
        	consensus_measure = network.evaluate_consensus_measure()
        	if log:
			self.logger.info('Consensus measure = ' + str(consensus_measure) + '\n')
        	counter += 1

        	while counter < iterations_max_num and consensus_measure > epsilon:

            		collective_decision_makers = []
            		all_decision_makers = network.get_decision_makers()
            		for i in range(len(all_decision_makers)):
                		if all_decision_makers[i].eval_best_alternative_num() == coll_pref:
                    			collective_decision_makers.append(i)

            		for i in range(len(all_decision_makers)):
                		if all_decision_makers[i].eval_best_alternative_num() != coll_pref:
                    			max_pref_sim = 0
                    			dm_number = 0
                    			for j in collective_decision_makers:
                        			sim = network.evaluate_preference_similarity(i, j)
                        			if sim > max_pref_sim:
                            				max_pref_sim = sim
                            				dm_number = j
					if exec_type == "default":
						all_decision_makers[i].change_opinion(all_decision_makers[dm_number].get_preference_mat(),
                                                          network.get_trust_mat()[i, dm_number])
					elif exec_type == "plain_c":
						all_decision_makers[i].change_opinion_c(all_decision_makers[dm_number].get_preference_mat(),
                                                          network.get_trust_mat()[i, dm_number])
					elif exec_type == "simd":
						all_decision_makers[i].change_opinion_vec(all_decision_makers[dm_number].get_preference_mat(),
                                                          network.get_trust_mat()[i, dm_number])
					elif exec_type == "omp":
						all_decision_makers[i].change_opinion_omp(all_decision_makers[dm_number].get_preference_mat(),
                                                          network.get_trust_mat()[i, dm_number])
					elif exec_type == "vec_omp":
						all_decision_makers[i].change_opinion_vec_omp(all_decision_makers[dm_number].get_preference_mat(),
                                                          network.get_trust_mat()[i, dm_number])

			if log:
				self.logger.info('Iteration ' + str(counter))
            		coll_opinion = network.get_collective_opinion()
            		if log:
				self.logger.info('Collective opinion: ' + str(coll_opinion.eval_best_alternative_num()))
            			for dm in network.get_decision_makers():
                			self.logger.info(dm.get_name() + ' opinion: ' + str(dm.eval_best_alternative_num()))
            		consensus_measure = network.evaluate_consensus_measure()
            		if log:
				self.logger.info('Consensus measure = ' + str(consensus_measure) + '\n')
           		counter += 1
			if log:
				if counter >= iterations_max_num:
		    			self.logger.info('Consensus was not reached through 15 iterations.')
				else:
		    			self.logger.info('Consensus was reached in ' + str(counter) + ' iterations.')


