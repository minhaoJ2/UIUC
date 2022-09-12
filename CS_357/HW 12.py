import numpy as np
import matplotlib.pyplot as plt
from scipy.misc import imread



total = 0
temp = 0
(m,n) = image.shape
aim = min(m,n)

U,Sigma,Vt = np.linalg.svd(image, full_matrices=False)

for i in range(aim):
    temp += Sigma[i]

for i in range(len(Sigma)):
    total += Sigma[i]
    if total > f * temp:
        k = i+1
        break

image_compressed = U[:, :k] @ np.diag(Sigma[:k]) @ Vt[:k, :]
