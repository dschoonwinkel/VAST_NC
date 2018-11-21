cd ..
pwd
echo "Running $1 times"
for ((i=0; i<$1; i++));
do
		sudo ./random_walkertalker >> /dev/null &
		sleep 2
		# echo $i
done

# cd scripts