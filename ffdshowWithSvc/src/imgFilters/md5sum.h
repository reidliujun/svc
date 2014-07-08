#ifndef AUTH_MD5_H
#define AUTH_MD5_H
/* ========================================================================== **
 *
 *                                    MD5.h
 *
 * Copyright:
 *  Copyright (C) 2003, 2004 by Christopher R. Hertel
 *
 * Email: crh@ubiqx.mn.org
 *
 * Modified for use with MPlayer, detailed CVS changelog at
 * http://www.mplayerhq.hu/cgi-bin/cvsweb.cgi/main/
 *
 * -------------------------------------------------------------------------- **
 *
 * Description:
 *  Implements the MD5 hash algorithm, as described in RFC 1321.
 *
 * -------------------------------------------------------------------------- **
 *
 * License:
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * -------------------------------------------------------------------------- **
 *
 * Notes:
 *
 *  None of this will make any sense unless you're studying RFC 1321 as you
 *  read the code.
 *
 *  MD5 is described in RFC 1321.
 *  The MD*4* algorithm is described in RFC 1320 (that's 1321 - 1).
 *  MD5 is very similar to MD4, but not quite similar enough to justify
 *  putting the two into a single module.  Besides, I wanted to add a few
 *  extra functions to this one to expand its usability.
 *
 *  There are three primary motivations for this particular implementation.
 *  1) Programmer's pride.  I wanted to be able to say I'd done it, and I
 *     wanted to learn from the experience.
 *  2) Portability.  I wanted an implementation that I knew to be portable
 *     to a reasonable number platforms.  In particular, the algorithm is
 *     designed with little-endian platforms in mind, but I wanted an
 *     endian-agnostic implementation.
 *  3) Compactness.  While not an overriding goal, I thought it worth-while
 *     to see if I could reduce the overall size of the result.  This is in
 *     keeping with my hopes that this library will be suitable for use in
 *     some embedded environments.
 *  Beyond that, cleanliness and clarity are always worth pursuing.
 *
 *  As mentioned above, the code really only makes sense if you are familiar
 *  with the MD5 algorithm or are using RFC 1321 as a guide.  This code is
 *  quirky, however, so you'll want to be reading carefully.
 *
 *  Yeah...most of the comments are cut-and-paste from my MD4 implementation.
 *
 * -------------------------------------------------------------------------- **
 *
 * References:
 *  IETF RFC 1321: The MD5 Message-Digest Algorithm
 *       Ron Rivest. IETF, April, 1992
 *
 * ========================================================================== **
 */

/* #include "auth_common.h"   This was in the original. */

/* -------------------------------------------------------------------------- **
 * Typedefs:
 */

struct Tmd5
{
private:
 uint32_t len;
 uint32_t ABCD[4];
 int      b_used;
 uint8_t  block[64];
 static inline uint32_t md5F(uint32_t X,uint32_t Y,uint32_t Z ) {return (X & Y) | ((~(X)) & (Z));}
 static inline uint32_t md5G(uint32_t X,uint32_t Y,uint32_t Z ) {return (X & Z) | ((Y) & (~(Z)));}
 static inline uint32_t md5H(uint32_t X,uint32_t Y,uint32_t Z ) {return  X ^ Y ^ Z;}
 static inline uint32_t md5I(uint32_t X,uint32_t Y,uint32_t Z ) {return  Y ^ (X | ~Z);}

 static inline uint8_t GetLongByte(uint32_t L,int idx ) {return (uint8_t)(( L >> ((idx & 0x03) << 3) ) & 0xFF);}
 static void Permute(uint32_t ABCD[4], const uint8_t block[64]);
public:
 Tmd5(void);
 void sum(const uint8_t *src,unsigned int len);
 void done(uint8_t dst[16]);
};

#endif /* AUTH_MD5_H */
