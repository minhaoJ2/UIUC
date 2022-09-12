import pandas as pd
import matplotlib.pyplot as plt

tumor_data = pd.io.parsers.read_csv("breast-cancer-train.dat", header=None, names=labels)
user_column = labels[10]
tumor_data[user_column].hist()

plt.xlabel("x")
plt.ylabel("y")
plt.title("z")




 
import pandas as pd
import numpy as np

tumor_data = pd.io.parsers.read_csv("breast-cancer-train.dat", header=None, names=labels)

temp = tumor_data[labels[1]]
lenth = len(temp)
#print(temp, lenth)

b = np.zeros(lenth)
for i in range(lenth):
    if temp[i] == 'M':
        b[i] = 1.0
        continue
    b[i] = -1.0







import pandas as pd
import numpy as np

tumor_data = pd.io.parsers.read_csv("breast-cancer-train.dat", header=None, names=labels)

A_linear = []
for i in subset_labels:
    A_linear.append(tumor_data[i].values)
A_linear = np.array(A_linear).T











import pandas as pd
import numpy as np

tumor_data = pd.io.parsers.read_csv("breast-cancer-train.dat", header=None, names=labels)

A_quad = []
for i in subset_labels:
    A_quad.append(tumor_data[i].values)

for i in subset_labels:
    A_quad.append(tumor_data[i].values**2)

for i in range(len(subset_labels) - 1):
    for j in range(i + 1, len(subset_labels)):
        value = tumor_data[subset_labels[i]].values * tumor_data[subset_labels[j]].values
        A_quad.append(value)
A_quad = np.array(A_quad).T








import numpy as np
import numpy.linalg as la
import pandas as pd
import matplotlib.pyplot as plt

# Load the data
validate_data = pd.io.parsers.read_csv("breast-cancer-validate.dat", header=None, names=labels)

u1, sigma1, vt1 = la.svd(A_linear, full_matrices=False)
weights_linear = vt1.T @ la.solve(np.diag(sigma1), u1.T @ b)


u2, sigma2, vt2 = la.svd(A_quad, full_matrices=False)
weights_quad = vt2.T @ la.solve(np.diag(sigma2), u2.T @ b)


A_val_linear = []
for i in labels[2:32]:
    A_val_linear.append(validate_data[i].values)
A_val_linear = np.array(A_val_linear).T


A_val_quad = []
for i in subset_labels:
    A_val_quad.append(validate_data[i].values)

for i in subset_labels:
    A_val_quad.append(validate_data[i].values**2)

for i in range(len(subset_labels) - 1):
    for j in range(i + 1, len(subset_labels)):
        value = validate_data[subset_labels[i]].values * validate_data[subset_labels[j]].values
        A_val_quad.append(value)
A_val_quad = np.array(A_val_quad).T


fn_linear = 0.0
fp_linear = 0.0
fn_quad = 0.0
fp_quad = 0.0
linear = A_val_linear @ weights_linear
quad = A_val_quad @ weights_quad

column = validate_data[labels[1]]
temp = np.zeros(len(column))
for i in range(len(column)):
    if column[i] != 'M':
        temp[i] = -1.0
    else:
        temp[i] = 1.0

for i in range(len(temp)):
    if temp[i] == 1:
        if linear[i] < 0:
            fn_linear += 1
        if quad[i] < 0:
            fn_quad += 1
    if temp[i] == -1:
        if linear[i] > 0:
            fp_linear += 1
        if quad[i] > 0:
            fp_quad += 1

bar_graph(fp_linear, fn_linear, fp_quad, fn_quad)
