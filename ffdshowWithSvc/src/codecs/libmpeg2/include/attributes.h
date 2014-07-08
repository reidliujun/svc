/*
 * attributes.h
 * Copyright (C) 2000-2003 Michel Lespinasse <walken@zoy.org>
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

#ifndef LIBMPEG2_ATTRIBUTES_H
#define LIBMPEG2_ATTRIBUTES_H

/* use GCC and MSVC attribs to align critical data structures */
#if defined(__GNUC__)
#define DECLARE_ALIGNED(n,t,v)      t v __attribute__ ((aligned (n)))
#define DECLARE_ASM_CONST(n,t,v)    static const t v attribute_used __attribute__ ((aligned (n)))
#elif defined(_MSC_VER)
#define DECLARE_ALIGNED(n,t,v)      __declspec(align(n)) t v
#define DECLARE_ASM_CONST(n,t,v)    __declspec(align(n)) static const t v
#else
#define DECLARE_ALIGNED(n,t,v)      t v
#define DECLARE_ASM_CONST(n,t,v)    static const t v
#endif

#if defined(__GNUC__) && (__GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ > 1)
#   define GCC420_OR_NEWER 1
#else
#   define GCC420_OR_NEWER 0
#endif

#ifndef attribute_align_arg
#if GCC420_OR_NEWER
#    define attribute_align_arg __attribute__((force_align_arg_pointer))
#else
#    define attribute_align_arg
#endif
#endif

#ifdef HAVE_BUILTIN_EXPECT
#define likely(x) __builtin_expect ((x) != 0, 1)
#define unlikely(x) __builtin_expect ((x) != 0, 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#endif /* LIBMPEG2_ATTRIBUTES_H */
