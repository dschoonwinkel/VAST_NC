cd ..
pwd
echo "Running $1 times"
sudo make
#Start the gateway
./VASTreal_console 0 1 &
sleep 2
for ((i=1; i<$1; i++));
do
		#start the other nodes in the P2P network
		./VASTreal_console $1 1 >> /dev/null &
		sleep 1
		# echo $i
done

# cd scripts
