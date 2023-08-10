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

NS_LOG_COMPONENT_DEFINE("1905103-offline-1");

Ptr<PacketSink> sink;     //!< Pointer to the packet sink application
uint64_t lastTotalRx = 0; //!< The value of the last total received bytes
Ptr<OutputStreamWrapper> stream;

void
CalculateThroughput()
{
    Time now = Simulator::Now(); /* Return the simulator's virtual time. */
    double cur = (sink->GetTotalRx() - lastTotalRx) * 8.0 /
                 1e6 / 0.5; /* Convert Application RX Packets to MBits. */
    // std::cout << now.GetSeconds() << "s: \t" << cur << " Mbit/s" << std::endl;
    NS_LOG_UNCOND(now.GetSeconds() << "s: \t" << cur << " Mbit/s");
    *stream->GetStream() << now.GetSeconds() << "\t" << cur  << std::endl;
    lastTotalRx = sink->GetTotalRx();
    // std::cout << "Sink total received : " << sink->GetTotalRx() << std::endl;
    Simulator::Schedule(MilliSeconds(500), &CalculateThroughput);
}

int main(int argc,char* argv[])
{
    bool verbose=true;
    uint32_t nWifi=10;
    uint32_t payloadSize=1024;
    int num_flow=100;
    int dataRate=5;
    bool tracing=false;
    
    CommandLine cmd(__FILE__);
    //cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("nWifi", "Number of wifi STA devices", nWifi);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);

    cmd.Parse(argc, argv);

    // The underlying restriction of 18 is due to the grid position
    // allocator's configuration; the grid layout will exceed the
    // bounding box if more than 18 nodes are provided.
    if (nWifi > 18)
    {
        std::cout << "nWifi should be 18 or less; otherwise grid layout exceeds the bounding box"
                  << std::endl;
        return 1;
    }

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
    pointToPoint.SetDeviceAttribute("DataRate",StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay",StringValue("2ms"));

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
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    WifiMacHelper mac0,mac1;
    Ssid ssid0=Ssid("ns-3-ssid-0");
    Ssid ssid1=Ssid("ns-3-ssid-1");

    NetDeviceContainer staDevices0,staDevices1;
    mac0.SetType("ns3::StaWifiMac","Ssid",SsidValue(ssid0),"ActiveProbing",BooleanValue(false));
    mac1.SetType("ns3::StaWifiMac","Ssid",SsidValue(ssid1),"ActiveProbing",BooleanValue(false));

    staDevices0=wifi.Install(phy0,mac0,wifiStaNodes0);
    staDevices1=wifi.Install(phy1,mac1,wifiStaNodes1);

    NetDeviceContainer apDevices0,apDevices1;
    apDevices0=wifi.Install(phy0,mac0,wifiApNode0);
    apDevices1=wifi.Install(phy1,mac1,wifiApNode1);

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

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    ApplicationContainer senderApps;
    ApplicationContainer receiverApps;

    int port=9;
    int temp=num_flow;
    while(true)
    {
      for(int i=0;i<nWifi/2;i++)
      {
        OnOffHelper senderHelper("ns3::TcpSocketFactory",InetSocketAddress(staNodeInterfaces1.GetAddress(i),port));
        senderHelper.SetAttribute("PacketSize",UintegerValue(payloadSize));
        senderHelper.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        senderHelper.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
        senderHelper.SetAttribute("DataRate",DataRateValue(DataRate(dataRate)));

        senderApps.Add(senderHelper.Install(wifiStaNodes0.Get(i)));

        PacketSinkHelper receiverHelper("ns3::TcpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(),port));
        receiverApps.Add(receiverHelper.Install(wifiStaNodes1.Get(i)));

        port++;
        temp--;
        if(temp==0)
          break;
      }
      if(temp==0)
        break;
    }

    receiverApps.Start(Seconds(1.0));
    senderApps.Start(Seconds(2.0));

    return 0;
}