/*

Basic Hashing
 
*/

#ifndef HASH_H
#define HASH_H


namespace NYX {

/* Murmur hash derived from public domain reference implementation at
 *   http://sites.google.com/site/murmurhash/
 *
 * Taken from the VP8 public domain reference encoder implamentation at
 * <add address>  
 */
unsigned int GenerateHash ( const void * key, int len, unsigned int seed );

unsigned int GenerateHash ( const void *key, int len );
    
}

#endif // HASH_H
