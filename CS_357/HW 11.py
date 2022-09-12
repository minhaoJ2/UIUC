import numpy as np
import numpy.linalg as la
import matplotlib.pyplot as plt

x = np.array([2005, 2006, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015])

y = np.array([12, 41, 63, 72, 78, 80, 83, 88, 84, 90])

A = np.vstack([x, np.ones(len(x))]).T

c1, c0 = la.lstsq(A, y)[0]

print(c1, c0)

plt.scatter(x, y)
plt.plot(x, c1*x + c0, 'r', label='Fitted line')

plt.title('a')
plt.xlabel('x')
plt.ylabel('y')







import numpy as np
import numpy.linalg as la
import matplotlib.pyplot as plt

x = np.array([2005, 2006, 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015])

y = np.array([12, 41, 63, 72, 78, 80, 83, 88, 84, 90])

A = np.vstack([x**2, x, np.ones(len(x))]).T

c2, c1, c0 = la.lstsq(A, y)[0]

print(c1, c0)

plt.scatter(x, y)
plt.plot(x, c2*x**2 + c1*x + c0, 'r', label='Fitted line')

plt.title('a')
plt.xlabel('x')
plt.ylabel('y')








import numpy as np
import numpy.linalg as la
import matplotlib.pyplot as pt

A = np.vstack([np.ones(len(x)),x]).T
c_temp,c1 = la.lstsq(A,np.log(y))[0]
c0 = np.exp(c_temp)

#coeffs = # ...

#pt.plot(xp, coeffs[0]*np.exp(coeffs[1]*xp))
#pt.plot(x, y, "o")







import numpy as np
import numpy.linalg as la
import sympy as sym
import matplotlib.pyplot as plt

def lst_sq(A, b):
    """
    Solve the least square equation using SVD
    """
    # Your code here
    U, s, V = la.svd(A)
    V = V.T
    y = np.zeros(len(A[0]))
    z = np.dot(U.T,b)
    k = 0
    threshold = 0.01
    while k < len(A[0]) and s[k] > threshold:
     y[k] = z[k]/s[k]
     k += 1
    x = np.dot(V,y)
    return x

A = np.vstack([times**2,times,np.ones(len(times))]).T
q_coeffs = lst_sq(A, prices)
t = sym.Symbol('t')
y = q_coeffs[0]*t**2+q_coeffs[1]*t+q_coeffs[2]
df = sym.diff(y,t)
best_time = sym.solve(df,t)[0]
print(best_time)
B = np.vstack([times,np.ones(len(times))]).T
l_coeffs = lst_sq(B,prices)
C = np.vstack([times**5, times**4, times**3,times**2,times,np.ones(len(times))]).T
m_coeffs = lst_sq(C, prices)
plt.plot(times, q_coeffs[0]*times**2+q_coeffs[1]*times+q_coeffs[2])
plt.plot(times, l_coeffs[0]*times+l_coeffs[1])
plt.plot(times, m_coeffs[0]*times**5+m_coeffs[1]*times**4+m_coeffs[2]*times**3+m_coeffs[3]*times**2+m_coeffs[4]*times+m_coeffs[5])
plt.plot(times, prices, 'o')
plt.plot(best_time)
plt.title('time & prices')
plt.legend()
price_plot = plt.gca()






import numpy as np
import numpy.linalg as la
n = len(s)
Sigma = np.zeros(n)
for i in range(n):
    Sigma[i] = np.sqrt(s[i])

Vtransp = X.T
U = A@X@la.inv(np.diag(Sigma))
