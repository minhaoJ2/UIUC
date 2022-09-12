import matplotlib.pyplot as pt
import numpy as np
import numpy.linalg as la

# a) Plot X
pt.plot(X)

# b) Plot this on top
pt.gca().set_aspect("equal")
dim = 2
mean = np.array([1/3000*sum(X[:,0]), 1/3000*sum(X[:,1])])

# for i in range(dim):
#     pt.arrow(mean[0], mean[1],
#             principal_components[i, 0], principal_components[i, 1],
#             linewidth=4, zorder=10)

# c) Y
N, M = X.shape
Y = np.zeros((N, 2))

# d) Compute X_prime and plot as before

for i in range(N):
    Y[i] = (X[i] - mean)*(1/np.sqrt(N-1))
U, S, Vt = np.linalg.svd(Y, full_matrices = False)
sigma = np.diag(S)
principal_components = (sigma@Vt)
S_prime = sigma.copy()
S_prime[1][1] = 0
Y_prime = U@S_prime@Vt
X_prime = np.zeros((N,2))
for i in range(N):
    X_prime[i] = Y_prime[i]*(np.sqrt(N-1))+mean



import numpy as np
import numpy.linalg as la

temp = np.delete(baseball_data.copy(),0,1)
mean = []
for i in range(len(temp[0])):
    mean.append(1/len(temp)*np.sum(temp[:,i]))

Y = np.zeros(temp.shape)

for i in range(len(temp)):
    for j in range(len(temp[0])):
        Y[i][j] = temp[i][j] - mean[j]

U, S, Vt = np.linalg.svd(Y, full_matrices=False)

p1 = Y@Vt[0]/la.norm(Vt[0])
p2 = Y@Vt[1]/la.norm(Vt[1])

cov_age_runs = np.cov(temp[:,0:1].reshape((N,)), temp[:,7:8].reshape((N,)))[0][1]

cov_stole_bases = np.cov(temp[:,9:10].reshape((N,)), temp[:,10:11].reshape((N,)))[0][1]
