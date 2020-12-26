import numpy as np

eps = 1e-6


class OpinionHolder(object):
    def __init__(self, preference_mat):
        mat_shape = preference_mat.shape

        if len(mat_shape) != 2:
            raise Exception('Argument should be a square matrix')
        alter_num = mat_shape[0]
        if alter_num != mat_shape[1]:
            raise Exception('Argument should be a square matrix')

        for i in range(alter_num):
            if preference_mat[i][i] != 0:
                raise Exception('All matrix diagonal elements should be zero')

        ones = preference_mat + preference_mat.transpose() + np.eye(alter_num)
        for i in range(alter_num):
            for j in range(alter_num):
                if (ones[i][j] > 1 + eps) | (ones[i][j] < 1 - eps):
                    print(preference_mat)
                    raise Exception('Aij + Aji = 1 (i != j) for all i, j')

        self.preference_mat = preference_mat

    def get_preference_mat(self):
        return self.preference_mat

    def eval_best_alternative_num(self):
        a = self.preference_mat.sum(axis=1)
        max_num = len(np.where(a == a.max())[0])
        if max_num != 1:
            return -1
        else:
            return a.argmax()
