#!/bin/bash

> "varyNodes.txt"
> "varyFlow.txt"
> "varyPackets.txt"
> "varyCoverage.txt"
> "static_plot.pdf"

#varyNodes
./ns3 run "scratch/1905103_1.cc --nNodes=20 --nFlows=60 --nPackets=100 --CoverageArea=5 --plot=1"
./ns3 run "scratch/1905103_1.cc --nNodes=40 --nFlows=60 --nPackets=100 --CoverageArea=5 --plot=1"
./ns3 run "scratch/1905103_1.cc --nNodes=60 --nFlows=60 --nPackets=100 --CoverageArea=5 --plot=1"
./ns3 run "scratch/1905103_1.cc --nNodes=80 --nFlows=60 --nPackets=100 --CoverageArea=5 --plot=1"
./ns3 run "scratch/1905103_1.cc --nNodes=100 --nFlows=60 --nPackets=100 --CoverageArea=5 --plot=1"

#varyFlow
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=10 --nPackets=100 --CoverageArea=5 --plot=2"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=20 --nPackets=100 --CoverageArea=5 --plot=2"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=30 --nPackets=100 --CoverageArea=5 --plot=2"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=40 --nPackets=100 --CoverageArea=5 --plot=2"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=50 --nPackets=100 --CoverageArea=5 --plot=2"

#varyPackets
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=60 --nPackets=100 --CoverageArea=5 --plot=3"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=60 --nPackets=200 --CoverageArea=5 --plot=3"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=60 --nPackets=300 --CoverageArea=5 --plot=3"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=60 --nPackets=400 --CoverageArea=5 --plot=3"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=60 --nPackets=500 --CoverageArea=5 --plot=3"

#varyCoverage
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=60 --nPackets=100 --CoverageArea=1 --plot=4"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=60 --nPackets=100 --CoverageArea=2 --plot=4"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=60 --nPackets=100 --CoverageArea=3 --plot=4"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=60 --nPackets=100 --CoverageArea=4 --plot=4"
./ns3 run "scratch/1905103_1.cc --nNodes=30 --nFlows=60 --nPackets=100 --CoverageArea=5 --plot=4"

gnuplot '1905103_1.plt'

