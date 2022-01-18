//
// Created by Chris Luttio on 3/24/21.
//

#ifndef P8_WEB_SERVER_UTILS_H
#define P8_WEB_SERVER_UTILS_H

#include <vector>
#include <string>
#include <memory>
#include <string>
#include <stdexcept>
#include <chrono>
#include "objects/general/route.h"

std::vector<std::string> split_string(const std::string& separator, const std::string& str);
std::string join(const std::string& separator, const std::vector<std::string>& arr);
int32_t decode_hex_str(const std::string&);
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
    int size_s = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    auto size = static_cast<size_t>( size_s );
    auto buf = std::make_unique<char[]>( size );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}

std::string generate_hash_id(int);

std::chrono::high_resolution_clock::time_point get_time();

uint32_t get_millisecond_duration(std::chrono::high_resolution_clock::time_point a, std::chrono::high_resolution_clock::time_point b);

uint32_t get_ms_to_now(std::chrono::high_resolution_clock::time_point t);

std::vector<std::string> parse_accept_header(const std::string& header);

bool compare_routes(const std::string& a, const std::string& b);

bool route_applies(const std::string&, const std::string&);

std::string get_ssl_str_err();

std::string encode_base_64(const uint8_t *, size_t);

uint32_t convert_from_big_endian(const char*, uint8_t);

std::string get_time_str(std::chrono::system_clock::time_point);

::std::string decode_base64(const ::std::string &ascdata);

std::string get_local_part(const std::string&);
std::string get_domain(const std::string&);
std::vector<std::string> parse_recipients(const std::string&);

#endif //P8_WEB_SERVER_UTILS_H
