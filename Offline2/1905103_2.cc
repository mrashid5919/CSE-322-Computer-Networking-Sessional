#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"

// Default Network Topology
//
//   Wifi 10.1.2.0
//                 AP
//  *    *    *    *
//  |    |    |    |    10.1.1.0
// n5   n6   n7   n0 -------------- n1   n2   n3   n4
//                   point-to-point  |    |    |    |
//                                   *    *    *    *
//                                   AP
//                                      Wifi 10.1.3.0

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("1905103-offline-2");

ApplicationContainer receiverApps;
uint32_t totalSentPackets=0;
uint32_t PacketSentTillLastSec=0;
uint32_t totalReceivedPackets=0;
uint32_t PacketReceivedTillLastSec=0;
uint32_t prevSecReceive=0;

double totalByteReceived()
{
    double x=0;
    uint32_t nApps=receiverApps.GetN();
    for(uint32_t i=0;i<nApps;i++)
    {
        Ptr<PacketSink> sink=StaticCast<PacketSink>(receiverApps.Get(i));
        x+=sink->GetTotalRx();
    }
    return x;
}

void packetSent(Ptr<const Packet>packet)
{
  totalSentPackets++;
}

void packetReceived(Ptr<const Packet> packet,const Address &address)
{
  totalReceivedPackets++;
}

void CalculateThroughput()
{
    double rx=totalByteReceived();
    uint32_t tmp=rx;
    rx=rx-prevSecReceive;
    prevSecReceive=tmp;
    rx=(rx*8.0)/1e6;
    std::cout<<std::fixed;
    std::cout<<(Simulator::Now()).GetSeconds()<<" "<<rx<<" Mbps"<<std::endl;
    Simulator::Schedule(Seconds(1.0),&CalculateThroughput);
}

void CalculatePacketDeliveryRatio()
{
    uint32_t nPacketReceived=totalReceivedPackets-PacketReceivedTillLastSec;
    uint32_t nPacketSent=totalSentPackets-PacketSentTillLastSec;
    double ratio=0;
    std::cout<<(Simulator::Now()).GetSeconds()<<" packetSent: "<<nPacketSent<<std::endl;
    std::cout<<(Simulator::Now()).GetSeconds()<<" packetReceived: "<<nPacketReceived<<std::endl;
    PacketSentTillLastSec=totalSentPackets;
    PacketReceivedTillLastSec=totalReceivedPackets;
    if(nPacketSent)
    {
      ratio=(double)nPacketReceived/(double)nPacketSent;
      std::cout<<(Simulator::Now()).GetSeconds()<<" Packet delivery ratio: "<<ratio<<std::endl;
    }
    Simulator::Schedule(Seconds(1.0),&CalculatePacketDeliveryRatio);
}

