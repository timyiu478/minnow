#include <iostream>

#include "arp_message.hh"
#include "debug.hh"
#include "ethernet_frame.hh"
#include "exception.hh"
#include "helpers.hh"
#include "network_interface.hh"

using namespace std;

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface( string_view name,
                                    shared_ptr<OutputPort> port,
                                    const EthernetAddress& ethernet_address,
                                    const Address& ip_address )
  : name_( name )
  , port_( notnull( "OutputPort", move( port ) ) )
  , ethernet_address_( ethernet_address )
  , ip_address_( ip_address )
{
  cerr << "DEBUG: Network interface has Ethernet address " << to_string( ethernet_address_ ) << " and IP address "
       << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but
//! may also be another host if directly connected to the same network as the destination) Note: the Address type
//! can be converted to a uint32_t (raw 32-bit IP address) by using the Address::ipv4_numeric() method.
void NetworkInterface::send_datagram( InternetDatagram dgram, const Address& next_hop )
{
  // Translate this datagram into an Ethernet frame 
  EthernetFrame eframe;

  eframe.header.src = ethernet_address_;

  uint32_t next_hop_numeric = next_hop.ipv4_numeric();

  // Case 1: If the destination Ethernet address is already known,
  // send it right away.
  if ( cache_.contains(next_hop_numeric) ) {
    eframe.header.dst = cache_[next_hop_numeric];
    eframe.header.type = EthernetHeader::TYPE_IPv4;
    eframe.payload = serialize( dgram );

    debug("Send datagram to this IP address - {}", next_hop_numeric);

    transmit( eframe );
    return;
  }

  // Case 2: The destination Ethernet address is unknown,
  // broadcast an ARP request for the next hop's Ethernet address
  if ( !last_arp_time_.contains(next_hop_numeric) || last_arp_time_[next_hop_numeric] + arp_timeout_ < now_ ) {
    ARPMessage msg;
    msg.opcode = ARPMessage::OPCODE_REQUEST;
    msg.sender_ethernet_address = ethernet_address_;
    msg.sender_ip_address = ip_address_.ipv4_numeric();
    msg.target_ip_address = next_hop_numeric;

    eframe.header.src = ethernet_address_;
    eframe.header.dst = ETHERNET_BROADCAST;
    eframe.header.type = EthernetHeader::TYPE_ARP;
    eframe.payload = serialize( msg );
    
    transmit( eframe );
    
    last_arp_time_[next_hop_numeric] = now_;
  }
  
  // Queue the IP datagram so it can be sent 
  // after the ARP reply is received
  queues_[next_hop_numeric].push({dgram, now_});
}

//! \param[in] frame the incoming Ethernet frame
void NetworkInterface::recv_frame( EthernetFrame frame )
{
  if ( frame.header.type == EthernetHeader::TYPE_IPv4 ) {
    if ( frame.header.dst != ethernet_address_ ) {
      debug("IPv4 datagram not for us");
      return;
    }

    InternetDatagram dgram;
    if ( !parse( dgram, frame.payload ) ) {
      debug("Failed to parse eframe payload to datagram");
      return;
    }

    datagrams_received_.push( dgram );

    return;
  } else if ( frame.header.type == EthernetHeader::TYPE_ARP ) {
    ARPMessage msg;
    if ( !parse( msg, frame.payload ) ) {
      debug("Failed to parse eframe payload to arp msg");
      return;
    }

    // Remember the mapping between the sender’s IP address 
    // and Ethernet address
    // even if the ARP message is not for us
    cache_[msg.sender_ip_address] = msg.sender_ethernet_address;
    last_cache_time_[msg.sender_ip_address] = now_;

    // Send an appropriate ARP reply
    if ( msg.opcode == ARPMessage::OPCODE_REQUEST ) {
      if ( msg.target_ip_address == ip_address_.ipv4_numeric() ) {
        ARPMessage reply;
        reply.opcode = ARPMessage::OPCODE_REPLY;
        reply.sender_ethernet_address = ethernet_address_;
        reply.sender_ip_address = ip_address_.ipv4_numeric();
        reply.target_ethernet_address = msg.sender_ethernet_address;
        reply.target_ip_address = msg.sender_ip_address;

        EthernetFrame eframe;

        eframe.header.src = ethernet_address_;
        eframe.header.dst = msg.sender_ethernet_address;
        eframe.header.type = EthernetHeader::TYPE_ARP;
        eframe.payload = serialize( reply );

        transmit( eframe );
      }

      debug("ARP request not for us");
    } else if ( msg.opcode == ARPMessage::OPCODE_REPLY ) {
      if ( frame.header.dst != ethernet_address_ || msg.target_ip_address != ip_address_.ipv4_numeric() ) {
        debug("ARP reply not for us");
      }
    }

    // Send any queued datagrams to this IP address
    while ( !queues_[msg.sender_ip_address].empty() ) {
      DgramEntry entry = queues_[msg.sender_ip_address].front();
      queues_[msg.sender_ip_address].pop();
      if ( entry.timestamp + arp_timeout_ <= now_ ) {
        continue;
      }
      send_datagram( entry.dgram, Address::from_ipv4_numeric( msg.sender_ip_address ) );
    }

    return;
  }

  debug("Received frame with wrong type {}", frame.header.type);
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick( const size_t ms_since_last_tick )
{
  now_ += ms_since_last_tick;

  // Expire any IP-to-Ethernet mappings that have expired
  for ( auto it = cache_.begin(); it != cache_.end(); )
  {
    if ( last_cache_time_[it->first] + cache_TTL_ <= now_ ) {
      it = cache_.erase(it);
    } else {
      ++it;
    }
  }
}
