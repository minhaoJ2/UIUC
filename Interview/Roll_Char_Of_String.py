""
Roll the first roll[i] characters of the string
s = "bca", roll = [1,2,3], output = "eeb"
""

import collections
def findRollOut(s, roll, n):
    # Your code goes here
    prefix = [0] * (n+1)
    a_to_z = [chr(i) for i in range(97, 97 + 26)]
    for i in range(len(roll)):
        prefix[0] += 1
        prefix[roll[i]] -= 1
    for i in range(1, n):
        prefix[i] += prefix[i-1]
    res = "" 
    for i in range(n):
        num = ord(s[i]) + prefix[i] - 97
        num = num % 26
        res += a_to_z[num]
    return res
