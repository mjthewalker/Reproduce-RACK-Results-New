/*
 * Copyright (c) 2019 NITK Surathkal
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 *
 *
 * Author: Shikha Bakshi <shikhabakshi912@gmail.com>
 *         Mohit P. Tahiliani <tahiliani@nitk.edu.in>
           Keerthana Polkampally <keerthana.keetu.p@gmail.com>
           Archana Priyadarshani Sahoo <archana98priya@gmail.com>
           Durvesh Shyam  Bhalekar <durvesh.5.db@gmail.com>
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

class TcpTlp : public Object
{
  public:
    /**
     * @brief Get the type ID.
     * @return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * @brief Constructor
     */
    TcpTlp();

    /**
     * @brief Copy constructor.
     * @param other object to copy.
     */
    TcpTlp(const TcpTlp& other);

    /**
     * @brief Deconstructor
     */
    virtual ~TcpTlp();

    /**
     * @brief Calculates Pto
     *
     * @param srtt smoother round trip time
     * @param flightsize flight size
     * @param rto retransmission timeout
     */
    Time CalculatePto(Time srtt, uint32_t flightsize, double rto);

  private:
    double m_srtt{0};      //!< Smoothened RTT (SRTT) as specified in [RFC6298]
    double m_alpha{0.125}; //!< EWMA constant for calculation of SRTT (alpha = 1/8)

    Time m_pto{0};    //!< PTO values>
    Time m_tlpRtt{0}; //!< RTT value used for TLP>
};
} // namespace ns3

/* TCP_TLP_H */
