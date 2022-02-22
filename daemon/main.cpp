#include <iostream>

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>

#include "deamonize.hpp"
#include "routines.hpp"


int main (int argc, char* argv[]) {

  try {

    if (argc != 3) {
      std::cerr << "Wrong usage.\n"
                   "Usage: " << argv[0] << " <ip> <port>" << std::endl;
      return EXIT_FAILURE;
    }

    namespace ip     = boost::asio::ip;
    namespace daemon = imaqliq::test::daemon;

    const auto listen_address { ip::make_address(argv[1]) };
    const auto listen_port    { boost::lexical_cast<ip::port_type>(argv[2]) };


    // daemon::deamonize();

    boost::asio::io_context io_context { 1 };

    boost::asio::signal_set signals { io_context, SIGHUP, SIGTERM };
    signals.async_wait (
      [&io_context]
      (const boost::system::error_code&, int signal) {
        std::cout << "Received signal " << signal << ".\n"
                     "Gracefully shutting down." << std::endl;
        io_context.stop ();
      }
    );

    boost::asio::co_spawn (
      io_context,
      daemon::routines::listen (listen_address, listen_port),
      boost::asio::detached
    );

    io_context.run ();

  } catch (const boost::system::system_error& e) {

    const boost::system::error_code& code = e.code();
    std::cerr << code.value() << ": " << code.message() << std::endl;
    return code.value(); 

  } catch (const std::exception& e) {

    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;

  }

}
