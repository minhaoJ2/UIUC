import numpy as np
import matplotlib.pyplot as plt

brackets = []
gs = (np.sqrt(5) - 1) / 2
m1 = a + (1 - gs) * (b - a)
m2 = a + gs * (b - a)
f1 = f(m1)
f2 = f(m2)
diff = b - a
# Begin your modifications below here

while abs(a - b) > 10**-5:
    if f1 > f2:
        a = m1
        m1 = m2
        f1 = f2
        diff = b - a
        m2 = a + gs * diff
        f2 = f(m2)

    else:
        b = m2
        m2 = m1
        f2 = f1
        diff = b - a
        m1 = a + (1 - gs) * diff
        f1 = f(m1)
    brackets.append([a, m1, m2, b])

# End your modifications above here

# Plotting code below, no need to modify
x = np.linspace(-10, 10)
plt.plot(x, f(x))

brackets = np.array(brackets)
names=['a', 'm1', 'm2', 'b']
for i in range(4):
    plt.plot(brackets[:, i], 3*np.arange(len(brackets)), 'o-', label=names[i])
plt.legend()









# import numpy as np
# import numpy.linalg as la
# import scipy
# import matplotlib.pyplot as plt

# def f(r):
#     x, y = r
#     return 3 +((x**2)/8) + ((y**2)/8) - np.sin(x)*np.cos((2**-0.5)*y)

# def gradient(r):
#     x, y = r
#     return np.array([x / 4 - np.cos(x) * np.cos((2**0.5 / 2) * y), y / 4 + (2**0.5 / 2) * np.sin(x) * np.sin((2**0.5 / 2) * y)])

# def hessian(r):
#     x, y = r
#     # return np.array([[0.25 + np.sin(x) * np.cos((2**0.5 / 2) * y), x / 4 + (2**0.5 / 2) * np.cos(x) * np.sin((2**0.5 / 2) * y)], [y / 4 + (2**0.5 / 2) * np.cos(x) * np.sin((2**0.5 / 2) * y), 0.25 + 0.5 * np.sin(x) * np.cos((2**0.5 / 2) * y)]])
#     h11 = 0.25 + np.sin(x)*np.cos(np.sqrt(2)*y/2)
#     h12 = (np.sqrt(2)/2)*np.cos(x)*np.sin(np.sqrt(2)*y/2)
#     h21 = (np.sqrt(2)/2)*np.cos(x)*np.sin(np.sqrt(2)*y/2)
#     h22 = 0.25 + 0.5*np.sin(x)*np.cos(np.sqrt(2)*y/2)
#     return np.array([[h11,h12],[h21,h22]])

# y1 = [r_init]
# def newtons_method(r_init):
#     r_new = r_init
#     r_prev = r_init
#     count = 0
#     while True:
#         r_prev = r_new
#         s = -la.solve(hessian(r_prev), gradient(r_prev))
#         r_new = r_prev + s
#         y1.append(r_new)
#         count += 1
#         if la.norm(r_prev - r_new) < stop:
#         #if count == 3:
#             break
#     return r_new, count


# def obj_func(alpha, x, s):
#     return f(x + alpha * s)

# y2 = [r_init]
# def steepest_descent(r_init):
#     r_new = r_init
#     r_prev = r_init
#     count = 0
#     while True:
#         r_prev = r_new
#         s = -gradient(r_prev)
#         # def obj_func(alpha):
#         #     return f(alpha * s + r_prev)
#         # alpha = scipy.optimize.minimize_scalar(obj_func).x
#         alpha = scipy.optimize.minimize_scalar(obj_func, args=(r_prev, s)).x
#         r_new = r_prev + alpha * s
#         count += 1
#         y2.append(r_new)
#         if count == 8:
#             break
#     return r_new, count

# iteration_count_newton = 0
# r_newton, iteration_count_newton = newtons_method(r_init)
# iteration_count_newton = 3
# print(iteration_count_newton)

# iteration_count_sd = 0
# r_sd, iteration_count_sd = steepest_descent(r_init)
# iteration_count_sd = 8

# y1 = np.array(y1) - r_newton
# y2 = np.array(y2) - r_sd
# for i in range(len(y1)):
#     y1[i] = la.norm([y1[i][0], y1[i][1]])
# print(y1[:,0])

# for i in range(len(y2)):
#     y2[i] = la.norm([y2[i][0], y2[i][1]])
# print(y2[:,0])


# x1 = [0, 1, 2, 3]
# x2 = [0, 1, 2, 3, 4, 5, 6, 7, 8]

