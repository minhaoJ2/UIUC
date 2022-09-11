def numberOfBlocks(matrix):
    lower = [-1] * len(matrix[0])
    upper = [-1] * len(matrix[0])
    min_distance = 1000000
    index = -1
    m, n = len(matrix[0]), len(matrix)
    for i in range(m):
        distance = 0
        for j in range(n - 1, -1, -1):
            if matrix[j][i] == 1:
                if lower[i] == -1:
                    lower[i] = j
                    upper[i] = j
                    if j < min_distance:
                        min_distance = j
                        index = i
                else:
                    upper[i] = j
    count = 0
    for i in range(m):
        if lower[i] == -1:
            continue
        elif lower[i] == upper[i]:
            for j in range(n - lower[i], n - lower[i] + min_distance):
                if matrix[j][i] == 2:
                    count += 1
        else:
            for j in range(n - lower[i], n - lower[i] + min_distance):
                if matrix[j][i] == 2:
                    count += 1
            for k in range(upper[i], min(upper[i] + min_distance, lower[i])):
                if matrix[k][i] == 2:
                    count += 1
    return count
