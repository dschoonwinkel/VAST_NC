import subprocess

processes = list()

for i in range(5):
	print("Starting %d" %i)
	processes.append(subprocess.Popen(["watch", "ls", "-al"], cwd="../", stdout=subprocess.PIPE))

try:
	processes[0].wait()	
except KeyboardInterrupt:
	try:
		for i in range(5):
			print("Stopping %d" %i)
			processes[i].terminate()
	except OSError:
		pass


print("I'm done!")