# #plt.plot(x1, np.log(y1[: , 0]), label = "newton")
# #y = [2.50923687e-01, 6.86667605e-02, 3.29527461e-04, 6.65244208e-06, 1.24693833e-09, 0.00000000e+00]
# #y = [-1.38198235, -5.84446293,-13.61814674, -29.14051682]
# #y = [-1.38198235, -5.84446293,-13.61814674, float('-inf')]
# y = [-1.38205953,  -5.84463144, -13.61903684, float('-inf')]
# yy = [-1.38264587, -3.02543986, -4.46892166, -6.16356383, -7.61143482, -9.31107708, -10.79736385, -12.52257877,
#       float('-inf')]
# print(np.log(y1[: , 0]))
# print(np.log(y2[: , 0]))
# plt.plot(x1, y, label = "newton")
# #plt.plot(x2, yy, label = "sd")
# plt.plot(x2, np.log(y2[: , 0]), label = "sd")
# plt.legend()
# plt.xlabel("x")
# plt.ylabel("y")
# plt.title("GLHF")

import numpy as np
import numpy.linalg as la
import scipy.optimize as sopt
import matplotlib.pyplot as plt

def f(r):
    x, y = r
    return 3 +((x**2)/8) + ((y**2)/8) - np.sin(x)*np.cos((2**-0.5)*y)

def gradf(x,y):
    gf1 = 0.25*x - np.cos(x)*np.cos(np.sqrt(2)*y/2)
    gf2 = 0.25*y + (np.sqrt(2)/2)*np.sin(x)*np.sin(np.sqrt(2)*y/2)
    return np.array([gf1, gf2])

def H(x,y):
    h11 = 0.25 + np.sin(x)*np.cos(np.sqrt(2)*y/2)
    h12 = (np.sqrt(2)/2)*np.cos(x)*np.sin(np.sqrt(2)*y/2)
    h21 = (np.sqrt(2)/2)*np.cos(x)*np.sin(np.sqrt(2)*y/2)
    h22 = 0.25 + 0.5*np.sin(x)*np.cos(np.sqrt(2)*y/2)
    return np.array([[h11,h12],[h21,h22]])

def f_line(alpha,s,x):
    fnew = f(x + alpha*s)
    return fnew
##########################################
r_newton = np.zeros(shape = (2,))


x = r_init[0]
y = r_init[1]
pltn = r_init.copy()
print(x,y)

iteration_count_newton = 0
save_r_init = r_init.copy()

while (la.norm(gradf(x,y),2) >= stop):
    s = la.solve(H(x,y), -gradf(x,y))
    r_init += s
    x = r_init[0]
    y = r_init[1]
    iteration_count_newton += 1

r_newton[0] = x
r_newton[1] = y
print(r_newton)
############################################
r_sd = np.zeros(shape = (2,))
m = save_r_init[0]
n = save_r_init[1]

plts =  save_r_init.copy()

iteration_count_sd = 0
while (la.norm(gradf(m,n),2) >= stop):
    s =-gradf(m,n)
    alpha_opt = sopt.minimize_scalar(f_line, args = (s,save_r_init)).x
    save_r_init += alpha_opt*s
    m = save_r_init[0]
    n = save_r_init[1]
    iteration_count_sd += 1

r_sd[0] = m
r_sd[1] = n
##############################################
new_iter_newton = iteration_count_newton+1
new_iter_sd = iteration_count_sd+1
################################################

error_newton = np.zeros(shape = (new_iter_newton,))

x = pltn[0]
y = pltn[1]

new_iter_newton_array = np.array([i for i in range(new_iter_newton)])
error_newton[0] = la.norm(pltn-r_newton,2)

count = 0
while (la.norm(gradf(x,y),2) >= stop):
    s = la.solve(H(x,y), -gradf(x,y))
    pltn += s
    x = pltn[0]
    y = pltn[1]
    a = la.norm(pltn-r_newton,2)
    count += 1
    error_newton[count] = a
error_newton = np.array([np.log(j) for j in error_newton])
print(error_newton)#.append(la.norm(r_newton-r_newton,2))
###########################
error_sd = np.zeros(shape = (new_iter_sd,))

m = plts[0]
n = plts[1]

new_iter_sd_array = np.array([i for i in range(new_iter_sd)])
error_sd[0] = la.norm(plts-r_sd,2)

count1 = 0
while (la.norm(gradf(m,n),2) >= stop):
    s =-gradf(m,n)
    alpha_opt = sopt.minimize_scalar(f_line, args = (s,plts)).x
    plts += alpha_opt*s
    m = plts[0]
    n = plts[1]
    count1 += 1
    b = la.norm(plts-r_sd,2)
    error_sd[count1] = b
error_sd = np.array([np.log(j) for j in error_sd])
print(error_sd)
#########################################
plt.figure()
plt.plot(new_iter_newton_array,error_newton)
plt.plot(new_iter_sd_array,error_sd)
plt.title('Newton and Steepest Descent')
plt.legend()
plt.xlabel('x')
plt.ylabel('y')
plt.show()
plot = plt.gca()
