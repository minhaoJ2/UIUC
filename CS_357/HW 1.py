def consecutive_elem_ratio(seq):
    cop = seq[:]
    seq.pop(0)
    for i,v in enumerate(cop):
        if i == len(cop) - 1:
            break
        temp.append(seq[i + 1] / v)
        #seq[i] = cop[i + 1] / v
    seq = temp[:]
    print(seq)

fibonnaci_seq = [1, 1, 2, 3, 5, 8, 13, 21]
consecutive_elem_ratio(fibonnaci_seq)
print(fibonnaci_seq)



import numpy as np
Ax = np.zeros(shape[0])
for i in list(A.keys()):
    for j in list(A[i].keys()):
        Ax[i] += x[j] * A[i][j]