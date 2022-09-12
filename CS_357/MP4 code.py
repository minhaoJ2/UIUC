import numpy as np
from matplotlib import colors
import matplotlib.pyplot as plt

xvec1 = np.array([0.5, 0.2, 0.7, 1.0, 0.0, 1.0])

xvec2 = np.array([0.5, 0.7, 0.0, 1.0, 0.7, 0.2])

def get_change(xvec1, xvec2):
    result = 0
    for i in range(len(xvec1)):
        diff = abs(xvec1[i] - xvec2[i])
        if result < diff:
            result = diff

    return result





import numpy as np

def dfunc(x, dx):
    # ADD CODE HERE
    df = np.zeros(len(x))
    val = func(x)
    for i in range(len(x)):
        # xi = x
        # xi[i] += dx
        x[i] += dx
        #print(xi)
        df[i] = (func(x) - val) / dx
        x[i] -= dx
        print(df)
    return df

print(xvec)
print(dfunc(xvec, 0.1))
print(df_exact)








import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import convolve2d

identity_filter = np.array([[0, 0, 0], [0, 1.0, 0], [0, 0, 0]])

blur_filter = np.ones((3, 3)) / 9

sharpen_filter = 2*identity_filter - blur_filter

identity_image = convolve2d(image, identity_filter, mode = 'same')

blurred_image = convolve2d(image, blur_filter, mode = 'same')

sharpened_image = convolve2d(image, sharpen_filter, mode = 'same')





import numpy as np
import math

def create_kernel(rmin):
    N = 2 * math.floor(rmin) + 1
    H = np.zeros((N, N))
    center = N / 2.0
    for i in range(N):
        for j in range(N):
            xdiff = abs((0.5 + i) - center)
            ydiff = abs((0.5 + j) - center)
            distance = (xdiff**2 + ydiff**2)**0.5
            H[j][i] = max(0, rmin - distance)

    return H
print(Htest)
print(create_kernel(1.5))











import numpy as np
from scipy.signal import convolve2d
from matplotlib import colors
import matplotlib.pyplot as plt
import math

def create_kernel(rmin,nelx,nely):
    N = 2 * math.floor(rmin) + 1
    H = np.zeros((N, N))
    center = N / 2.0
    for i in range(N):
        for j in range(N):
            xdiff = abs((0.5 + i) - center)
            ydiff = abs((0.5 + j) - center)
            distance = (xdiff**2 + ydiff**2)**0.5
            H[j][i] = max(0, rmin - distance)
    H1 = convolve2d(np.ones((nelx, nely)), H, mode = 'same')
    return H, H1

def filter_design_variable(xvec,H,H1):
    shapex = len(H1)
    shapey = len(H1[0])
    X = xvec.reshape(H1.shape)
    return (convolve2d(X, H, mode = 'same') / H1).reshape(shapex * shapey)

H, H1 = create_kernel(1.5, nelx, nely)
xfilt = filter_design_variable(xvec, H, H1)

image_plot = plt.imshow(-xfilt.reshape(nelx,nely).T, cmap='gray',interpolation='none',norm=colors.Normalize(vmin=-1,vmax=0))














import numpy as np
from matplotlib import colors
import matplotlib.pyplot as plt



x0 = xvec
x1 = xvec
count = 0
while True:
    f = func(x0)
    df = dfunc(x0)
    xnew = optimizer(xvec,f,df)
    x1 = filter_design_variable(xnew)
    diff = get_change(x0, x1)
    count += 1
    if (count >= maxiter or diff < tol):
        break
    x0 = x1
xnew = x1
image_plot = plt.imshow(-xnew.reshape(nelx,nely).T, cmap='gray',interpolation='none',norm=colors.Normalize(vmin=-1,vmax=0))










import numpy as np
from matplotlib import colors
import matplotlib.pyplot as plt

nelx = 60
nely = 30
image1 = topopt(60, 30, 0.5, 60, 1e-3, 1.5, 2)
image_plot_1 = plt.imshow(-image1.reshape(nelx,nely).T, cmap='gray',interpolation='none',norm=colors.Normalize(vmin=-1,vmax=0))


image2 = topopt(60, 30, 0.5, 60, 1e-3, 1.5, 1)
image_plot_2 = plt.imshow(-image2.reshape(nelx,nely).T, cmap='gray',interpolation='none',norm=colors.Normalize(vmin=-1,vmax=0))


image3 = topopt(60, 30, 0.5, 60, 1e-3, 2.5, 1)
image_plot_3 = plt.imshow(-image3.reshape(nelx,nely).T, cmap='gray',interpolation='none',norm=colors.Normalize(vmin=-1,vmax=0))


image4 = topopt(60, 30, 0.5, 60, 1e-3, 4.0, 1)
image_plot_4 = plt.imshow(-image4.reshape(nelx,nely).T, cmap='gray',interpolation='none',norm=colors.Normalize(vmin=-1,vmax=0))
