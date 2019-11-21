#!/usr/bin/env python3
import numpy as np
import scipy.stats as st
import sys
from plot_result_utils import NET_MODEL_STRINGS
from plot_results_summary_utils import *
import re
from os.path import expanduser


ax1 = plot.subplot(111)
x = [np.random.randn(10), np.random.randn(10)]
ax1.boxplot(x)

ax1_twiny = ax1.twiny()

x = [np.random.randn(10), np.random.randn(10)]
plot.boxplot(x, positions=[2,3])

ax1_twiny.set_xlim((1,4))


plot.show()

