# vle

- VLE is a simple variable-length encoder/decoder (C99)(C++03)
- VLE is compact. No matter what type you encode, VLE encodes magnitudes with low overhead.
- VLE is designed for 1/2/3..62/63/64/\*... bits signed/unsigned integers (\*: easily extendable).
- VLE is streamable. Format is 7-bit packing, MSB terminator.
- VLE is embeddable. Header-only. Both C/C++ APIs provided.
- VLE is BOOST licensed.

## TL;DR
- Say you want to serialize an `uint64_t buf[3];`. Instead of flushing a 24-bytes stream, you use VLE.
- VLE serializes 64-bit numbers from 1 byte (best-case) to 10 bytes (worst-case). So this buffer could get encoded from 3 bytes (best-case, savings of 19-bytes) to 30 bytes (worst-case, overhead of 6-bytes).

## Theory
Data is processed into/from a variable length encoding value, where bytes are just 7-bit shifted and added from/into a big accumulator until MSB is found. Additionally, signed integers are pre-encoded to a more efficent bitwise encoding.

## Features
- Encodes/decodes any magnitude integer with low overhead: 8,9,10,11...20..24..40..48..56..63,64 bits.
- Type does not matter. Magnitude of encoded integer determinates size of stream.
  - Ie, big integers that contain small values are serialized to smallest fit:
  - byte(0), short(0), int(0), int64(0) all of them require 1-byte.
  - byte(127), short(127), int(127), int64(127) all of them require 1-byte.
  - byte(255), short(255), int(255), int64(255) all of them require 2-bytes.
  - short(16384), int(16384), int64(16384) all of them require 3-bytes.
  - and so on... (see range tables below).
- Rule is, the smaller the magnitude integer you encode, the smaller the stream you decode. 
  - Negative integers are rearranged to meet this criteria (see appendix).
- VLE applies to serialization, network, binary headers, packets, etc...

## API, C++
```c++
ns vlei {
  string   encode( int64_t );
  int64_t  decode( string );
}
ns vleu {
  string   encode( uint64_t );
  uint64_t decode( string );   
}
```

## API, C
- Basic API. Allows streaming and fine control.
- Encoders do not append null character at end of string.
- Decoders do not need null character at end of string.
- All functions return integer of streamed bytes.
```c
 VLE_API uint64_t vle_encode_u(  uint8_t *buffer, uint64_t value );
 VLE_API uint64_t vle_encode_i(  uint8_t *buffer,  int64_t value );
 VLE_API uint64_t vle_decode_u( uint64_t *value, const uint8_t *buffer );
 VLE_API uint64_t vle_decode_i(  int64_t *value, const uint8_t *buffer );
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

## appendix, some more sign theory

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

