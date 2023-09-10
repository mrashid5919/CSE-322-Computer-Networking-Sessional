#include "tcp-adaptive-reno.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "rtt-estimator.h"
#include "tcp-socket-base.h"

NS_LOG_COMPONENT_DEFINE ("TcpAdaptiveReno");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TcpAdaptiveReno);

TypeId
TcpAdaptiveReno::GetTypeId (void)
{
  static TypeId tid = TypeId("ns3::TcpAdaptiveReno")
    .SetParent<TcpNewReno>()
    .SetGroupName ("Internet")
    .AddConstructor<TcpAdaptiveReno>()
    .AddAttribute("FilterType", "Use this to choose no filter or Tustin's approximation filter",
                  EnumValue(TcpAdaptiveReno::TUSTIN), MakeEnumAccessor(&TcpAdaptiveReno::m_fType),
                  MakeEnumChecker(TcpAdaptiveReno::NONE, "None", TcpAdaptiveReno::TUSTIN, "Tustin"))
    .AddTraceSource("EstimatedBW", "The estimated bandwidth",
                    MakeTraceSourceAccessor(&TcpAdaptiveReno::m_currentBW),
                    "ns3::TracedValueCallback::Double")
  ;
  return tid;
}

TcpAdaptiveReno::TcpAdaptiveReno (void) :
  TcpWestwoodPlus(),
  minRtt (Time (0)),
  curRtt (Time (0)),
  jPacketLossRtt (Time (0)),
  jConjRtt (Time (0)),
  prevConjRtt (Time(0)),
  m_incWnd (0),
  m_baseWnd (0),
  m_probeWnd (0)
{
  NS_LOG_FUNCTION (this);
}

TcpAdaptiveReno::TcpAdaptiveReno (const TcpAdaptiveReno& sock) :
  TcpWestwoodPlus (sock),
  minRtt (Time (0)),
  curRtt (Time (0)),
  jPacketLossRtt (Time (0)),
  jConjRtt (Time (0)),
  prevConjRtt (Time(0)),
  m_incWnd (0),
  m_baseWnd (0),
  m_probeWnd (0)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Invoked the copy constructor");
}

TcpAdaptiveReno::~TcpAdaptiveReno (void)
{
}

void
TcpAdaptiveReno::PktsAcked (Ptr<TcpSocketState> tcb, uint32_t packetsAcked,
                        const Time& rtt)
{
  NS_LOG_FUNCTION (this << tcb << packetsAcked << rtt);

  if (rtt.IsZero ())
    {
      //NS_LOG_WARN ("RTT measured is zero!");
      return;
    }

  m_ackedSegments += packetsAcked;


  // calculate min rtt here
  if(minRtt.IsZero()) { minRtt = rtt; }
  else if(rtt <= minRtt) { minRtt = rtt; }

  curRtt = rtt;

  TcpWestwoodPlus::EstimateBW (rtt, tcb);
}


double
TcpAdaptiveReno::EstimateCongestionLevel()
{

  float m_a = 0.85; // exponential smoothing factor
  if(prevConjRtt < minRtt) m_a = 0; // the initial value should take the full current Jth loss Rtt
  
  double conjRtt = m_a*prevConjRtt.GetSeconds() + (1-m_a)*jPacketLossRtt.GetSeconds(); 
  jConjRtt = Seconds(conjRtt); 
  NS_LOG_LOGIC("conj rtt : " << jConjRtt << " ; prevConjRtt : " << prevConjRtt << " ; j rtt : " << jPacketLossRtt);

  return std::min(
    (curRtt.GetSeconds() - minRtt.GetSeconds()) / (conjRtt - minRtt.GetSeconds()),
    1.0
  );
}


