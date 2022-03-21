#include "net_routines.hpp"

#include <fstream>
#include <ranges>

#include <openssl/md5.h>


namespace {

  namespace r  = std::ranges;
  namespace v  = std::ranges::views;
  namespace fs = std::filesystem;
  using std::span;
  using std::byte;
  using std::array;

  using boost::asio::use_awaitable;
  using namespace imaqliq::test::net_routines;
  using continuation_ut = std::underlying_type_t<continuation>;


  using chunk_size_t = std::uint_fast16_t;

  constexpr chunk_size_t max_chunk_size { 64 * 1024 - 1 - 60 };
  constexpr chunk_size_t chunk_size { max_chunk_size};
  static_assert (chunk_size <= max_chunk_size, "Incorrent chunk size");

  using chunk_t = array<byte, chunk_size>;

  constexpr std::size_t checksum_length { 16 };
  using chunk_checksum = array<byte, checksum_length>;


  chunk_checksum calc_chunk_checksum (const chunk_t& chunk) {
    const auto data = reinterpret_cast<const unsigned char*>( chunk.data () );
    chunk_checksum result;
    MD5 (data, chunk_size, reinterpret_cast<unsigned char*>( result.data () ));
    return result;
  }

  awaitable<void> receive_chunk (
      socket_t& socket,
      chunk_t& buffer,
      chunk_size_t size,
      std::ostream& os) {

    while (true) {

      chunk_checksum received_checksum;
      co_await receive_bytes(
          socket,
          span { received_checksum }.first(checksum_length));
      
      co_await receive_bytes(socket, span { buffer }.first(size));

      chunk_checksum calced_checksum { calc_chunk_checksum(buffer) };

      const bool checksum_match
          { std::equal(
                received_checksum.begin(),
                received_checksum.end(),
                calced_checksum.begin()) };

      if (checksum_match)
        break;

      co_await send_continuation(socket, continuation::REPEAT);
    }

    os.write(reinterpret_cast<const char*>(buffer.data()), size);
    co_await send_continuation(socket);
  }

  awaitable<void> send_chunk (
      socket_t& socket,
      chunk_t& buffer,
      chunk_size_t size,
      std::istream& is) {

    while (true) {

      is.read(reinterpret_cast<char*>(buffer.data()), size);

      chunk_checksum checksum { calc_chunk_checksum(buffer) };
      co_await send_bytes(socket, span { checksum } .first(checksum_length));

      co_await send_bytes(socket, span { buffer } .first( size ));

      continuation ack { co_await receive_continuation(socket) };
      if (ack == continuation::OK)
        break;
    }
  }

}


namespace imaqliq::test::net_routines {

awaitable<void> receive_file_contents (
    socket_t& socket,
    length_t file_size,
    const fs::path& save_path) {

  std::ofstream os { save_path };
  chunk_t buffer;

  const std::size_t full_chunk_count { file_size / chunk_size };
  const std::size_t last_chunk_size { (file_size - 1) % chunk_size + 1 };

  for (std::size_t chunk : v::iota (0ul, full_chunk_count))
    co_await receive_chunk(socket, buffer, chunk_size, os);

  if (last_chunk_size != chunk_size) 
    co_await receive_chunk(socket, buffer, last_chunk_size, os);

  os.close();
}

awaitable<void> send_file_contents (
    socket_t& socket,
    length_t file_size,
    const fs::path& file_path) {

  std::ifstream is { file_path, std::ios::in | std::ios::binary };
  std::array<std::byte, chunk_size> buffer;

  const std::size_t full_chunk_count { file_size / chunk_size };
  const std::size_t last_chunk_size { (file_size - 1) % chunk_size + 1 };

  for (std::size_t chunk : v::iota(0ul, full_chunk_count))
    co_await send_chunk(socket, buffer, chunk_size, is);

  if (last_chunk_size != chunk_size) 
    co_await send_chunk(socket, buffer, last_chunk_size, is);

  is.close ();
}


awaitable<std::string> receive_string (socket_t& socket) {
  
  length_t length { co_await receive_int<length_t>(socket) };

  std::string result;
  result.resize(length);

  co_await receive_bytes(socket, span { result.data() , length });
  co_return result;
}

awaitable<void> send_string (socket_t& socket, std::string_view string) {

  auto length = string.length();
  if (length > max_string_length)
    throw std::invalid_argument("String is too long to be sent.");

  co_await send_int<length_t>(socket, length);
  co_await send_bytes(socket, span { string.data(), length });
}


awaitable<continuation> receive_continuation (socket_t& socket) {
  co_return static_cast<continuation>(
      co_await receive_int<continuation_ut>(socket));
}
  
awaitable<void> send_continuation (socket_t& socket, continuation value) {
  co_await send_int<continuation_ut>(
      socket,
      static_cast<continuation_ut>(value));
}

}
