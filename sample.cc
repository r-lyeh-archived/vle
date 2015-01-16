#include "vle.hpp"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

std::string report;
uint64_t errors = 0;

void quick_tests() {
     int64_t is[] = { 1ull << 63, +(1ll << 63), -(1ull << 63), INT64_MIN, INT64_MIN + 1, INT64_MIN / 2, INT64_MIN / 4, -0, 0,  INT64_MAX / 4,  INT64_MAX / 2,  INT64_MAX - 1,  INT64_MAX };
    uint64_t us[] = { 1ull << 63, +(1ll << 63), -(1ull << 63),                                                         -0, 0, UINT64_MAX / 4, UINT64_MAX / 2, UINT64_MAX - 1, UINT64_MAX };

    for( auto i : is ) {
        if( i != vlei::decode(vlei::encode(i)) ) {
            errors++;
            report += std::to_string(i) + "i,";
        } 
    }

    for( auto u : us ) {
        if( u != vleu::decode(vleu::encode(u)) ) {
            errors++;
            report += std::to_string(u) + "u,";
        } 
    }   
}


#include <string>
#include <iostream>

int main( int argc, const char **argv )
{
    // signed
    {
        // negative
        int64_t capacity = ~0u, i0 = -(1ll << 63);
        for( int32_t j = 64; j-- >= 0; ) {
            int64_t i = -(1ll << j);
            auto len = vlei::encode( i ).size();
            assert( i == vlei::decode( vlei::encode( i ) ) );
            if( len < capacity && j < 63 ) {
                capacity = len;
                printf("%2llu bytes [%lld..%lld]\n", capacity + 1, i0, j < 0 ? 0 : i - 1 ); 
                i0 = i;
            }
        }
        // positive
        capacity = 1, i0 = 0;
        for( uint32_t j = 0; j <= 63; ++j ) {
            int64_t i = 1ll << j;
            auto len = vlei::encode( i ).size();
            assert( i == vlei::decode( vlei::encode( i ) ) );
            if( len > capacity || j == 63 ) {
                printf("%2llu bytes [%llu..%llu]\n", capacity, i0, (i-1) ); 
                capacity = len;
                i0 = i;
            }
        }
    }

    // unsigned, positive
    {
        uint64_t capacity = 1, i0 = 0;
        for( uint32_t j = 0; j <= 64; ++j ) {
            uint64_t i = 1ull << j;
            auto len = vleu::encode( i ).size();
            assert( i == vleu::decode( vleu::encode( i ) ) );
            if( len > capacity || j == 64 ) {
                printf("%2llu bytes [%llu..%llu]\n", capacity, i0, (i-1) ); 
                capacity = len;
                i0 = i;
            }
        }
    }

    if( argc > 1 ) {
        // usage: 
        // sample 128
        // sample -128
        // sample 128u

        std::string arg( argv[1] );
        if( arg.back() == 'u' ) {
            arg.pop_back();
            uint64_t value = std::strtoull( arg.c_str(), 0, 0 );
            std::string encoded = vleu::encode(value);
            int64_t decoded = vleu::decode(encoded);
            assert( value == decoded );
            printf("%llu encodes into %d byte(s)\n", value, encoded.size() ); 
        } else {
            int64_t value = std::strtoll( arg.c_str(), 0, 0 );
            std::string encoded = vlei::encode(value);
            int64_t decoded = vlei::decode(encoded);
            assert( value == decoded );
            printf("%lld encodes into %d byte(s)\n", value, encoded.size() ); 
        }
    } 

    quick_tests();

    if( report.empty() ) {
        printf("%s\n", "All ok.");
    } else {
        printf("%lld errors: %s\n", errors, report.c_str());
    }
}
