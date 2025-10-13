/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Shikha Bakshi <shikhabakshi912@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */
#include "ns3/log.h"
#include "ns3/tcp-westwood-plus.h"
#include "tcp-general-test.h"
#include "ns3/simple-channel.h"
#include "ns3/node.h"
#include "tcp-error-model.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("TcpTlpTest");

class TcpTlpTest : public TcpGeneralTest
{
public:
  /**
   * \brief Constructor
   * \param congControl Type of congestion control.
   * \param seqToKill Sequence number of the packet to drop.
   * \param rack Boolean variable to enable or disable RACK.
   * \param msg Test message.
   */
  TcpTlpTest (TypeId congControl, uint32_t seqToKill, bool tlp, const std::string &msg);

  virtual Ptr<ErrorModel> CreateSenderErrorModel ();
  virtual Ptr<ErrorModel> CreateReceiverErrorModel ();

  virtual Ptr<TcpSocketMsgBase> CreateSenderSocket (Ptr<Node> node);

protected:


  virtual void CongStateTrace (const TcpSocketState::TcpCongState_t oldValue,
                               const TcpSocketState::TcpCongState_t newValue);

  virtual void CWndTrace (uint32_t oldValue, uint32_t newValue);

  /**
   * \brief Check if the packet being dropped is the right one.
   * \param ipH IPv4 header.
   * \param tcpH TCP header.
   * \param p The packet.
   */
  void PktDropped (const Ipv4Header &ipH, const TcpHeader& tcpH, Ptr<const Packet> p);

  virtual void ConfigureProperties ();
  virtual void ConfigureEnvironment ();


  bool m_pktDropped;      //!< The packet has been dropped.
  bool m_pktWasDropped;   //!< The packet was dropped (according to the receiver).
  uint32_t m_seqToKill;   //!< Sequence number to drop.
  bool m_tlp;            //!< Enable/Disable RACK.

  Ptr<TcpSeqErrorModel> m_errorModel; //!< Error model.
};

TcpTlpTest::TcpTlpTest (TypeId typeId, uint32_t seqToKill, bool tlp,
                                  const std::string &msg)
  : TcpGeneralTest (msg),
    m_pktDropped (false),
    m_seqToKill (seqToKill),
    m_tlp (tlp)
{
  m_congControlTypeId = typeId;
}

void
TcpTlpTest::ConfigureProperties ()
{
  TcpGeneralTest::ConfigureProperties ();
  SetInitialSsThresh (SENDER, 0);
}

void
TcpTlpTest::ConfigureEnvironment ()
{
  TcpGeneralTest::ConfigureEnvironment ();
  SetAppPktCount (100);
}

Ptr<ErrorModel>
TcpTlpTest::CreateSenderErrorModel ()
{
  return 0;
}

Ptr<ErrorModel>
TcpTlpTest::CreateReceiverErrorModel ()
{
  m_errorModel = CreateObject<TcpSeqErrorModel> ();
  m_errorModel->AddSeqToKill (SequenceNumber32 (m_seqToKill));
  m_errorModel->SetDropCallback (MakeCallback (&TcpTlpTest::PktDropped, this));

  return m_errorModel;
}


Ptr<TcpSocketMsgBase>
TcpTlpTest::CreateSenderSocket (Ptr<Node> node)
{
  Ptr<TcpSocketMsgBase> socket = TcpGeneralTest::CreateSenderSocket (node);
  socket->SetAttribute ("MinRto", TimeValue (Seconds (10.0)));
  socket->SetAttribute ("Sack", BooleanValue (true));
//  socket->SetAttribute ("Dsack", BooleanValue (true));
  if (m_tlp)
    {
      socket->SetAttribute ("Tlp", BooleanValue (true));
    }

  return socket;
}

void
TcpTlpTest::PktDropped (const Ipv4Header &ipH, const TcpHeader& tcpH, Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (this << ipH << tcpH);

  m_pktDropped = true;

  NS_TEST_ASSERT_MSG_EQ (tcpH.GetSequenceNumber (), SequenceNumber32 (m_seqToKill),
                         "Packet dropped but sequence number differs");
}

void
TcpTlpTest::CongStateTrace (const TcpSocketState::TcpCongState_t oldValue,
                                 const TcpSocketState::TcpCongState_t newValue)
{
  NS_LOG_FUNCTION (this << oldValue << newValue);

  if (oldValue == TcpSocketState::CA_OPEN && m_tlp)
  {
    NS_TEST_ASSERT_MSG_EQ (newValue, TcpSocketState::CA_DISORDER, "TLP not working");
  }
  else if (oldValue == TcpSocketState::CA_OPEN && !m_tlp)
  {
    NS_TEST_ASSERT_MSG_EQ (newValue, TcpSocketState::CA_LOSS, "Something working");
  }
}

void
TcpTlpTest::CWndTrace (uint32_t oldValue, uint32_t newValue)
{
  NS_LOG_FUNCTION (this << oldValue << newValue);

  if (m_tlp)
    {
      std::ofstream fPlotQueue ("tlp-on.plotme", std::ios::out | std::ios::app);
      fPlotQueue << Simulator::Now ().GetSeconds () << " " << newValue/500.0 << std::endl;
      fPlotQueue.close ();
    }
  else
    {
      std::ofstream fPlotQueue ("tlp-off.plotme", std::ios::out | std::ios::app);
      fPlotQueue << Simulator::Now ().GetSeconds () << " " << newValue/500.0 << std::endl;
      fPlotQueue.close ();
    }
}
/**
 * \ingroup internet-test
 * \ingroup tests
 *
 * \brief Testsuite for the RACK
 */
class TcpTlpTestSuite : public TestSuite
{
public:
  TcpTlpTestSuite () : TestSuite ("tcp-tlp-test", UNIT)
  {
    AddTestCase (new TcpTlpTest (TcpNewReno::GetTypeId (), 49501, true, "Tlp testing"), TestCase::QUICK);
    AddTestCase (new TcpTlpTest (TcpNewReno::GetTypeId (), 49501, false, "Tlp testing"), TestCase::QUICK);
  }
};

static TcpTlpTestSuite g_TcpTlpTestSuite; //!< Static variable for test initialization


