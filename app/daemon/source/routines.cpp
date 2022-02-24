#include "routines.hpp"

#include <span>
#include <string>
#include <iostream>
#include <chrono>
#include <fstream>

#include "net_routines.hpp"


using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
namespace this_coroutine = boost::asio::this_coro;

using boost::asio::ip::tcp;
namespace ip = boost::asio::ip;
namespace net_routines = imaqliq::test::net_routines;


namespace {
  
  std::string get_current_timestamp () {

    using namespace std::chrono;

    time_point current_time { system_clock::now() };
    const auto ticks { current_time.time_since_epoch().count() };

    return std::to_string(ticks);

  }

}


namespace imaqliq::test::daemon::routines {

awaitable<void>
listen (const ip::address& listen_address, ip::port_type listen_port) {

  auto executor { co_await this_coroutine::executor };
  tcp::acceptor acceptor { executor, { listen_address, listen_port } };

  while (true) {
    tcp::socket socket { co_await acceptor.async_accept (use_awaitable) };
    co_spawn (executor, service (std::move(socket)), detached);
  }

}

awaitable<void> service (tcp::socket&& s) {

  tcp::socket socket { std::forward<tcp::socket>(s) };
  co_await net_routines::send_int<uint8_t>(socket, true);

  const uint64_t file_size
      { co_await net_routines::receive_int<uint64_t>(socket) };
  co_await net_routines::send_int<uint8_t>(socket, true);
  
  const std::string file_name { co_await net_routines::receive_string(socket) };
  std::cout << file_name << std::endl;
  co_await net_routines::send_int<uint8_t>(socket, true);

  const std::string file_contents { co_await net_routines::receive_string(socket) };
  std::ofstream os { get_current_timestamp() + '_' + file_name };
  os << file_contents;
  os.close();
  
}


}
