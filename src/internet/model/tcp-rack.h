/*
 * Copyright (c) 2018 NITK Surathkal
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Shikha Bakshi <shikhabakshi912@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#pragma once

#include "tcp-option-sack.h"
#include "tcp-socket-state.h"

#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/sequence-number.h"
#include "ns3/simulator.h"
#include "ns3/traced-value.h"

namespace ns3
{

class TcpRack : public Object
{
  public:
    /**
     * @brief Get the type ID.
     * @return the object TypeId
     */
    static TypeId GetTypeId(void);

    /**
     * @brief Constructor
     */
    TcpRack();

    /**
     * @brief Copy constructor.
     * @param other object to copy.
     */
    TcpRack(const TcpRack& other);

    /**
     * @brief Deconstructor
     */
    virtual ~TcpRack();

    /**
     * @brief checks if packet1 sent after packet2
     *
     * @param t1 transmission time of packet1
     * @param t2 transmission time of packet2
     * @param seq1 sequence number of packet1
     * @param seq2 sequence number of packet2
     */
    bool SentAfter(Time t1, Time t2, uint32_t seq1, uint32_t seq2);

    /**
     * @brief updates reo_wnd
     *
     * @param reorderSeen whether re-ordering is seen or not
     * @param dsackSeen whether D-SACK block is seen or not
     * @param sndNxt SND.NXT
     * @param sndUna SND.UNA
     * @param tcb Socket State object
     * @param sacked Number of packets sacked
     * @param dupAckThresh Threshold for number of dupacks to enter recovery
     * @param exiting if the connection is exiting Recovery State
     */
    void UpdateReoWnd(bool reorderSeen,
                      bool dsackSeen,
                      SequenceNumber32 sndNxt,
                      SequenceNumber32 sndUna,
                      Ptr<TcpSocketState> tcb,
                      uint32_t sacked,
                      uint32_t dupAckThresh,
                      bool exiting);

    /**
     * @brief Updates the RACK parameters based on the most recently (S)ACKed packet.
     *
     * @param tser echo timestamp
     * @param retrans whether the packet is retransmitted or not
     * @param xmitTs transmission timestamp of the packet
     * @param endSeq end sequence number of the packet
     * @param sndNxt SND.NXT when the RTT is updated
     * @param lastRtt estimated round trip time
     */
    virtual void UpdateStats(uint32_t tser,
                             bool retrans,
                             Time xmitTs,
                             SequenceNumber32 endSseq,
                             SequenceNumber32 sndNxt,
                             Time lastRtt);

    /**
     * @brief returns Reordering Window
     *
     */
    double GetReoWnd() const
    {
        return m_reoWnd;
    }

    /**
     * @brief returns recent transmission time of Rack.packet
     *
     */
    Time GetXmitTs()
    {
        return m_rackXmitTs;
    }

    /**
     * @brief returns Ending sequence of Rack.packet
     *
     */
    uint32_t GetEndSeq()
    {
        return m_rackEndSeq.GetValue();
    }

    /**
     * @brief returns the RTT of the most recently transmitted packet
     *        that has been acknowledged
     *
     */
    Time GetRtt()
    {
        return m_rackRtt;
    }

  private:
    Time m_rackXmitTs{0};             //!< Latest transmission timestamp of Rack.packet
    SequenceNumber32 m_rackEndSeq{0}; //!< Ending sequence number of Rack.packet
    Time m_rackRtt{0};  //!< RTT of the most recently transmitted packet that has been acknowledged
    double m_reoWnd{0}; //!< Re-ordering Window
    Time m_minRtt{0};   //!< Minimum RTT
    SequenceNumber32 m_rttSeq{0}; //!< SND.NXT when RACK.rtt is updated
    bool m_dsack{false}; //!< If a DSACK option has been received since last RACK.reo_wnd change
    uint32_t m_reoWndIncr{1};     //!< Multiplier applied to adjust RACK.reo_wnd
    uint32_t m_reoWndPersist{16}; //!< Number of loss recoveries before resetting RACK.reo_wnd
    double m_srtt{0};             //!< Smoothened RTT (SRTT) as specified in [RFC6298]
    double m_alpha{0.125};        //!< EWMA constant for calculation of SRTT (alpha = 1/8)
};
} // namespace ns3

/* TCP_RACK_H */
