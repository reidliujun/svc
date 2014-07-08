//	VirtualDub - Video processing and capture application
//	Copyright (C) 1998-2001 Avery Lee
//
//	This program is free software; you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation; either version 2 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program; if not, write to the Free Software
//	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "stdafx.h"
#include "base64.h"

static char base64[]=
	"ABCDEFGHIJKLMNOP"
	"QRSTUVWXYZabcdef"
	"ghijklmnopqrstuv"
	"wxyz0123456789+/"
	"=";

void memunbase64(char *t, const char *s, long cnt) {

	char c1, c2, c3, c4, *ind, *limit = t+cnt;
	long v;

	for(;;) {
		while((c1=*s++) && !(ind = strchr(base64,c1)));
		if (!c1) break;
		c1 = (char)(ind-base64);

		while((c2=*s++) && !(ind = strchr(base64,c2)));
		if (!c2) break;
		c2 = (char)(ind-base64);

		while((c3=*s++) && !(ind = strchr(base64,c3)));
		if (!c3) break;
		c3 = (char)(ind-base64);

		while((c4=*s++) && !(ind = strchr(base64,c4)));
		if (!c4) break;
		c4 = (char)(ind-base64);

		// [c1,c4] valid -> 24 bits (3 bytes)
		// [c1,c3] valid -> 18 bits (2 bytes)
		// [c1,c2] valid -> 12 bits (1 byte)
		// [c1] valid    ->  6 bits (0 bytes)

		v = ((c1 & 63)<<18) | ((c2 & 63)<<12) | ((c3 & 63)<<6) | (c4 & 63);

		if (c1!=64 && c2!=64) {
			*t++ = (char)(v >> 16);

			if (t >= limit) return;

			if (c3!=64) {
				*t++ = (char)(v >> 8);
				if (t >= limit) return;
				if (c4!=64) {
					*t++ = (char)v;
					if (t >= limit) return;
					continue;
				}
			}
		}
		break;
	}
}

void membase64(char *t, const char *s, long l) {

	unsigned char c1, c2, c3;

	while(l>0) {
		c1 = (unsigned char)s[0];
		if (l>1) {
			c2 = (unsigned char)s[1];
			if (l>2)
				c3 = (unsigned char)s[2];
		}

		t[0] = base64[(c1 >> 2) & 0x3f];
		if (l<2) {
			t[1] = base64[(c1<<4)&0x3f];
			t[2] = t[3] = '=';
		} else {
			t[1] = base64[((c1<<4)|(c2>>4))&0x3f];
			if (l<3) {
				t[2] = base64[(c2<<2)&0x3f];
				t[3] = '=';
			} else {
				t[2] = base64[((c2<<2) | (c3>>6))&0x3f];
				t[3] = base64[c3 & 0x3f];
			}
		}

		l-=3;
		t+=4;
		s+=3;
	}
	*t=0;
}
