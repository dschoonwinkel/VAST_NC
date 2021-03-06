import re
import numpy as np

NET_MODEL_STRINGS = ['', 'net_emu', 'net_ace', 'net_udp', 'net_udpNC']
NET_MODEL_STRINGS_THESIS = ['', 'net_emu', 'TCP', 'UDP', 'UDPNC']
NET_ACE = 2
NET_UDP = 3
NET_UDPNC = 4

PLATFORM_STRINGS = ['Mininet', 'Docker', 'realOVS']

def parseFilenameLabel(LABEL_string):
	NET_MODEL_str = re.search(r'net_[a-zA-Z]+', LABEL_string).group(0)
	NET_MODEL = NET_MODEL_STRINGS.index(NET_MODEL_str)
	# print("NET_MODEL:", NET_MODEL)

	NODECOUNT_str = re.search(r'NODES\d+', LABEL_string).group(0)
	NODECOUNT = NODECOUNT_str[len('NODES'):]
	# print("NODECOUNT:", NODECOUNT)

	BW_str = re.search(r'BW\d+\.*\d*', LABEL_string).group(0)
	BW = BW_str[len('BW'):]
	# print("BW:", BW)

	DELAY_str = re.search(r'DELAY\d+', LABEL_string).group(0)
	DELAY = DELAY_str[len('DELAY'):]
	# print("DELAY:", DELAY)

	LOSS_str = re.search(r'LOSS\d+', LABEL_string).group(0)
	LOSS = LOSS_str[len('LOSS'):]
	# print("LOSS:", LOSS)

	STEPS_str = re.search(r'STEPS\d+', LABEL_string).group(0)
	STEPS = STEPS_str[len('STEPS'):]
	# print("STEPS:", STEPS)

	PLATFORM_str = re.search(r'((Mininet)|(Docker)|(realOVS)){1}', LABEL_string).group(0)
	# print(PLATFORM_str)
	PLATFORM = PLATFORM_STRINGS.index(PLATFORM_str)+1
	# print("PLATFORM:", PLATFORM)


	DATESTAMP_str_re = re.search(r'\d+_\d+_\d+-\d+:\d+', LABEL_string)

	if DATESTAMP_str_re == None:
		DATESTAMP_str_re = re.search(r'\d+_\d+_\d+-\d+_\d+', LABEL_string)

	DATESTAMP_str = DATESTAMP_str_re.group(0)
		


	return [NET_MODEL, int(NODECOUNT), float(BW), int(DELAY), int(LOSS), int(STEPS), PLATFORM], DATESTAMP_str

def parseFilenameNodesTimesteps(LABEL_string):
	return parseFilenameLabel(LABEL_string)[0][1], parseFilenameLabel(LABEL_string)[0][5]

def finite_mean(array):
	where_isfinite = np.isfinite(array)
	return np.mean(array[where_isfinite])

def finite_max(array):
	where_isfinite = np.isfinite(array)
	return np.max(array[where_isfinite])