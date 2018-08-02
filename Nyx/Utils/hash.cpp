/*

Basic Hashing

*/

#include "hash.h"

#include <cstdlib>
#include <ctime>

namespace NYX {

/* Murmur hash derived from public domain reference implementation at
 *   http://sites.google.com/site/murmurhash/
 *
 * Taken from the VP8 public domain reference encoder implamentation at
 * <add address>  
 */
unsigned int GenerateHash ( const void * key, int len, unsigned int seed )
{
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	unsigned int h = seed ^ len;

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
	{
		unsigned int k;

		k  = data[0];
		k |= data[1] << 8;
		k |= data[2] << 16;
        k |= data[3] << 24;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
			h *= m;
	}

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}
    
unsigned int GenerateHash( const void *key, int len )
{
    std::srand( std::time( nullptr ) );
    return GenerateHash( key, len, std::rand() % 100);
}

}

