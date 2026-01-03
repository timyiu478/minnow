#include "router.hh"
#include "debug.hh"

#include <iostream>

using namespace std;

// route_prefix: The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
// prefix_length: For this route to be applicable, how many high-order (most-significant) bits of
//    the route_prefix will need to match the corresponding bits of the datagram's destination address?
// next_hop: The IP address of the next hop. Will be empty if the network is directly attached to the router (in
//    which case, the next hop address should be the datagram's final destination).
// interface_num: The index of the interface to send the datagram out on.
void Router::add_route( const uint32_t route_prefix,
                        const uint8_t prefix_length,
                        const optional<Address> next_hop,
                        const size_t interface_num )
{
  cerr << "DEBUG: adding route " << Address::from_ipv4_numeric( route_prefix ).ip() << "/"
       << static_cast<int>( prefix_length ) << " => " << ( next_hop.has_value() ? next_hop->ip() : "(direct)" )
       << " on interface " << interface_num << "\n";

  if ( next_hop.has_value() ) {
    route_table_.push_back( { route_prefix, prefix_length, next_hop.value().ipv4_numeric(), interface_num, true} );
  } else {
    route_table_.push_back( { route_prefix, prefix_length, {}, interface_num, false} );
  }
}

// Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
void Router::route()
{
  for ( auto iface : interfaces_ ) {
    std::queue<InternetDatagram>& queue = iface->datagrams_received();
    while ( !queue.empty() ) {
      InternetDatagram dgram = queue.front();
      queue.pop();

      uint8_t longest_prefix_length = 0;
      uint8_t select_idx;

      for (  uint i = 0 ; i <  route_table_.size(); i++  ) {
        Route r = route_table_[i];
        if ( r.prefix_length == 0 ) { 
          if ( r.route_prefix != dgram.header.dst ) { continue; }
        } else if ( r.route_prefix >> ( 32 - r.prefix_length ) != dgram.header.dst >> ( 32 - r.prefix_length ) || r.prefix_length <= longest_prefix_length ) {
          continue;
        }
        longest_prefix_length = r.prefix_length;
        select_idx = i;
      }

      debug("longest_prefix_length is {}, select_idx is {}", longest_prefix_length, select_idx);
      
      if ( longest_prefix_length > 0 ) {
        if ( dgram.header.ttl == 0 ) { continue; }
        dgram.header.ttl -= 1;
        dgram.header.compute_checksum();

        debug("dgram ttl is {}", dgram.header.ttl);

        Address next_hop = Address::from_ipv4_numeric(dgram.header.dst);
        if ( route_table_[select_idx].has_next_hop ) {
          next_hop = Address::from_ipv4_numeric(route_table_[select_idx].next_hop);
        }

        interface(route_table_[select_idx].interface_num)->send_datagram(dgram, next_hop);
      }
    }
  }
}
