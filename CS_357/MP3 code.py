import numpy as np

team_names = []    # list team names here
A = np.zeros((30, 30))         # the rows and columns should have the same order as your team_names

for i in home_team:
    if i not in team_names:
        team_names.append(i)
print(team_names)

for index in range(len(home_team)):
    diff = home_score[index] -away_score[index]
    ht = home_team[index]
    at = away_team[index]
    if (diff > 0):
        A[team_names.index(ht)][team_names.index(at)] += diff
    else:
        A[team_names.index(at)][team_names.index(ht)] += -1*diff





import numpy as np

def transition(A):
    # construct the matrix M
    M = A
    for i in range(len(A)):
        col = A[ : , i]
        #print(col)
        col_sum = sum(col)
        if col_sum == 0:
            M[ : , i] = np.ones(len(A)) *(1 / len(A))
            continue
        M[ : , i] = col / (col_sum * 1.0)
        #print(sum(M[ : , i]))
    return M
M = A
M = transition(A)
#print(M)





import numpy as np
import scipy.linalg as la

def power_iteration(M,tol):
    xstart = np.ones(len(M[0])) / len(M[0])
    diff = 1
    while diff > tol:
        temp = M @ xstart
        diff = la.norm((temp - xstart), 2)
        xstart = temp
    # compute things!
    return xstart

xstar = power_iteration(M,tol)






import numpy as np
import matplotlib.pyplot as plt

result = power_iteration(M, tol)
dic = {}
index = 0
for i in result:
    dic[i] = team_names[index]
    index += 1
result.sort()
team_ranks = []
print(result)
for i in result[::-1]:
    team_ranks.append(dic[i])

## Horizontal bar plot for the percentage of fan support for each team
## (following the order of team_ranks - "best" to "worst" team)
bar_height = result[::-1]
y_label = team_ranks


f, axs = plt.subplots(1,1,figsize=(8,10))
plt.subplots_adjust(left=0.25)
y_axis = range(len(team_ranks))
plt.barh(y_axis, bar_height)
plt.yticks(y_axis, y_label)
plt.title(season_name + ' season NBA teams ranking based on first 150 games')
axs.invert_yaxis()













import numpy as np
import matplotlib.pyplot as plt

# Simply run the following code.

for i in range(3):
    f, axs = plt.subplots(figsize=(7, 8))
    plt.subplots_adjust(left=0.25)
    y_axis = range(len(y_label[i]))
    axs.barh(y_axis, bar_height[i])
    left_dots_x = bar_height[i] + 0.01
    right_dot_x = max(bar_height[i]) * 2
    plt.yticks(y_axis, y_label[i])
    for j, label in enumerate(y_label[i]):
        axs.plot([left_dots_x[j], right_dot_x],[y_axis[j], final_rankings[i][y_label[i][j]]])
    axs.scatter([right_dot_x for j in range(20)], y_axis, label="Ranking at the end of season")
    axs.scatter(left_dots_x, y_axis, label="Ranking using PageRank")
    plt.legend()
    axs.set_title(season_name[i] + ' Premier League ranking based on first 80 games')
    axs.invert_yaxis()
plt.show()
