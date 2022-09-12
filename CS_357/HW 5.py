import numpy as np

def genRandomVect():
    # define return numpy array
    x =  np.random.choice(100, 50, replace=False)
    return(x)






import numpy as np
import matplotlib.pyplot as plt

def calculate_pi(x, y):
    result = 0.0
    for i in range(len(x)):
        if ((x[i]**2 + y[i]**2)**0.5 <= 1):
            result += 1
    return float(result / len(x)) * 4

pi = []
for i in range(7):
    x =  xs[:10**i]
    y =  ys[:10**i]
    pi.append(calculate_pi(x, y))
pi = np.array(pi)

plt.title('a')
plt.xlabel('x')
plt.ylabel('y')
err = abs(pi - np.pi * np.ones(7))
n = [1, 10, 100, 1000, 10000, 100000, 1000000]
plt.loglog(err, n)
# Save plot for grading
plot = plt.gca()




import numpy as np
import matplotlib
import matplotlib.pyplot as plt


def duplicate_birthdays(n):
    # Generate 1000 simulations of rooms (with the birthdays of n people in each room)
    samples = []
    dul = 0
    for i in range(1000):
        samples.append(genroom(n))
    # Compute the number of rooms with duplicate birthdays.
    for i in samples:
        #date = list(range(1, 356))
        k = list(i)
        for j in k:
            #if j in date:
                #date.remove(j)
        #if 365 - len(date) > n:
            if k.count(j) > 1:
                dul += 1
                break
    # Returns the number of rooms with duplicate birthdays
    return dul

# Part 1
prob_n = []
for n in range(2, 101):
    # call function duplicate_birthdays(n)
    result = duplicate_birthdays(n) / 1000.0
    # update the array prob_n
    prob_n.append(result)
prob_n = np.array(prob_n)

# Part 2
# Estimate perc_50
num = 2
for i in prob_n:
    if (i > 0.5):
        perc_50 = num
        break
    num += 1

# Part 3
# Plot prob_n
plt.title('a')
plt.xlabel('x')
plt.ylabel('y')
plt.plot(list(range(2, 101)), prob_n)
# Save plot for grading
plot = plt.gca()
