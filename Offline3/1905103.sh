#!/bin/bash

>"scratch/Offline3/bottle_neck_datarate_vs_throughput_1.txt"

>"scratch/Offline3/bottle_neck_datarate_vs_throughput_2.txt"

>"scratch/Offline3/packet_loss_rate_vs_throughput_1.txt" 

>"scratch/Offline3/packet_loss_rate_vs_throughput_2.txt"

>"task1.pdf"

#varying bottleLinkRate

#TcpHighSpeed
for((i=1;i<=10;i++))
do
    r=$(($i*30))
    ./ns3 run "scratch/1905103.cc --bottleLinkRate=$r --packetLossExp=6 --tcpType=1 --plot=1"
done

#TcpAdaptiveReno
for((i=1;i<=10;i++))
do
    r=$(($i*30))
    ./ns3 run "scratch/1905103.cc --bottleLinkRate=$r --packetLossExp=6 --tcpType=2 --plot=1"
done

#varying packetLossRate
for((i=2;i<=6;i++))
do
    ./ns3 run "scratch/1905103.cc --bottleLinkRate=50 --packetLossExp=$i --tcpType=1 --plot=2"
done

for((i=2;i<=6;i++))
do
    ./ns3 run "scratch/1905103.cc --bottleLinkRate=50 --packetLossExp=$i --tcpType=2 --plot=2"
done

>"scratch/Offline3/flow1.txt"
>"scratch/Offline3/flow2.txt"

./ns3 run "scratch/1905103.cc --bottleLinkRate=50 --packetLossExp=4 --tcpType=1 --plot=3"
oldFile="scratch/Offline3/flow1.txt"
touch "scratch/Offline3/newreno_cong1.txt"
newFile="scratch/Offline3/newreno_cong1.txt"
mv "$oldFile" "$newFile"

oldFile="scratch/Offline3/flow2.txt"
touch "scratch/Offline3/highspeed_cong.txt"
newFile="scratch/Offline3/highspeed_cong.txt"
mv "$oldFile" "$newFile"

>"scratch/Offline3/flow1.txt"
>"scratch/Offline3/flow2.txt"

./ns3 run "scratch/1905103.cc --bottleLinkRate=50 --packetLossExp=4 --tcpType=2 --plot=3"
oldFile="scratch/Offline3/flow1.txt"
touch "scratch/Offline3/newreno_cong2.txt"
newFile="scratch/Offline3/newreno_cong2.txt"
mv "$oldFile" "$newFile"

oldFile="scratch/Offline3/flow2.txt"
touch "scratch/Offline3/adaptive_cong.txt"
newFile="scratch/Offline3/adaptive_cong.txt"
mv "$oldFile" "$newFile"

gnuplot 'task1.plt'