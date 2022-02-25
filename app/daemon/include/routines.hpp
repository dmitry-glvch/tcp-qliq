#pragma once

#include <boost/asio.hpp>


namespace imaqliq::test::daemon::routines {

boost::asio::awaitable<void>
listen(
  const boost::asio::ip::address& listen_address,
  boost::asio::ip::port_type listen_port
);

boost::asio::awaitable<void>
service (boost::asio::ip::tcp::socket&& socket);

}

