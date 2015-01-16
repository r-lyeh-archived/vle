// check all possible VLEi/VLEu combinations by brute force
// r-lyeh, boost licensed

#include "vle.hpp"

#include <stdint.h>
#include <stdlib.h>

#ifdef ONLY_32BITS
#define IMIN  INT32_MIN
#define IMAX  INT32_MAX
#define UMAX UINT32_MAX
#else
#define IMIN  INT64_MIN
#define IMAX  INT64_MAX
#define UMAX UINT64_MAX
#endif

std::string report;
uint64_t errors = 0;

inline 
void testi( int64_t i ) {
	if( i != vlei::decode(vlei::encode(i)) ) {
		errors++;
		report += std::to_string(i) + "i,";
	} 
	if( ( i & 0xffffff ) == 0 ) {
		printf("%c", '.');
	}
}

inline 
void testu( uint64_t u ) {
	if( u != vleu::decode(vleu::encode(u)) ) {
		errors++;
		report += std::to_string(u) + "u,";
	} 
	if( ( u & 0xffffff ) == 0 ) {
		printf("%c", ',');
	}
}

int main() {

	for( int64_t i = IMIN; i < IMAX; ++i ) {
		testi(i);
	}
	testi(IMAX);

	for( uint64_t u = 0; u < UMAX; ++u ) {
		testu(u);
	}
	testu(UMAX);

	if( report.empty() ) {
		printf("\n%s\n", "All ok.");
	} else {
		printf("\n%d errors: %s\n", errors, report.c_str() );
	}
}
