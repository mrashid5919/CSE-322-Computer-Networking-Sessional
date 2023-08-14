set terminal pdf
set output "static_plot.pdf"

#vary nNodes


set title "Wireless Static (Throughput vs Number of Nodes) \nNumber of Flows = 60 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte \nTX Range = 10 \nCoverage Constant = 5"
set xlabel "Number of Nodes"
set ylabel "Throughput (Mbps)"
plot "varyNodes.txt" using 1:2 with lines title "Throughput"

set title "Wireless Static (Packet Delivery Ratio vs Number of Nodes) \nNumber of Flow = 60 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte \nTX Range = 10 \nCoverage Constant = 5"
set xlabel "Number of Nodes"
set ylabel "Packet Delivery Ratio"
plot "varyNodes.txt" using 1:3 with lines title "Packet Delivery Ratio"


#vary nFlow


set title "Wireless Static (Throughput vs Number of Flow) \nNumber of Nodes = 30 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte \nTX Range = 10 \nCoverage Constant = 5"
set xlabel "Number of Flow"
set ylabel "Throughput (Mbps)"
plot "varyFlow.txt" using 1:2 with lines title "Throughput"

set title "Wireless Static (Packet Delivery Ratio vs Number of Flow) \nNumber of Nodes = 30 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte \nTX Range = 10 \nCoverage Constant = 5"
set xlabel "Number of Flow"
set ylabel "Packet Delivery Ratio"
plot "varyFlow.txt" using 1:3 with lines title "Packet Delivery Ratio"


#vary nPPS


set title "Wireless Static (Throughput vs Number of Packets per second) \nNumber of Nodes = 30 \nNumber of Flow = 30 \nPacket Size = 1024 byte \nTX Range = 10 \nCoverage Constant = 5"
set xlabel "Number of Packets per second"
set ylabel "Throughput (Mbps)"
plot "varyPackets.txt" using 1:2 with lines title "Throughput"

set title "Wireless Static (Packet Delivery Ratio vs Number of Packets per second) \nNumber of Nodes = 30 \nNumber of Flow = 30 \nPacket Size = 1024 byte \nTX Range = 10 \nCoverage Constant = 5"
set xlabel "Number of Packets per second"
set ylabel "Packet Delivery Ratio"
plot "varyPackets.txt" using 1:3 with lines title "Packet Delivery Ratio"


#vary nCoverage


set title "Wireless Static (Throughput vs Coverage Constant) \nNumber of Nodes = 30 \nNumber of Flow = 30 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte \nTX Range = 10"
set xlabel "Coverage constant"
set ylabel "Throughput (Mbps)"
plot "varyCoverage.txt" using 1:2 with lines title "Throughput"

set title "Wireless Static (Packet Delivery Ratio vs Coverage Constant) \nNumber of Nodes = 30 \nNumber of Flow = 30 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte \nTX Range = 10"
set xlabel "Coverage Constant"
set ylabel "Packet Delivery Ratio"
plot "varyCoverage.txt" using 1:3 with lines title "Packet Delivery Ratio"
