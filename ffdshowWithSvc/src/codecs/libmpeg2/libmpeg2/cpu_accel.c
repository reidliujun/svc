/*
 * cpu_accel.c
 * Copyright (C) 2000-2004 Michel Lespinasse <walken@zoy.org>
 * Copyright (C) 1999-2000 Aaron Holtzman <aholtzma@ess.engr.uvic.ca>
 *
 * This file is part of mpeg2dec, a free MPEG-2 video stream decoder.
 * See http://libmpeg2.sourceforge.net/ for updates.
 *
 * mpeg2dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * mpeg2dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"

#include "../../../inttypes.h"

#include "mpeg2.h"
#include "attributes.h"
#include "mpeg2_internal.h"

#if defined(ARCH_X86) || defined(ARCH_X86_64)
static inline uint32_t arch_accel (uint32_t accel)
{
    if (accel & (MPEG2_ACCEL_X86_3DNOW | MPEG2_ACCEL_X86_MMXEXT))
	accel |= MPEG2_ACCEL_X86_MMX;

    if (accel & (MPEG2_ACCEL_X86_SSE2 | MPEG2_ACCEL_X86_SSE3))
	accel |= MPEG2_ACCEL_X86_MMXEXT;

    if (accel & (MPEG2_ACCEL_X86_SSE3))
	accel |= MPEG2_ACCEL_X86_SSE2;

    return accel;
}
#endif /* ARCH_X86 || ARCH_X86_64 */

uint32_t mpeg2_detect_accel (uint32_t accel)
{
#if defined (ARCH_X86) || defined (ARCH_X86_64)
    accel = arch_accel (accel);
#endif
    return accel;
}
