import numpy as np
import numpy.linalg as la
import matplotlib.pyplot as plt
#calculate A1, A2
A1 = np.zeros((200, 200))
A2 = np.zeros((200, 200))

for i in range(200):
    A1[i][i] = -2
    A2[i][i] = -(2 + Ke[i])
    if i == 0:
        A1[i][i + 1] = 1
        A2[i][i + 1] = 1

    if i > 0 and i <199:
        A1[i][i - 1] = 1
        A1[i][i + 1] = 1
        A2[i][i - 1] = 1
        A2[i][i + 1] = 1

    if i == 199:
        A1[i][i - 1] = 1
        A2[i][i - 1] = 1

val1, vec1 = la.eig(A1)
val2, vec2 = la.eig(A2)

val1_low5freq = np.array(sorted(list(val1), key = abs)[0:5])
val2_low5freq = np.array(sorted(list(val2), key = abs)[0:5])

vec1_low5freq = np.zeros((200, 5))
vec2_low5freq = np.zeros((200, 5))

for i in range(5):
    index1 = np.where(val1 == val1_low5freq[i])[0][0]
    vec1_low5freq[ : , i] = vec1[ : , index1]

    index2 = np.where(val2 == val2_low5freq[i])[0][0]
    vec2_low5freq[ : , i] = vec2[ : , index2]

x = list(range(200))
plt.plot(x, vec1_low5freq)
val1_plot = plt.gca()

plt.figure()
plt.plot(x, vec2_low5freq)
val2_plot = plt.gca()








import numpy as np
import numpy.linalg as la

mat = np.zeros((5, 5))

mat[ : , 0] = np.array([0, 0.25, 0.25, 0.25, 0.25])
mat[ : , 1] = np.array([0.5, 0, 0, 0.5, 0])
mat[ : , 2] = np.array([0.5, 0.5, 0, 0, 0])
mat[ : , 3] = np.array([0, 0, 0.5, 0, 0.5])
mat[ : , 4] = np.array([0, 1, 0, 0, 0])

prob = np.array([1, 0, 0, 0, 0])
for i in range(50):
    prob = mat @ prob
print(prob)









import numpy as np
import numpy.linalg as la

M = np.zeros((4, 4))
M[ : , 0] = np.array([0.7, 0.1, 0.1, 0.1])
M[ : , 1] = np.array([0, 0.5, 0.4, 0.1])
M[ : , 2] = np.array([0, 0.1, 0.8, 0.1])
M[ : , 3] = np.array([0, 0, 0, 1])

x = np.array([1, 0, 0, 0])
hours = 0
while x[3] <= 0.9:
    x = M@x
    hours += 1
