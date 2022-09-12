import numpy as np

xk1 = xks[1]
xk0 = xks[0]
roots = np.zeros(5)

for i in range(5):
    fp = (f(xk1) - f(xk0)) / (xk1 - xk0)
    xk2 = xk1 - f(xk1) / fp
    roots[i] = xk2

    xk0 = xk1
    xk1 = xk2

print(xks, roots)













import numpy as np

roots = []

for i in range(len(intervals)):
    a = intervals[i][0]
    b = intervals[i][1]

    if not (a < b) or not (function(a) * function(b) < 0):
        roots.append(None)
        continue
    count = 0
    for j in range(n_iter):
        m = (a + b) / 2
        fm = function(m)
        if (not abs(fm) > epsilon):
            roots.append(m)
            break

        fa = function(a)
        fb = function(b)

        if fm * fa > 0:
            a = m

        if fm * fb > 0:
            b = m
        # if not (a < b) or not (function(a) * function(b) < 0):
        #     roots[i] = None
        #     break
        count += 1

    if count == n_iter:
        roots.append(None)
print(roots)







import numpy as np
import numpy.linalg as la

def f(x,y):
    return np.array([x**3 - y**2, x+y*x**2 - 2])

# A function that returns the Jacobian may be useful
def J(x,y):
    return np.array([[3*x**2, -2*y], [1 + 2*x*y, x**2]])

res = 10
x = xi[0]
y = xi[1]
while not res < tol:
    xNew = [x, y] - la.inv(J(x, y))@f(x, y)

    x = xNew[0]
    y = xNew[1]
    root = np.array([x, y])
    res = la.norm(f(x, y), 2)









import numpy as np
import matplotlib.pyplot as plt

iterations_to_converge = []
Xs = np.zeros((len(X0), 10))
iteration_limit = 100

def f(x):
    return 0.5 * x * np.sin(2 * x) - 0.8 * x + 1

def dx(x):
    return 0.5 * np.sin(2*x) + x * np.cos(2*x) - 0.8

for i in range(len(X0)):

    diff = 10
    count = 0
    x0 = X0[i]

    while not diff < tol and count < 100:
        fx0 = f(x0)
        fpx0 = dx(x0)

        x1 = x0 - (fx0 / fpx0)
        diff = abs(x1 - x0)
        if count <= 9:
            Xs[i][count] = x1

        count += 1
        x0 = x1

    iterations_to_converge.append(count)


# Use the following code to plot the result.
plt.figure(figsize = (8,6))
plot_x = np.linspace(-5, 5, 500)
plt.plot(plot_x, f(plot_x), label = "fx = 0.5x * sin(2x) - 0.8 * x + 1")
plt.plot(plot_x, [0 for i in range(len(plot_x))], label="f(x) = 0")
plt.scatter(X0, f(X0), c = [1 - t/100 for t in iterations_to_converge], label = "heat-map of times to converge")
for i in range(len(X0)):
    plt.text(X0[i], f(X0[i])+ (-1)**(i+1) * 0.5, str(iterations_to_converge[i]), ha="center", va="center", color="black")
plt.legend()
plt.title("Local convergence of Newton's method")
plt.show()
