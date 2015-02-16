vle <a href="https://travis-ci.org/r-lyeh/vle"><img src="https://api.travis-ci.org/r-lyeh/vle.svg?branch=master" align="right" /></a>
===

- VLE is a simple variable-length encoder/decoder (C99)(C++03)
- VLE is simple. Format is 7-bit packing, MSB stream terminator.
- VLE is streamable. Designed to encode and decode integers with low overhead.
- VLE is embeddable. Header-only. No external deps. C and C++ APIs provided.
- VLE is extendable. Signed/unsigned integers of any size (8/16/32/64/...) are supported.
- VLE is BOOST licensed.

## Quick tutorial
- You want to serialize an `struct { uint16_t len; uint64_t buffer[6]; }` to disk, network, etc...
- You could just flush a 50-bytes stream, or you could flush a VLE stream instead.
- This VLE stream will range from `7-bytes` (best-case, 43-bytes saved) up to `63-bytes` (worst-case, 13-bytes overhead).

## Features
- For any 8-bit sequence, the VLE stream will range from `len` up to `2*len` bytes.
- For any 16-bit sequence, the VLE stream will range from `len` up to `3*len` bytes.
- For any 32-bit sequence, the VLE stream will range from `len` up to `5*len` bytes.
- For any 64-bit sequence, the VLE stream will range from `len` up to `10*len` bytes.
- Magnitude of value determinates size of encoded stream. Magnitude of type does not matter.
  - All `byte(0), short(0), int(0), int64(0)...` serialize to a `1-byte` stream.
  - All `byte(127), short(127), int(127), int64(127)...` serialize to a `1-byte` stream.
  - All `byte(128), short(128), int(128), int64(128)...` serialize to a `2-bytes` stream.
  - All `byte(255), short(255), int(255), int64(255)...` serialize to a `2-bytes` stream.
  - All `short(16383), int(16383), int64(16383)...` serialize to a `2-bytes` stream.
  - All `short(16384), int(16384), int64(16384)...` serialize to a `3-bytes` stream.
  - And so on... (see range tables below).
- Rule: The closer to zero integer you encode, the smaller the stream you decode.
  - Note: Negative integers are rearranged to meet this criteria (see appendix).

## VLE stream format
- All non-significative (zero) bits on the left are discarded.
- Bytes are repacked into 7-bit components and glued together until a LSB/MSB bit is found.
- Additionally, signed integers are pre-encoded and post-decoded to a more efficient signed number representation.

## API, C
- Basic API. Allows streaming and fine control.
- Encoders do not append null character at end of string.
- Decoders do not need null character at end of string.
- All functions assume buffers are preallocated to worst-case scenarios.
- All functions return integer of streamed bytes.
```c
 VLE_API uint64_t vle_encode_u(  uint8_t *buffer, uint64_t value );
 VLE_API uint64_t vle_encode_i(  uint8_t *buffer,  int64_t value );
 VLE_API uint64_t vle_decode_u( uint64_t *value, const uint8_t *buffer );
 VLE_API uint64_t vle_decode_i(  int64_t *value, const uint8_t *buffer );
```

## API, C++
```c++
vlei:: {
  string   encode( int64_t );
  int64_t  decode( string );
}
vleu:: {
  string   encode( uint64_t );
  uint64_t decode( string );
}
```

## VLE unsigned, 64-bit ranges
```c++
 1 byte  [0..127]
 2 bytes [128..16383]
 3 bytes [16384..2097151]
 4 bytes [2097152..268435455]
 5 bytes [268435456..34359738367]
 6 bytes [34359738368..4398046511103]
 7 bytes [4398046511104..562949953421311]
 8 bytes [562949953421312..72057594037927935]
 9 bytes [72057594037927936..9223372036854775807]
10 bytes [9223372036854775808..18446744073709551615]
```

## VLE signed, 64-bit ranges
```c++
10 bytes [-9223372036854775808..-4611686018427387905]
 9 bytes [-4611686018427387904..-36028797018963969]
 8 bytes [-36028797018963968..-281474976710657]
 7 bytes [-281474976710656..-2199023255553]
 6 bytes [-2199023255552..-17179869185]
 5 bytes [-17179869184..-134217729]
 4 bytes [-134217728..-1048577]
 3 bytes [-1048576..-8193]
 2 bytes [-8192..-65]
 1 byte  [-64..63]
 2 bytes [64..8191]
 3 bytes [8192..1048575]
 4 bytes [1048576..134217727]
 5 bytes [134217728..17179869183]
 6 bytes [17179869184..2199023255551]
 7 bytes [2199023255552..281474976710655]
 8 bytes [281474976710656..36028797018963967]
 9 bytes [36028797018963968..4611686018427387903]
10 bytes [4611686018427387904..9223372036854775807]
```

## used in
- [collage](https://github.com/r-lyeh/collage), a diff/patch library.
- [bundle](https://github.com/r-lyeh/bundle), a de/compression library.

## appendix: sign theory and conversion functions

| ## | Signed-Magnitude | Complement-2  | VLEi (*)  |
|:--:|:----------------:|:-------------:|:-----:|
| +7 | 0 111 | 0 _111_ | _111_ 0 |
| +6 | 0 110 | 0 _110_ | _110_ 0 |
| +5 | 0 101 | 0 _101_ | _101_ 0 |
| +4 | 0 100 | 0 _100_ | _100_ 0 |
| +3 | 0 011 | 0 _011_ | _011_ 0 |
| +2 | 0 010 | 0 _010_ | _010_ 0 |
| +1 | 0 001 | 0 _001_ | _001_ 0 |
| +0 | 0 000 | 0 _000_ | _000_ 0 |
| -0 | 1 000 | ----- | ----- |
| -1 | 1 001 | 1 **111** | **000** 1 |
| -2 | 1 010 | 1 **110** | **001** 1 |
| -3 | 1 011 | 1 **101** | **010** 1 |
| -4 | 1 100 | 1 **100** | **011** 1 |
| -5 | 1 101 | 1 **011** | **100** 1 |
| -6 | 1 110 | 1 **010** | **101** 1 |
| -7 | 1 111 | 1 **001** | **110** 1 |
| -8 | ---- | 1 **000** | **111** 1 |

(\*): compared to C2 (default signed integer), bits are _shared_ or **inverted**

```c++
c2tovlei(n) {
    return n & MSB ? ~(n<<1) : (n<<1);
}
vleitoc2(n) {
    return n & LSB ? ~(n>>1) : (n>>1);
}
```

