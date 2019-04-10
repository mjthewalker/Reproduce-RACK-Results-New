#include "tcp-tlp.h"

#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("TcpTlp");
NS_OBJECT_ENSURE_REGISTERED(TcpTlp);

TypeId
TcpTlp::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::TcpTlp").SetParent<Object>().AddConstructor<TcpTlp>().SetGroupName("Internet");
    return tid;
}

TcpTlp::TcpTlp(void)
    : Object(),
      m_srtt(0),
      m_alpha(0.125),
      m_pto(MilliSeconds(2)),
      m_tlpRtt(0)
{
    NS_LOG_FUNCTION(this);
}

TcpTlp::TcpTlp(const TcpTlp& other)
    : Object(other),
      m_srtt(other.m_srtt),
      m_alpha(other.m_alpha),
      m_pto(other.m_pto),
      m_tlpRtt(other.m_tlpRtt)

{
    NS_LOG_FUNCTION(this);
}

TcpTlp::~TcpTlp()
{
    NS_LOG_FUNCTION(this);
}

// Calculate the value of PTO
Time
TcpTlp::CalculatePto(Time rtt, uint32_t inflight, double rto_left)
{
    NS_LOG_FUNCTION(this);

    Time curr_pto;
    if (rtt > Seconds(0))
    {
        m_tlpRtt = rtt;
    }

    if (m_srtt == 0)
    {
        m_srtt = m_tlpRtt.GetSeconds();
    }
    else
    {
        m_srtt = (1 - m_alpha) * m_srtt + m_alpha * m_tlpRtt.GetSeconds();
    }

    if (m_srtt > 0)
    {
        curr_pto = 2 * Time::FromDouble(m_srtt, Time::S);
        if (inflight == 1)
        {
            curr_pto += Time::FromDouble(0.2, Time::S); // 200ms
        }
        else
        {
            curr_pto += Time::FromDouble(0.002, Time::S); // 2ms
        }
    }
    else
    {
        curr_pto += Time::FromDouble(1, Time::S);
    }

    curr_pto = Min(curr_pto, Time::FromDouble(rto_left, Time::S));
    return curr_pto;
}
} // namespace ns3
