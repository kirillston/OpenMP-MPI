from OpinionHolder import OpinionHolder

from ctypes import *

import numpy as np

custom_fun = CDLL("./lib_matop.so")

class DecisionMaker(OpinionHolder):
    def __init__(self, name, preference_mat):
        OpinionHolder.__init__(self, preference_mat)
        self.name = name
        #self.custom_fun = CDLL("./lib_matop.so")

    def get_name(self):
        return self.name

    def change_opinion(self, matrix, trust_index):
        self.preference_mat = (1 - trust_index) * self.preference_mat + trust_index * matrix

    def change_opinion_c(self, matrix, trust_index):
	mat1 = self.preference_mat.astype(np.double)
	matp = mat1.ctypes.data_as(POINTER(c_double))
	mat2 = matrix.astype(np.double)
	mat2 = mat2.ctypes.data_as(POINTER(c_double))
        custom_fun.mat_op(matp, mat2, c_double(trust_index), c_int(matrix.shape[0]))
	self.preference_mat = mat1

    def change_opinion_vec(self, matrix, trust_index):
	mat1 = self.preference_mat.astype(np.double)
	matp = mat1.ctypes.data_as(POINTER(c_double))
	mat2 = matrix.astype(np.double)
	mat2 = mat2.ctypes.data_as(POINTER(c_double))
        custom_fun.mat_op_vec(matp, mat2, c_double(trust_index), c_int(matrix.shape[0]))
	self.preference_mat = mat1

    def change_opinion_omp(self, matrix, trust_index):
	mat1 = self.preference_mat.astype(np.double)
	matp = mat1.ctypes.data_as(POINTER(c_double))
	mat2 = matrix.astype(np.double)
	mat2 = mat2.ctypes.data_as(POINTER(c_double))
        custom_fun.mat_op_omp(matp, mat2, c_double(trust_index), c_int(matrix.shape[0]))
	self.preference_mat = mat1

    def change_opinion_vec_omp(self, matrix, trust_index):
	mat1 = self.preference_mat.astype(np.double)
	matp = mat1.ctypes.data_as(POINTER(c_double))
	mat2 = matrix.astype(np.double)
	mat2 = mat2.ctypes.data_as(POINTER(c_double))
        custom_fun.mat_op_vec_omp(matp, mat2, c_double(trust_index), c_int(matrix.shape[0]))
	self.preference_mat = mat1
	
