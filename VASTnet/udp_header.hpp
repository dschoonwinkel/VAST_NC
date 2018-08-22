//
// udp_header.hpp
// ~~~~~~~~~~~~~~~
// Modified from IPV4 header by
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef UDP_HEADER_HPP
#define UDP_HEADER_HPP

#include <algorithm>
#include <boost/asio/ip/address_v4.hpp>

// Packet header for UDP.
//
// The wire format of an UDP header is:
// 
// 0               8               16                             31
// +-------+-------+---------------+------------------------------+
// |          Source Port          |         Dest Port            |
// +-------+-------+---------------+-+-+-+------------------------+
// |            Length             |          Checksum            |
// +---------------+---------------+-+-+-+------------------------+
class udp_header
{
public:
  udp_header() { std::fill(rep_, rep_ + sizeof(rep_), 0); }

  unsigned short src_port() const { return decode(0, 1); }
  unsigned short dest_port() const { return decode(2, 3); }
  unsigned short length() const { return decode(4, 5); }
  unsigned short checksum() const { return decode(6, 7); }


  friend std::istream& operator>>(std::istream& is, udp_header& header)
  {
    is.read(reinterpret_cast<char*>(header.rep_), 8);
    return is;
  }

  void print_bytes() {
    for (int i = 0; i < 8; i++) {
      printf("%2X ", rep_[i] & 0xFF);
    }
    printf("\n");
  }

private:
  unsigned short decode(int a, int b) const
    { return (rep_[a] << 8) + rep_[b]; }

  unsigned char rep_[8];
};

#endif // UDP_HEADER_HPP