set terminal pdf
set output "task1.pdf"

#vary bottleLinkRate

set title "TcpHighSpeed (Throughput vs BottleNeck Link Capacity) \n"
set xlabel "BottleNeck Link Capacity (Mbps)"
set ylabel "Throughput (Kbps)"
plot "scratch/Offline3/bottle_neck_datarate_vs_throughput_1.txt" using 1:2 title "TcpNewReno" with lines , "scratch/Offline3/bottle_neck_datarate_vs_throughput_1.txt" using 1:3 title "TcpHighSpeed" with lines 

set title "TcpAdaptiveReno (Throughput vs BottleNeck Link Capacity) \n"
set xlabel "BottleNeck Link Capacity (Mbps)"
set ylabel "Throughput (Kbps)"
plot "scratch/Offline3/bottle_neck_datarate_vs_throughput_2.txt" using 1:2 title "TcpNewReno" with lines , "scratch/Offline3/bottle_neck_datarate_vs_throughput_2.txt" using 1:3 title "TcpAdaptiveReno" with lines 


#vary pktLossRate

set title "TcpHighSpeed (Throughput vs Packet Loss Rate) \n"
set xlabel "Packet Loss Rate"
set ylabel "Throughput (Kbps)"
plot "scratch/Offline3/packet_loss_rate_vs_throughput_1.txt" using 1:2 title "TcpNewReno" with lines , "scratch/Offline3/packet_loss_rate_vs_throughput_1.txt" using 1:3 title "TcpHighSpeed" with lines

set title "TcpAdaptiveReno (Throughput vs Packet Loss Rate) \n"
set xlabel "Packet Loss Rate"
set ylabel "Throughput (Kbps)"
plot "scratch/Offline3/packet_loss_rate_vs_throughput_2.txt" using 1:2 title "TcpNewReno" with lines , "scratch/Offline3/packet_loss_rate_vs_throughput_2.txt" using 1:3 title "TcpAdaptiveReno" with lines 

set title "Congestion vs Time \n"
set xlabel "time"
set ylabel "Cwnd"
plot "scratch/Offline3/newreno_cong1.txt" using 1:2 title "TcpNewReno" with lines, "scratch/Offline3/highspeed_cong.txt" using 1:2 title "TcpHighSpeed" with lines

set title "Congestion vs Time \n"
set xlabel "time"
set ylabel "Cwnd"
plot "scratch/Offline3/newreno_cong2.txt" using 1:2 title "TcpNewReno" with lines, "scratch/Offline3/adaptive_cong.txt" using 1:2 title "TcpAdaptiveReno" with lines