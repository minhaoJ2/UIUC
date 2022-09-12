import numpy as np

data_sum = 0.0
np.sort(data)
le = len(data)

if (le % 2) == 1:
    for i in range(int(le / 2)):
        data_sum = data_sum + (data[i] + data[le - i - 1])
    data_sum = data_sum +  data[int(le / 2) + 1]
if (len(data) % 2) == 0:
    for i in range(int(le / 2)):
        data_sum = data_sum + (data[i] + data[le - i - 1])







#a = 2**53 + 1
k = 0
i = 0
while True:
    if a == a + 10 ** -i:

        k = -i + 1
        break
    i+= 1
