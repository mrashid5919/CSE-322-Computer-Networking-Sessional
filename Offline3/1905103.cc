
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-layout-module.h"
#include "ns3/applications-module.h"
#include "ns3/stats-module.h"
#include "ns3/callback.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/csma-module.h"
#include "tutorial-app.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Task1");

// ===========================================================================
//
//            s0----      -----r0
//             router0---router1
//            s1----      -----r1
//
// ===========================================================================
//

static void
CwndChange (Ptr<OutputStreamWrapper> stream, uint32_t oldCwnd, uint32_t newCwnd)
{
  // NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << " " << newCwnd);
  *stream->GetStream () << Simulator::Now ().GetSeconds () << " " << newCwnd << std::endl;
}



int main(int argc, char *argv[]){
    uint32_t payloadSize = 1024;
    uint32_t nnodes=2;
    uint32_t nflows=2;
    std::string dataRate="1Gbps";
    std::string delay="1ms";
    //uint32_t bottleLinkRateInt=50;
    std::string bottleLinkDelay="100ms";
    uint32_t plot=1;
    uint32_t simulTime=60;
    uint32_t cleanTime=2;

    uint32_t bottleLinkRate=50; //range 1-300 Mbps, const packet loss 10^-6
    int packetLossExp=6; //range 10^-2 to 10^-6, const blRate= 50Mbps
    std::string tcpType="ns3::TcpHighSpeed";
    uint32_t type=1;

    CommandLine cmd (__FILE__);
    cmd.AddValue("bottleLinkRate","Max Packets allowed in the device queue",bottleLinkRate);
    cmd.AddValue("packetLossExp","Exponent of Packet Loss Rate",packetLossExp);
    cmd.AddValue("tcpType","Type of tcp used for flow",type);
    cmd.AddValue("plot","varying parameter for plot",plot);
    cmd.Parse(argc,argv);

    if(type==1)
      tcpType="ns3::TcpHighSpeed";
    else
      tcpType="ns3::TcpAdaptiveReno";

    double pktLossRate=(1.0/std::pow(10,packetLossExp));
    double bandwidth_delay_product=(bottleLinkRate*100)/1024;
    Config::SetDefault("ns3::TcpSocket::SegmentSize",UintegerValue(payloadSize));

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate",StringValue(std::to_string(bottleLinkRate)+"Mbps"));
    pointToPoint.SetChannelAttribute("Delay",StringValue(bottleLinkDelay));

    PointToPointHelper leafNodes;
    leafNodes.SetDeviceAttribute("DataRate",StringValue(dataRate));
    leafNodes.SetChannelAttribute("Delay",StringValue(delay));

    leafNodes.SetQueue ("ns3::DropTailQueue", "MaxSize",
    StringValue (std::to_string (bandwidth_delay_product) + "p"));

    PointToPointDumbbellHelper dumbbel(nnodes,leafNodes,nnodes,leafNodes,pointToPoint);

    Ptr<RateErrorModel> errorModel=CreateObject<RateErrorModel>();
    errorModel->SetAttribute("ErrorRate",DoubleValue(pktLossRate));
    dumbbel.m_routerDevices.Get(1)->SetAttribute("ReceiveErrorModel",PointerValue(errorModel));

    Config::SetDefault("ns3::TcpL4Protocol::SocketType",StringValue("ns3::TcpNewReno"));
    InternetStackHelper stack1;

    stack1.Install(dumbbel.GetLeft(0));
    stack1.Install(dumbbel.GetRight(0));
    stack1.Install(dumbbel.GetLeft());
    stack1.Install(dumbbel.GetRight());

    Config::SetDefault("ns3::TcpL4Protocol::SocketType",StringValue(tcpType));

    InternetStackHelper stack2;
    stack2.Install(dumbbel.GetLeft(1));
    stack2.Install(dumbbel.GetRight(1));

    dumbbel.AssignIpv4Addresses(Ipv4AddressHelper("10.1.1.0","255.255.255.0"),Ipv4AddressHelper("10.2.1.0","255.255.255.0"),Ipv4AddressHelper("10.3.1.0","255.255.255.0")); //left, right, routers

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    FlowMonitorHelper flowMonitor;
    flowMonitor.SetMonitorAttribute("MaxPerHopDelay",TimeValue(Seconds(cleanTime)));
    Ptr<FlowMonitor> flmonitor=flowMonitor.InstallAll();

    for(uint32_t i=0;i<nflows;i++)
    {
        Address remoteAddress (InetSocketAddress(dumbbel.GetRightIpv4Address(i),8080));
        PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(),8080));
        ApplicationContainer sinkApps=packetSinkHelper.Install(dumbbel.GetRight(i));
        sinkApps.Start(Seconds(0));
        sinkApps.Stop(Seconds(simulTime+cleanTime));

        std::ostringstream oss;
        oss << "scratch/Offline3/flow" << i+1 <<  ".txt";
        AsciiTraceHelper asciiTraceHelper;
        Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream (oss.str());
        
        Ptr<Socket> ns3TcpSocket = Socket::CreateSocket (dumbbel.GetLeft (i), TcpSocketFactory::GetTypeId ());
        Ptr<TutorialApp> tutorialApp = CreateObject<TutorialApp> ();
        uint32_t nPackets = 10000000;
        tutorialApp->Setup (ns3TcpSocket, remoteAddress, 1024, nPackets, DataRate (dataRate));
        dumbbel.GetLeft (i)->AddApplication (tutorialApp);
        tutorialApp->SetStartTime (Seconds (1));
        tutorialApp->SetStopTime (Seconds (simulTime));       
        ns3TcpSocket->TraceConnectWithoutContext ("CongestionWindow", MakeBoundCallback (&CwndChange, stream));
    }

    Simulator::Stop(Seconds(simulTime+cleanTime));
    Simulator::Run();

    float CurrentThroughputAra[]={0,0};
    std::ofstream os("",std::ios_base::app);

    Ptr<Ipv4FlowClassifier> classifier=DynamicCast<Ipv4FlowClassifier> (flowMonitor.GetClassifier());
    FlowMonitor::FlowStatsContainer flowStats=flmonitor->GetFlowStats();

    uint32_t cnt=0;
    for(auto it=flowStats.begin();it!=flowStats.end();it++)
    {
        CurrentThroughputAra[cnt]+=it->second.rxBytes;
        cnt=1-cnt;
    }
    for(uint32_t i=0;i<2;i++){
      //CurrentThroughputAra[i]*=8;
      CurrentThroughputAra[i]/=((simulTime+cleanTime)*1000);
    }

    std::ofstream out;

    std::string filepath;
    if(plot==1)
      filepath="scratch/Offline3/bottle_neck_datarate_vs_throughput_"+std::to_string(type)+".txt";
    else if(plot==2)
      filepath="scratch/Offline3/packet_loss_rate_vs_throughput_" + std::to_string(type) + ".txt";
    out.open(filepath,std::ios::app);
    if(plot==1)
      out<<bottleLinkRate<<" "<<CurrentThroughputAra[0]<<" "<<CurrentThroughputAra[1]<<std::endl;
    else if(plot==2)
      out<<(-1)*packetLossExp<<" "<<CurrentThroughputAra[0]<<" "<<CurrentThroughputAra[1]<<std::endl;
    out.close();

    Simulator::Destroy();

    return 0;
}