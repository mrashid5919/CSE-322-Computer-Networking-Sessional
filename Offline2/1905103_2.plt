set terminal pdf
set output "mobile_plot.pdf"

#vary nNodes


set title "Wireless Mobile (Throughput vs Number of Nodes) \nNumber of Flows = 60 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte \n \nSpeed = 20"
set xlabel "Number of Nodes"
set ylabel "Throughput (Mbps)"
set yrange [0:6000]
plot "varyNodes.txt" using 1:2 with lines title "Throughput"

set title "Wireless Mobile (Packet Delivery Ratio vs Number of Nodes) \nNumber of Flow = 60 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte \n \nSpeed = 20"
set xlabel "Number of Nodes"
set ylabel "Packet Delivery Ratio"
set yrange [20:60]
plot "varyNodes.txt" using 1:3 with lines title "Packet Delivery Ratio"


#vary nFlow


set title "Wireless Mobile (Throughput vs Number of Flow) \nNumber of Nodes = 30 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte \nSpeed = 20"
set xlabel "Number of Flow"
set ylabel "Throughput (Mbps)"
set yrange [0:6000]
plot "varyFlow.txt" using 1:2 with lines title "Throughput"

set title "Wireless Mobile (Packet Delivery Ratio vs Number of Flow) \nNumber of Nodes = 30 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte \nSpeed = 20"
set xlabel "Number of Flow"
set ylabel "Packet Delivery Ratio"
set yrange [40:80]
plot "varyFlow.txt" using 1:3 with lines title "Packet Delivery Ratio"


#vary nPPS


set title "Wireless Mobile (Throughput vs Number of Packets per second) \nNumber of Nodes = 30 \nNumber of Flow = 30 \nPacket Size = 1024 byte \nSpeed = 20"
set xlabel "Number of Packets per second"
set ylabel "Throughput (Mbps)"
set yrange [0:4000]
plot "varyPackets.txt" using 1:2 with lines title "Throughput"

set title "Wireless Mobile (Packet Delivery Ratio vs Packets per second) \nNumber of Nodes = 30 \nNumber of Flow = 30 \nPacket Size = 1024 byte \nSpeed = 20"
set xlabel "Number of Packets per second"
set ylabel "Packet Delivery Ratio"
set yrange [0:60]
plot "varyPackets.txt" using 1:3 with lines title "Packet Delivery Ratio"


#vary nCoverage


set title "Wireless Mobile (Throughput vs Node Speed) \nNumber of Nodes = 30 \nNumber of Flow = 30 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte"
set xlabel "Speed"
set ylabel "Throughput (Mbps)"
set yrange [2000:4000]
plot "varySpeed.txt" using 1:2 with lines title "Throughput"

set title "Wireless Mobile (Packet Delivery Ratio vs Node Speed) \nNumber of Nodes = 30 \nNumber of Flow = 30 \nNumber of Packets Per Second = 100 \nPacket Size = 1024 byte"
set xlabel "Speed"
set ylabel "Packet Delivery Ratio"
set yrange [40:80]
plot "varySpeed.txt" using 1:3 with lines title "Packet Delivery Ratio"
