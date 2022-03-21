#include "net_routines.hpp"

#include <fstream>
#include <ranges>

#include <openssl/md5.h>


namespace {

  namespace r = std::ranges;
  namespace v = std::ranges::views;

  using boost::asio::awaitable;
  using boost::asio::use_awaitable;

  using boost::asio::ip::tcp;
  namespace ip = boost::asio::ip;

  using namespace imaqliq::test::net_routines;
  using continuation = imaqliq::test::net_routines::continuation;
  using continuation_ut = std::underlying_type_t<continuation>;

  // template <typename range_t>
  // concept buffer_range =
  //     r::sized_range<range_t> &&
  //     r::common_range<range_t> &&
  //     r::forward_range<range_t>;

  // template <typename range_t>
  // concept output_buffer_range =
  //     buffer_range<range_t> &&
  //     r::output_range<range_t, r::iterator_t<range_t>>;


  using chunk_size_t = uint16_t;

  constexpr chunk_size_t max_chunk_size { 64 * 1024 - 1 - 60 };
  constexpr chunk_size_t chunk_size { max_chunk_size};
  static_assert (chunk_size <= max_chunk_size, "Incorrent chunk size");

  using chunk = std::array<std::byte, chunk_size>;

  constexpr uint8_t checksum_length { 16u };
  using chunk_checksum = std::array<std::byte, checksum_length>;


  chunk_checksum calc_chunk_checksum (const chunk& c) {
    const auto data = reinterpret_cast<const unsigned char*>( c.data () );
    chunk_checksum result;
    MD5 (data, chunk_size, reinterpret_cast<unsigned char*>( result.data () ));
    return result;
  }

  awaitable<void> receive_chunk (
      tcp::socket& socket,
      chunk& buffer,
      chunk_size_t size,
      std::ostream& os) {

    while (true) {

      chunk_checksum received_checksum;
      co_await receive_bytes(socket, received_checksum, checksum_length);
      
      co_await receive_bytes(socket, buffer, size);
      chunk_checksum calced_checksum { calc_chunk_checksum(buffer) };

      const bool checksum_match
          { std::equal(
                received_checksum.begin(),
                received_checksum.end(),
                calced_checksum.begin()) };

      if (checksum_match)
        break;

      co_await send_continuation(socket, continuation::REPREAT);

    }

    os.write(reinterpret_cast<const char*>(buffer.data()), size);
    co_await send_continuation(socket);

  }

  awaitable<void> send_chunk (
      tcp::socket& socket,
      chunk& buffer,
      chunk_size_t size,
      std::istream& is) {

    while (true) {

      is.read(reinterpret_cast<char*>(buffer.data()), size);

      chunk_checksum checksum { calc_chunk_checksum(buffer) };
      const std::byte* checkum_ptr { checksum.data() };
      co_await send_bytes(socket, checkum_ptr, checksum_length);

      co_await send_bytes(socket, buffer, size);

      continuation ack { co_await receive_continuation(socket) };
      if (ack == continuation::OK)
        break;

    }
  }

}


namespace imaqliq::test::net_routines {

awaitable<void> receive_file_contents (
    tcp::socket& socket,
    length_t file_size,
    const std::filesystem::path& save_path) {

  std::ofstream os { save_path };
  std::array<std::byte, chunk_size> buffer;

  const std::size_t full_chunk_count { file_size / chunk_size };
  const std::size_t last_chunk_size { (file_size - 1) % chunk_size + 1 };

  for (auto chunk : v::iota (0ul, full_chunk_count))
    co_await receive_chunk(socket, buffer, chunk_size, os);

  if (last_chunk_size != chunk_size) 
    co_await receive_chunk(socket, buffer, last_chunk_size, os);

  os.close();
}

awaitable<void> send_file_contents (
    tcp::socket& socket,
    length_t file_size,
    const std::filesystem::path& file_path) {

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


awaitable<std::string> receive_string (tcp::socket& socket) {
  
  length_t length { co_await receive_int<length_t>(socket) };

  std::string result (length + 1, '\0');
  co_await receive_bytes(socket, result, length);

  co_return result;
}

awaitable<void> send_string (tcp::socket& socket, std::string_view string) {

  auto length { string.length() };
  if (length > std::numeric_limits<length_t>::max())
    throw std::invalid_argument("String is too long to be sent.");

  co_await send_int<length_t>(socket, string.length());
  co_await send_bytes(socket, string, length);
}

awaitable<void> get_ack_or_throw (
    tcp::socket& socket,
    continuation expected_value) {

  continuation ack { co_await receive_continuation(socket) };
  if (ack != expected_value)
    throw std::runtime_error("Unexpected answer from server");
}

awaitable<continuation> receive_continuation (tcp::socket& socket) {
  co_return static_cast<continuation>(
      co_await receive_int<continuation_ut>(socket));
}
  
awaitable<void> send_continuation (tcp::socket& socket, continuation value) {
  co_await send_int<continuation_ut>(
      socket,
      static_cast<continuation_ut>(value));
}

}