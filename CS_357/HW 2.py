print(minifloats)

import numpy as np
outputs = []
for i in minifloats:
    m = 0
    power = 3
    exponent = 0
    for j in i[1:5]:
        exponent += j * 2**power
        power -= 1
    power = 1
    for k in i[5:]:
        m += k * (1 / 2**power)
        power += 1
    exponent -= 8
    if exponent > 0:
        outputs.append((-1) ** i[0] *(1 + m) * 2**exponent)
    else:
        outputs.append((-1) ** i[0] *(1 + m) * (1 / 2**(-1 * exponent)))
