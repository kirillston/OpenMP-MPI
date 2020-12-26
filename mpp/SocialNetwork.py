import numpy as np
from DecisionMaker import DecisionMaker
from OpinionHolder import OpinionHolder


class SocialNetwork:
    def __init__(self, decision_makers):
        self.collective_opinion = None
        if type(decision_makers) == list:
            self.decision_makers = list(decision_makers)
            dm_num = len(decision_makers)
            self.trust_mat = np.zeros((dm_num, dm_num))
        else:
            self.decision_makers = list([decision_makers])
            self.trust_mat = np.zeros((1, 1))

    def add_decision_maker(self, decision_maker):
        self.decision_makers.append(decision_maker)
        mat = self.trust_mat
        n = len(mat)
        mat = np.concatenate((mat, np.zeros((n, 1))), axis=1)
        self.trust_mat = np.concatenate((mat, np.zeros((1, n+1))), axis=0)

    def print_decision_makers(self):
        for dm in self.decision_makers:
            print(dm.get_name())

    def get_decision_maker(self, i):
        if type(i) == int:
            return self.decision_makers[i]
        else:
            for dm in self.decision_makers:
                if dm.get_name() == i:
                    return dm
            raise Exception('No such decision maker in social network')

    def get_decision_makers(self):
        return self.decision_makers

    def get_trust_mat(self):
        return self.trust_mat

    def set_trust_mat(self, trust_mat):
        self.trust_mat = trust_mat

    def add_trust_value(self, dm1, dm2, value):
        type1 = type(dm1)
        type2 = type(dm2)

        # TODO: Add other types of dm

        if not(type1 == int and type2 == int):
            raise Exception('Integers expected')

        self.trust_mat[dm1][dm2] = value

    def get_centrality_index(self, i):
        if type(i) != int:
            raise Exception("Integer expected")

        return np.sum(self.trust_mat, axis=0)[i]/(len(self.decision_makers) - 1)

    def get_relative_importance_degree(self, i):
        if type(i) != int:
            raise Exception("Integer expected")
        m = len(self.decision_makers)
        sum_c = 0
        for j in range(m):
            sum_c += self.get_centrality_index(j)
        return self.get_centrality_index(i)/sum_c

    def get_collective_opinion(self, aggr_op='WA'):
        if self.collective_opinion is None:
            mat_shape = self.decision_makers[0].get_preference_mat().shape
            mat = np.zeros(mat_shape)
            lambdas = [self.get_relative_importance_degree(k) for k in range(len(self.decision_makers))]
            opinions = [self.decision_makers[k].get_preference_mat() for k in range(len(self.decision_makers))]

            if aggr_op == 'WA':
                for i in range(mat_shape[0]):
                    for j in range(mat_shape[1]):
                        fun = 0
                        for k in range(len(self.decision_makers)):
                            fun += lambdas[k] * opinions[k][i][j]
                        mat[i][j] = fun

            self.collective_opinion = OpinionHolder(mat)
        # TODO: Add C
        return self.collective_opinion

    def evaluate_consensus_measure(self):
        coll_pref = self.get_collective_opinion().eval_best_alternative_num()
        counter = 0.0
        for dm in self.decision_makers:
            if dm.eval_best_alternative_num() != coll_pref:
                counter += 1
        return counter / len(self.decision_makers)

    def evaluate_preference_similarity(self, dm1, dm2):
        type1 = type(dm1)
        type2 = type(dm2)

        # TODO: Add other types of dm

        if not (type1 == int and type2 == int):
            raise Exception('Integers expected')

        mat1 = self.decision_makers[dm1].get_preference_mat()
        mat2 = self.decision_makers[dm2].get_preference_mat()
        return np.sum((mat1 - mat2)**2)
