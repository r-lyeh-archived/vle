/* Simple variable-length encoder/decoder (C99)(C++03)
 * - rlyeh, boost licensed.
 */

/*
 * C++ API
 * Quick API.
 *
 * ns vlei {
 *   string   encode( int64_t );
 *   int64_t  decode( string );
 * }
 * ns vleu {
 *   string   encode( uint64_t );
 *   uint64_t decode( string );   
 * }
 */

/* C API
 * Basic API. Allows streaming and fine control.
 * Encoders do not append null character at end of string.
 * Decoders do not need null character at end of string.
 * All functions return number of streamed bytes.
 *
 * - VLE_API uint64_t vle_encode_u(  uint8_t *buffer, uint64_t value );
 * - VLE_API uint64_t vle_encode_i(  uint8_t *buffer,  int64_t value );
 * - VLE_API uint64_t vle_decode_u( uint64_t *value, const uint8_t *buffer );
 * - VLE_API uint64_t vle_decode_i(  int64_t *value, const uint8_t *buffer );
 */

#ifndef __VLE_H__
#define __VLE_H__

#include <stdint.h>

#ifdef __cplusplus
#define VLE_API extern "C" static inline
#else
#define VLE_API            static inline
#endif

enum { VLE_MIN_REQ_BYTES = 1, VLE_MAX_REQ_BYTES = 10 };

VLE_API uint64_t vle_encode_u( uint8_t *buffer, uint64_t value ) {
    /* 7-bit packing. MSB terminates stream */
    const uint8_t *buffer0 = buffer;
    do {
        *buffer++ = (uint8_t)( 0x80 | (value & 0x7f) );
        value >>= 7;
    } while( value > 0 );
    *(buffer-1) ^= 0x80;
    return buffer - buffer0;
}
VLE_API uint64_t vle_decode_u( uint64_t *value, const uint8_t *buffer ) {
    /* 7-bit unpacking. MSB terminates stream */
    const uint8_t *buffer0 = buffer;
    uint64_t out = 0, j = -7;
    do {
        out |= (( ((uint64_t)(*buffer)) & 0x7f) << (j += 7) );
    } while( ((uint64_t)(*buffer++)) & 0x80 );
    *value = out;
    return buffer - buffer0;
}

VLE_API uint64_t vle_encode_i( uint8_t *buffer, int64_t value ) {
    /* convert sign|magnitude to magnitude|sign */
    uint64_t nv = (uint64_t)value;
    nv = nv & (1ull << 63) ? ~(nv << 1) : (nv << 1);
    /* encode unsigned */
    return vle_encode_u( buffer, nv );
}
VLE_API uint64_t vle_decode_i( int64_t *value, const uint8_t *buffer ) {
    /* decode unsigned */
    uint64_t nv, ret = vle_decode_u( &nv, buffer );
    /* convert magnitude|sign to sign|magnitude */
    *value = nv & (1) ? ~(nv >> 1) : (nv >> 1);
    return ret;
}

#ifdef __cplusplus
#include <string>

namespace vlei {
    static inline
    std::string encode( int64_t value ) {
        unsigned char buf[VLE_MAX_REQ_BYTES + 1];
        buf[ vle_encode_i( &buf[0], value ) ] = (unsigned char)'\0';
        return (const char *)&buf[0];
    }

    static inline
    int64_t decode( const std::string &buf ) {
        int64_t value;
        vle_decode_i( &value, (const unsigned char *)&buf[0] );
        return value;
    }
}

namespace vleu {
    static inline
    std::string encode( uint64_t value ) {
        unsigned char buf[VLE_MAX_REQ_BYTES + 1];
        buf[ vle_encode_u( &buf[0], value ) ] = (unsigned char)'\0';
        return (const char *)&buf[0];
    }

    static inline
    uint64_t decode( const std::string &buf ) {
        uint64_t value;
        vle_decode_u( &value, (const unsigned char *)&buf[0] );
        return value;
    }
}

#endif

#endif
