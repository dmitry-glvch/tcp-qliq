#include "net_routines.hpp"

#include <fstream>
#include <ranges>


namespace {

  namespace v  = std::ranges::views;
  namespace fs = std::filesystem;
  using std::span;

  using boost::asio::use_awaitable;

  using namespace imaqliq::test::net_routines;
  using continuation_ut = std::underlying_type_t<continuation>;


  using chunk_size_t = std::uint_fast16_t;

  constexpr chunk_size_t max_chunk_size { 64 * 1024 - 1 - 60 };
  constexpr chunk_size_t chunk_size { max_chunk_size};
  static_assert (chunk_size <= max_chunk_size, "Incorrent chunk size");

  using chunk_t = std::array<std::byte, chunk_size>;

  struct chunk_info_t {
    std::size_t  regular_chunks;
    chunk_size_t regular_chunk_size;
    chunk_size_t last_chunk_size;
  };

  chunk_info_t calc_chunk_info (std::size_t file_size) {
    return {
      .regular_chunks = (file_size <= chunk_size) ? 0 : file_size / chunk_size,
      .regular_chunk_size = chunk_size,
      .last_chunk_size = (file_size - 1) % chunk_size + 1
    };
  }

}


namespace imaqliq::test::net_routines {

awaitable<void> receive_file_contents (
    socket_t& socket,
    length_t file_size,
    const fs::path& save_path) {

  std::ofstream os { save_path, std::ios::binary | std::ios::out };
  chunk_t buffer;

  const chunk_info_t chunk_info { calc_chunk_info (file_size) };

  const auto receive_chunk = 
      [ &socket, &buffer, &os ] (chunk_size_t size) -> awaitable<void> {
        co_await receive_bytes(socket, span { buffer }.first (size));
        os.write (reinterpret_cast<const char*>( buffer.data () ), size);
        co_await send_continuation (socket);
      };

  for (std::size_t _ : v::iota (0ul, chunk_info.regular_chunks))
    co_await receive_chunk (chunk_info.regular_chunk_size);
  co_await receive_chunk (chunk_info.last_chunk_size);

  os.close();
}

awaitable<void> send_file_contents (
    socket_t& socket,
    length_t file_size,
    const fs::path& file_path) {

  std::ifstream is { file_path, std::ios::in | std::ios::binary };
  chunk_t buffer;

  const chunk_info_t chunk_info { calc_chunk_info (file_size) };

  const auto send_chunk = 
      [ &socket, &buffer, &is ] (chunk_size_t size) -> awaitable<void> {
        is.read(reinterpret_cast<char*>( buffer.data () ), size);
        co_await send_bytes(socket, span { buffer }.first (size));
        co_await receive_continuation (socket);
      };

  for (std::size_t _ : v::iota (0ul, chunk_info.regular_chunks))
    co_await send_chunk (chunk_info.regular_chunk_size);
  co_await send_chunk (chunk_info.last_chunk_size);
}


awaitable<std::string> receive_string (socket_t& socket) {
  
  length_t length { co_await receive_int<length_t> (socket) };

  std::string result;
  result.resize (length);

  co_await receive_bytes (socket, span { result.data () , length });
  co_return result;
}

awaitable<void> send_string (socket_t& socket, std::string_view string) {

  auto length = string.length ();
  if (length > max_string_length)
    throw std::invalid_argument ("String is too long to be sent.");

  co_await send_int<length_t> (socket, length);
  co_await send_bytes (socket, span { string.data (), length });
}


awaitable<continuation> receive_continuation (socket_t& socket) {
  co_return static_cast<continuation> (
      co_await receive_int<continuation_ut> (socket));
}
  
awaitable<void> send_continuation (socket_t& socket, continuation value) {
  co_await send_int<continuation_ut> (
      socket,
      static_cast<continuation_ut>( value ));
}

}
