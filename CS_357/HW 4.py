import numpy as np
import matplotlib.pyplot as plt

def func(x):
    '''
    Parameters
    x: 1D numpy array
    Returns
    f: scalar function value
    '''
    # WRITE YOUR CODE HERE
    f = 0.0
    f = 2 * x[0]**2 - 0.5 * x[0] * x[1] + 5 *x[1]**3
    return f

def dfunc(x):
    '''
    Parameters
    x: 1D numpy array
    Returns
    df: 1D numpy array containing first derivatives wrt x
    '''
    # WRITE YOUR CODE HERE
    df = np.zeros(2)
    df[0] = 4 * x[0] - 0.5 * x[1]
    df[1] = 15 * x[1]**2 - 0.5 * x[0]
    return df

def fd(x,dx):
    '''
    Parameters
    x: 1D numpy array
    dx: small perturbation (increment in x)  (scalar)
    Returns
    df: 1D numpy array containing approximations for the first derivatives wrt x
    '''
    # WRITE YOUR CODE HERE
    df = np.zeros(2)
    df[0] = (func(np.array([x[0] + dx, x[1]])) - func(x)) / dx
    df[1] = (func(np.array([x[0], x[1] + dx])) - func(x)) / dx
    return df

# COMPUTE FINITE DIFFERENCE APPROXIMATIONS FOR DECREASING VALUES OF dx
df_exact = dfunc(xvec)
error = []
for dx in dxvec:
    # compute df_approx (using df function)
    df_approx = fd(xvec, dx)
    # compute df_exact (using dfunc)
    err_val = max(abs(df_approx-df_exact))
    # update variable error
    error.append(err_val)
error = np.array(error)
plt.figure()
plt.title('a')
plt.xlabel('x')
plt.ylabel('y')
## Plot the error as a function of the perturbation dx
plt.show()




import numpy as np

f1 = 0.7

f3 = 0.7 - 0.7**3 / 6

sin_ = 0.5 * 2**0.5
diff = 0.7 - np.pi / 4
f3_ = sin_ + sin_ * diff - 0.5 * sin_ * diff**2 - sin_ * (1/6) * diff**3

ft = np.sin(0.7)

err_0_1 = abs((f1 - ft) / ft)

err_0_3 = abs((f3 - ft) / ft)

err_pi4_3 = abs((f3_ - ft) / ft)







import numpy as np
import matplotlib.pyplot as plt

exp_approx = np.zeros(10)
for i in range(10):
    for j in range(i+1):
        #print(j)
        exp_approx[i] += x**j / np.math.factorial(j)

true_value = np.e**x
abs_error = np.zeros(10)
rel_error = np.zeros(10)
abs_error =abs(exp_approx - true_value * np.ones(10))
rel_error  = abs_error / (np.ones(10) * true_value)

N_cutoff = 0
for i in range(10):
    if rel_error[i] < desired_rel_error:
        N_cutoff = i
        break

# Save plot for grading
plt.title('a')
plt.xlabel('x')
plt.ylabel('y')
n = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
plt.semilogy(n, rel_error)
plot = plt.gca()
#plt.show()
