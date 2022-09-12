import numpy as np
import numpy.linalg as la

x = la.solve(A, b)

count = 0
temp = la.cond(A)

print(temp)

while temp >= 5:
    temp = temp / 10
    count += 1

print(count)

correct_digits = 15 - count






import numpy as np
import numpy.linalg as la

err_xhat = xtrue - xhat

rel_err_xhat = la.norm(err_xhat, 2) / la.norm(xtrue, 2)


btrue = np.dot(A, xtrue)
bhat = np.dot(A, xhat)
err_bhat = btrue - bhat
rel_err_Axhat = la.norm(err_bhat, 2) / la.norm(btrue, 2)

cond_A = la.cond(A)

bound_rel_err_Axhat = rel_err_xhat * cond_A
print(bound_rel_err_Axhat)











import numpy as np
import numpy.linalg as la

size = len(components)
A = np.zeros((size, size))
b = np.zeros(size)
count = 0

for i in test_data:
    temp = {}
    for j in test_data[i]:
        temp[j[0]] = j[1]
    row = []
    for k in components:
        row.append(temp[k])
    A[count, : ] = row
    b[count] = temp['EnergyConsumed']
    count += 1

power_usage = la.solve(A, b)