void 
TcpAdaptiveReno::EstimateIncWnd(Ptr<TcpSocketState> tcb)
{
  /*

      c = EstimateCongestionLevel() // congestion level
      scalingFactor_m = 10 // in Mbps
      m_maxIncWnd = EstimateBW() / scalingFactor_m

      alpha = 10 
      beta = 2 * m_maxIncWnd * (1/alpha - (1/alpha + 1)/(e^alpha))
      gamma = 1 - 2 * m_maxIncWnd * (1/alpha - (1/alpha + 1/2)/(e^alpha))

      m_incWnd = (m_maxIncWnd / (e^(alpha * c))) + (beta * c) + gamma

  */

  double congestion = EstimateCongestionLevel();
  int scalingFactor_m = 1000; // 10 mbps in paper 
  
  // m_currentBW; -> already calculated in packetsAck
  double m_maxIncWnd = (m_currentBW.Get().GetBitRate()/8) / scalingFactor_m * static_cast<double> (tcb->m_segmentSize * tcb->m_segmentSize) ; 

  double alpha = 10; // 2 10
  double beta = 2 * m_maxIncWnd * ((1/alpha) - ((1/alpha + 1)/(std::exp(alpha))));
  double gamma = 1 - (2 * m_maxIncWnd * ((1/alpha) - ((1/alpha + 0.5)/(std::exp(alpha)))));

  m_incWnd = (int)((m_maxIncWnd / std::exp(alpha * congestion)) + (beta * congestion) + gamma);

  NS_LOG_LOGIC ("MinRtt: " << minRtt.GetMilliSeconds () << "ms");
  NS_LOG_LOGIC ("ConjRtt: " << jConjRtt.GetMilliSeconds () << "ms");
  NS_LOG_LOGIC("maxInc: "<<m_maxIncWnd<<"; congestion: "<<congestion<<" ; beta: "<<beta<<" ; gamma: "<<gamma<<
  " ; exp(alpha * congestion): "<<std::exp(alpha * congestion));
  NS_LOG_LOGIC("m_incWnd: "<<m_incWnd<<" ; prev_wind: "<<tcb->m_cWnd<<" ; new: "<<(m_incWnd / (int)tcb->m_cWnd));

  NS_LOG_LOGIC ("Congestion level: " << congestion);
  NS_LOG_LOGIC ("Increment Window: " << m_incWnd);
}


void
TcpAdaptiveReno::CongestionAvoidance (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked)
{
  /*
  
      base_window = USE NEW RENO IMPLEMENTATION
      m_probeWnd = max(m_probeWnd + m_incWnd / current_window,  0)
      current_window = base_window + m_probeWnd

  */

  NS_LOG_FUNCTION (this << tcb << segmentsAcked);

  if (segmentsAcked > 0)
    {
      EstimateIncWnd(tcb);
      // base_window = USE NEW RENO IMPLEMENTATION
      double adder = static_cast<double> (tcb->m_segmentSize * tcb->m_segmentSize) / tcb->m_cWnd.Get ();
      adder = std::max (1.0, adder);
      m_baseWnd += static_cast<uint32_t> (adder);

      // change probe window
      m_probeWnd = std::max(
        (double) (m_probeWnd + m_incWnd / (int)tcb->m_cWnd.Get()), 
        (double) 0
      );
      
      NS_LOG_LOGIC("Before "<<tcb->m_cWnd<< " ; base "<<m_baseWnd<<" ; probe "<<m_probeWnd);
      tcb->m_cWnd = m_baseWnd + m_probeWnd;
      // NS_LOG_UNCOND("Congestion window changed");

      NS_LOG_INFO ("In CongAvoid, updated to cwnd " << tcb->m_cWnd <<
                   " ssthresh " << tcb->m_ssThresh);
    }

}

uint32_t
TcpAdaptiveReno::GetSsThresh (Ptr<const TcpSocketState> tcb,
                          uint32_t bytesInFlight)
{
  /*
  
      c = EstimateCongestionLevel() // congestion level
      RETURN:
      new_window = current_window / (1 + c)

  */
  prevConjRtt = jConjRtt; // a loss event has occured. so set the previous conjestion RTT
  jPacketLossRtt = curRtt; // this will now contain the RTT of previous packet or jth loss event
  
  double congestion = EstimateCongestionLevel();
  // NS_LOG_UNCOND("SSThresh called");

  uint32_t ssthresh = std::max (
    2*tcb->m_segmentSize,
    (uint32_t) (tcb->m_cWnd / (1.0+congestion))
  );

  // reset calculations
  m_baseWnd = ssthresh;
  m_probeWnd = 0;
  
  NS_LOG_LOGIC("new ssthresh : "<<ssthresh<<" ; old conj Rtt : "<<prevConjRtt<<" ; new conj Rtt : "<<jConjRtt<<" ; cong : "<<congestion);
  
  return ssthresh;

}

Ptr<TcpCongestionOps>
TcpAdaptiveReno::Fork ()
{
  return CreateObject<TcpAdaptiveReno> (*this);
}

} // namespace ns3
