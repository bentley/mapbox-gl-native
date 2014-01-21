#ifndef LLMR_UTIL_PBF
#define LLMR_UTIL_PBF

/*
 * Some parts are from upb - a minimalist implementation of protocol buffers.
 *
 * Copyright (c) 2008-2011 Google Inc.  See LICENSE for details.
 * Author: Josh Haberman <jhaberman@gmail.com>
 */

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>


namespace llmr {

struct pbf {
    struct exception : std::exception {};
    struct unterminated_varint_exception : exception {};
    struct varint_too_long_exception : exception {};
    struct unknown_field_type_exception : exception {};
    struct end_of_buffer_exception : exception {};

    inline pbf(const unsigned char *data, uint32_t length);

    inline bool next();
    template <typename T = uint32_t> inline T varint();
    template <typename T = uint32_t> inline T svarint();
    inline std::string string();
    inline float float32();
    inline double float64();
    inline bool boolean();

    inline void skip();
    inline void skipValue(uint32_t val);
    inline void skipBytes(uint32_t bytes);

    const uint8_t *data;
    const uint8_t *end;
    uint32_t value;
    uint32_t tag;
};

pbf::pbf(const unsigned char *data, uint32_t length)
    : data(data),
      end(data + length) {
}

bool pbf::next() {
    if (data < end) {
        value = (uint32_t)varint();
        tag = value >> 3;
        return true;
    }
    return false;
}


template <typename T>
T pbf::varint() {
    uint8_t byte = 0x80;
    T result = 0;
    int bitpos;
    for (bitpos = 0; bitpos < 70 && (byte & 0x80); bitpos += 7) {
        if (data >= end) {
            throw unterminated_varint_exception();
        }
        result |= ((T)(byte = *data) & 0x7F) << bitpos;

        data++;
    }
    if (bitpos == 70 && (byte & 0x80)) {
        throw varint_too_long_exception();
    }

    return result;
}

template <typename T>
T pbf::svarint() {
    T n = varint<T>();
    return (n >> 1) ^ -(T)(n & 1);
}

std::string pbf::string() {
    uint32_t bytes = (uint32_t)varint();
    const char *string = (const char *)data;
    skipBytes(bytes);
    return std::string(string, bytes);
}

float pbf::float32() {
    skipBytes(4);
    float result;
    memcpy(&result, data - 4, 4);
    return result;
}

double pbf::float64() {
    skipBytes(8);
    double result;
    memcpy(&result, data - 8, 8);
    return result;
}

bool pbf::boolean() {
    skipBytes(1);
    return *(bool *)(data - 1);
}

void pbf::skip() {
    skipValue(value);
}

void pbf::skipValue(uint32_t val) {
    switch (val & 0x7) {
        case 0: // varint
            varint();
            break;
        case 1: // 64 bit
            skipBytes(8);
            break;
        case 2: // string/message
            skipBytes((uint32_t)varint());
            break;
        case 5: // 32 bit
            skipBytes(4);
            break;
        default:
            throw unknown_field_type_exception();
    }
}

void pbf::skipBytes(uint32_t bytes) {
    data += bytes;
    if (data > end) {
        throw end_of_buffer_exception();
    }
}

} // end namespace llmr

#endif
