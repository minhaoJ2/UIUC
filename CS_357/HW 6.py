import numpy as np

normWaldo = np.linalg.norm(waldo_image)
width = len(waldo_image[0])
height = len(waldo_image)
min_diff = 100000
top_left = (0, 0)
for i in range(len(group_image) - height):
    for j in range(len(group_image[0]) - width):
        norm = np.linalg.norm(waldo_image - group_image[i : i + height, j : j + width ])
        if norm < min_diff:
            min_diff = norm
            top_left = (i, j)
#print(normWaldo, len(waldo_image), len(waldo_image[0]))








import matplotlib.pyplot as plt
import numpy as np

# It may be helpful to complete this function
def norm(x, p):
    """Computes a p-norm.

    Args:
        x (ndarray): input array
        p (int or float): order of the norm

    Returns:
        (float): the p-norm of the array
    """
    return np.sum(abs(x)**p, axis = 0)**(1 / p)

ps = [1, 2, 5, 0.5]
phi = np.linspace(0, 2 * np.pi, 500)
unit_circle = np.array([np.cos(phi), np.sin(phi)])

plt.figure()
y1 = norm(unit_circle, ps[0])
norY1 = unit_circle / y1
plt.plot(r * norY1[0], r * norY1[1])
fig_1 = plt.gca()

plt.figure()
y2 = norm(unit_circle, ps[1])
norY2 = unit_circle / y2
plt.plot(r * norY2[0], r * norY2[1])
fig_2 = plt.gca()

plt.figure()
y3 = norm(unit_circle, ps[2])
norY3 = unit_circle / y3
plt.plot(r * norY3[0], r * norY3[1])
fig_3 = plt.gca()

plt.figure()
y4 = norm(unit_circle, ps[3])
norY4 = unit_circle / y4
plt.plot(r * norY4[0], r * norY4[1])
fig_4 = plt.gca()





import numpy as np

ratings = np.zeros(k)

for i in range(k):
    for j in range(n):
        ratings[i] += sum(get_friend_prefs(j) * get_movie_attr(i)) / m

top = np.where(ratings == np.amax(ratings))[0][0]
print(top)










import numpy as np

prefs = np.zeros((m,n))

movieAttrs = np.zeros((k, m))
for i in range(k):
    movieAttrs[i] = get_movie_attr(i)

for j in range(n):
    prefs[:, j] = np.linalg.solve(movieAttrs, get_ratings(j))
print(prefs)
