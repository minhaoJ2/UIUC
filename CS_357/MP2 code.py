import numpy as np

x = [0, 0, 2, 0, 2, -4]
x = np.array(x)

F = [0, 0, 0, 0, 0, -20]
F = np.array(F)
 



import numpy as np

def reorder_rows_columns(K,equation_numbers):
    # construct the matrix Khat
    Khat = np.zeros((K.shape[0],K.shape[1]))
    for i in range(equation_numbers.size):
        for j in range(K.shape[1]):
            Khat[i][j] = K[equation_numbers[i]-1][equation_numbers[j]-1]

    return Khat






import numpy as np

def partition_stiffness_matrix(K,equation_numbers,nk):
    # construct the smaller matrices
    Khat = reorder_rows_columns(K,equation_numbers)
    nf = K.shape[0] - nk
    Kpp = np.zeros((nk,nk))
    Kpf = np.zeros((nk,nf))
    Kfp = np.zeros((nf,nk))
    Kff = np.zeros((nf,nf))
    for i in range(nk):
        for j in range(nk):
            Kpp[i][j] = Khat[i][j]
    for i in range(nk):
        for j in range(nf):
            Kpf[i][j] = Khat[i][nk + j]
    for i in range(nf):
        for j in range(nk):
            Kfp[i][j] = Khat[nk + i][j]
    for i in range(nf):
        for j in range(nf):
            Kff[i][j] = Khat[nk + i][nk + j]

    return  Kpp,Kpf,Kfp,Kff







import numpy as np
import scipy.linalg as la
import matplotlib.pyplot as plt

def fea_solve(Kpp,Kpf,Kfp,Kff,xp,Ff):
    # do stuff here
    x = np.matmul(Kfp,xp)
    y = Ff.copy()
    y = y - x
    xf = np.linalg.solve(Kff,y)
    Fp = np.zeros(3)
    matr = np.matmul(Kpf,xf)
    matr2 = np.matmul(Kpp,xp)
    Fp = matr + matr2

    return xf,Fp

xp = np.array([0,0,0])
Ff = np.array([0,0,-10])

Kpp = partition_stiffness_matrix(K,equation_numbers,3)[0]
Kpf = partition_stiffness_matrix(K,equation_numbers,3)[1]
Kfp = partition_stiffness_matrix(K,equation_numbers,3)[2]
Kff = partition_stiffness_matrix(K,equation_numbers,3)[3]

xf = fea_solve(Kpp,Kpf,Kfp,Kff,xp,Ff)[0]
Fp = fea_solve(Kpp,Kpf,Kfp,Kff,xp,Ff)[1]

image_xf = plot_truss(xf)










import numpy as np


def my_lu(A):
    # The upper triangular matrix U is saved in the upper part of the matrix M (including the diagonal)
    # The lower triangular matrix L is saved in the lower part of the matrix M (not including the diagonal)
    # Do NOT use `scipy.linalg.lu`!
    # You should not use pivoting

    M = A.copy()
    for i in range(A.shape[0]-1):
        for j in range(i+1,A.shape[0]):
            M[j,i] = M[j,i]/M[i,i]
            for k in range(i+1,A.shape[0]):
                M[j,k] -= M[j,i]*M[i,k]

    return M


def my_triangular_solve(M, b):
    # A = LU (L and U are stored in M)
    # A x = b (given A and b, find x)
    # M is a 2D numpy array
    # The upper triangular matrix U is stored in the upper part of the matrix M (including the diagonal)
    # The lower triangular matrix L is stored in the lower part of the matrix M (not including the diagonal)
    # b is a 1D numpy array
    # x is a 1D numpy array
    # Do not use `scipy.linalg.solve_triangular`

    n = len(b)
    M = M.copy()
    x = np.zeros(n)
    y = np.zeros(n)

    for i in range(0, n):
        tmp = b[i]
        for j in range(0, i):
            tmp -= y[j]*M[i,j]
        y[i]=tmp

    for i in range(n-1, -1, -1):
        tmp = y[i]
        for j in range(i+1, n):
            tmp -= x[j]*M[i,j]
        x[i] = tmp/M[i,i]


    return x


def fea_solve(Kpp, Kpf, Kfp, Kff, xp, Ff):
    # Use my_lu and my_triangular_solve

    x = np.matmul(Kfp,xp)
    y = Ff.copy()
    y = y - x
    xf = my_triangular_solve(my_lu(Kff),y)
    Fp = np.zeros(3)
    matr = np.matmul(Kpf,xf)
    matr2 = np.matmul(Kpp,xp)
    Fp = matr + matr2

    return xf, Fp








import numpy as np
import matplotlib.pyplot as plt

x = np.matmul(Kfp,xp)
y = Ff.T[0].copy()
y = y - x
mid = my_lu(Kff)
y2 = Ff.T[1].copy()
xf1 = my_triangular_solve(mid, y)
xf2 = my_triangular_solve(mid, y2)

xf_1 = xf1
image_xf_1 = plot_truss(xf_1)
xf_2 = xf2
image_xf_2 = plot_truss(xf2)