int main(int argc,char* argv[])
{
    bool verbose=true;
    uint32_t nNodes = 20;
    uint32_t nFlows = 40;
    uint32_t nPackets = 100;  
    uint32_t speed = 5;  //for wireless mobile topology
    uint32_t plot=1;
    //uint32_t coverageArea = 10; //for static topology
    //uint32_t Tx_range = 5;

    std::string bottleNeckLinkBw = "5Mbps";
    std::string bottleNeckLinkDelay = "50ms";
    

    CommandLine cmd(__FILE__);
    cmd.AddValue ("nNodes","Number of left and right side leaf nodes", nNodes);
    cmd.AddValue ("nFlows","Number of flows", nFlows);
    cmd.AddValue ("nPackets", "Number of packets per second", nPackets);
    cmd.AddValue ("speed", "Speed of nodes:", speed);

    cmd.AddValue("plot", "varying parametre for plot", plot);
    //cmd.AddValue("CoverageArea", "Coverage area for static topology", coverageArea);
    cmd.Parse(argc, argv);

    uint32_t dataRateInt = (nPackets * 1024 * 8);
    std::string dataRate = std::to_string(dataRateInt) + "bps"; 
    uint32_t payloadSize = 1024;           /* Transport layer payload size in bytes. it is same as the pckt size */
    uint32_t nWifi = nNodes/2;
    //Tx_range *= coverageArea;
    //bool tracing=false;
    
    cmd.Parse(argc, argv);

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    /* Configure TCP Options */
    Config::SetDefault("ns3::TcpSocket::SegmentSize", UintegerValue(payloadSize));

    // Create gateways, sources, and sinks
    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate",StringValue(bottleNeckLinkBw));
    pointToPoint.SetChannelAttribute("Delay",StringValue(bottleNeckLinkDelay));

    NetDeviceContainer p2pDevices;
    p2pDevices=pointToPoint.Install(p2pNodes);

    NodeContainer wifiStaNodes0,wifiStaNodes1;
    wifiStaNodes0.Create(nWifi);
    wifiStaNodes1.Create(nWifi);
    NodeContainer wifiApNode0=p2pNodes.Get(0);
    NodeContainer wifiApNode1=p2pNodes.Get(1);

    //constructs the wifi devices and the interconnection channel between these wifi nodes
    YansWifiChannelHelper channel0=YansWifiChannelHelper::Default();
    YansWifiChannelHelper channel1=YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy0,phy1;
    phy0.SetChannel(channel0.Create());
    phy1.SetChannel(channel1.Create());

    WifiHelper wifi;
    //wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper mac0,mac1;
    Ssid ssid0=Ssid("ns-3-ssid-0");
    Ssid ssid1=Ssid("ns-3-ssid-1");

    mac0.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid0),
                "ActiveProbing", BooleanValue (false)); 
                
    mac1.SetType ("ns3::StaWifiMac",
                "Ssid", SsidValue (ssid1),
                "ActiveProbing", BooleanValue (false));

    NetDeviceContainer staDevices0,staDevices1;
    mac0.SetType("ns3::StaWifiMac","Ssid",SsidValue(ssid0),"ActiveProbing",BooleanValue(false));
    mac1.SetType("ns3::StaWifiMac","Ssid",SsidValue(ssid1),"ActiveProbing",BooleanValue(false));

    staDevices0=wifi.Install(phy0,mac0,wifiStaNodes0);
    staDevices1=wifi.Install(phy1,mac1,wifiStaNodes1);

    mac0.SetType ("ns3::ApWifiMac",
                "Ssid", SsidValue (ssid0));
    mac1.SetType ("ns3::ApWifiMac",
                "Ssid", SsidValue (ssid1));

    NetDeviceContainer apDevices0,apDevices1;
    apDevices0=wifi.Install(phy0,mac0,wifiApNode0);
    apDevices1=wifi.Install(phy1,mac1,wifiApNode1);

    MobilityHelper mobility;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (0.5),
                                 "DeltaY", DoubleValue (1.0),
                                 "GridWidth", UintegerValue (nNodes/2),
                                 "LayoutType", StringValue ("RowFirst"));
  
  // // tell STA nodes how to move
   mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                              "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)),
                              "Speed", StringValue ("ns3::ConstantRandomVariable[Constant="+std::to_string(speed)+"]"));
   //mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   //Config::SetDefault("ns3::RangePropagationLossModel::MaxRange",DoubleValue(Tx_range));
  // install on STA nodes
  mobility.Install (wifiStaNodes0);
  mobility.Install (wifiStaNodes1);

  // tell AP node to stay still
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  // install on AP node
  mobility.Install (wifiApNode0);
  mobility.Install (wifiApNode1);

    InternetStackHelper stack;
    stack.Install(wifiApNode0);
    stack.Install(wifiApNode1);
    stack.Install(wifiStaNodes0);
    stack.Install(wifiStaNodes1);

    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces=address.Assign(p2pDevices);

    address.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer staNodeInterfaces0=address.Assign(staDevices0);
    Ipv4InterfaceContainer apNodeInterfaces0=address.Assign(apDevices0);

    address.SetBase("10.1.3.0","255.255.255.0");
    Ipv4InterfaceContainer staNodeInterfaces1=address.Assign(staDevices1);
    Ipv4InterfaceContainer apNodeInterfaces1=address.Assign(apDevices1);

    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(),9));
    for(uint32_t i=0;i<nWifi;i++)
    {
        receiverApps.Add(packetSinkHelper.Install(wifiStaNodes1.Get(i)));
    }

    OnOffHelper senderHelper("ns3::TcpSocketFactory",Address());
    senderHelper.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    senderHelper.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    senderHelper.SetAttribute("PacketSize",UintegerValue(payloadSize));
    senderHelper.SetAttribute("DataRate",DataRateValue(DataRate(dataRate)));

    ApplicationContainer senderApps;

    uint32_t curFlow=0;
    while(curFlow<nFlows)
    {
        for(uint32_t i=0;i<nWifi;i++)
        {
            AddressValue remoteAddress(InetSocketAddress(staNodeInterfaces1.GetAddress(i),9));
            senderHelper.SetAttribute("Remote",remoteAddress);
            senderApps.Add(senderHelper.Install(wifiStaNodes0.Get(i)));
            curFlow++;

            if(curFlow==nFlows)
              break;
        }
    }
    receiverApps.Start(Seconds(1.0));
    senderApps.Start(Seconds(2.0));
    senderApps.Stop(Seconds(6.0));
    receiverApps.Stop(Seconds(6.5));

    Simulator::Schedule(Seconds(1.0),&CalculateThroughput);
    Simulator::Schedule(Seconds(1.0),&CalculatePacketDeliveryRatio);


    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(10.0));

    //Trace
    for(uint32_t i=0;i<senderApps.GetN();i++)
    {
      Ptr<OnOffApplication> packet=StaticCast<OnOffApplication>(senderApps.Get(i));
      packet->TraceConnectWithoutContext("Tx",MakeCallback(&packetSent));
    }

    for(uint32_t i=0;i<receiverApps.GetN();i++)
    {
      Ptr<OnOffApplication> packet=StaticCast<OnOffApplication>(receiverApps.Get(i));
      packet->TraceConnectWithoutContext("Rx",MakeCallback(&packetReceived));
    }
    Simulator::Run();
    Simulator::Destroy();

    double avgThroughPut=(totalReceivedPackets*8*1024)/(1e6*9);
    double deliveryRatio=((double)totalReceivedPackets/totalSentPackets);
    std::cout<<avgThroughPut<<" Mbps "<<deliveryRatio<<"\n";

    if(plot == 1){
      std::ofstream out1("varyNodes.txt",std::ios::app);
      out1<<nNodes<<" "<<avgThroughPut<<" "<<deliveryRatio<<std::endl;
      out1.close();
    }

    else if(plot == 2){
      std::ofstream out2("varyFlow.txt",std::ios::app);
      out2<<nFlows<<" "<<avgThroughPut<<" "<<deliveryRatio<<std::endl;
      out2.close();    
    }

    else if(plot == 3){

      std::ofstream out3("varyPackets.txt",std::ios::app);
      out3<<nPackets<<" "<<avgThroughPut<<" "<<deliveryRatio<<std::endl;
      out3.close();    
    }

    else{

      std::ofstream out4("varySpeed.txt",std::ios::app);
      out4<<speed<<" "<<avgThroughPut<<" "<<deliveryRatio<<std::endl;
      out4.close(); 
    
    }
}