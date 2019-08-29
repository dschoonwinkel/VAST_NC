#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np

data_a = [[1,2,5], [5,7,2,2,5]]
data_b = [[6,4,2], [1,2,5,3,2]]

ticks = ['A', 'B']

plt.figure()

bpl = plt.boxplot(data_a, positions=[1, 2], sym='')
bpr = plt.boxplot(data_b, positions=[1.5, 2.5], sym='')

plt.tight_layout()
plt.xlim([0, 3])
plt.show()