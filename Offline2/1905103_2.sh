#!/bin/bash

> "varyNodes.txt"
> "varyFlow.txt"
> "varyPackets.txt"
> "varySpeed.txt"
> "mobile_plot.pdf"

#varyNodes
./ns3 run "scratch/1905103_2.cc --nNodes=20 --nFlows=60 --nPackets=100 --speed=20 --plot=1"
./ns3 run "scratch/1905103_2.cc --nNodes=40 --nFlows=60 --nPackets=100 --speed=20 --plot=1"
./ns3 run "scratch/1905103_2.cc --nNodes=60 --nFlows=60 --nPackets=100 --speed=20 --plot=1"
./ns3 run "scratch/1905103_2.cc --nNodes=80 --nFlows=60 --nPackets=100 --speed=20 --plot=1"
./ns3 run "scratch/1905103_2.cc --nNodes=100 --nFlows=60 --nPackets=100 --speed=20 --plot=1"

#varyFlow
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=10 --nPackets=100 --speed=20 --plot=2"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=20 --nPackets=100 --speed=20 --plot=2"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=30 --nPackets=100 --speed=20 --plot=2"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=40 --nPackets=100 --speed=20 --plot=2"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=50 --nPackets=100 --speed=20 --plot=2"

#varyPackets
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=60 --nPackets=100 --speed=20 --plot=3"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=60 --nPackets=200 --speed=20 --plot=3"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=60 --nPackets=300 --speed=20 --plot=3"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=60 --nPackets=400 --speed=20 --plot=3"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=60 --nPackets=500 --speed=20 --plot=3"

#varyCoverage
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=60 --nPackets=100 --speed=5 --plot=4"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=60 --nPackets=100 --speed=10 --plot=4"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=60 --nPackets=100 --speed=15 --plot=4"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=60 --nPackets=100 --speed=20 --plot=4"
./ns3 run "scratch/1905103_2.cc --nNodes=30 --nFlows=60 --nPackets=100 --speed=25 --plot=4"

gnuplot '1905103_2.plt'

