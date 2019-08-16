#/bin/bash

grep -r "AddressSanitizer" *.txt | uniq -c

grep -r "Subsc" *.txt | uniq -c
grep -r "Subsc" *.txt | sort | uniq | wc

grep -r "isJoined == false" *.txt
grep -r "remove" *.txt | sort | uniq
# grep -r "VASTMatcher::removeFailedSubscribers" *.txt
# grep -r "VASTClient::removeGhosts" *.txt
grep -r "No connection to" *.txt

echo "Number of forwarded RELAY_QUERY's"
grep -r "meant for" *.txt | wc 

grep -r "discarding packet to " *.txt
# grep -r "Empty RLNC" *.txt | sort | uniq -c
grep -r "removeExpiredMatchers" *.txt

grep -r -B 1 "stacked_packets_received" *.txt
grep -r "stacked_packets_perc" *.txt
