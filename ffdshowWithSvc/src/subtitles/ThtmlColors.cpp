/*
 * Copyright (c) 2005,2006 Milan Cutka
 *
 * gathered from http://www.netwave.or.jp/~shikai/shikai/shcolor.htm
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "ThtmlColors.h"

ThtmlColors::ThtmlColors(void)
{
 static struct
  {
   const char_t *name;
   COLORREF color;
  } const colors[]=
   {
    {_l("white"), 0xffffff},
    {_l("whitesmoke"), 0xf5f5f5},
    {_l("ghostwhite"), 0xf8f8ff},
    {_l("snow"), 0xfffafa},
    {_l("gainsboro"), 0xdcdcdc},
    {_l("lightgrey"), 0xd3d3d3},
    {_l("silver"), 0xc0c0c0},
    {_l("darkgray"), 0xa9a9a9},
    {_l("gray"), 0x808080},
    {_l("dimgray"), 0x696969},
    {_l("lightslategray"), 0x778899},
    {_l("slategray"), 0x708090},
    {_l("darkslategray"), 0x2f4f4f},
    {_l("black"), 0x000000},

    {_l("azure"), 0xf0ffff},
    {_l("aliceblue"), 0xf0f8ff},
    {_l("mintcream"), 0xf5fffa},
    {_l("honeydew"), 0xf0fff0},
    {_l("lightcyan"), 0xe0ffff},
    {_l("paleturqoise"), 0xafeeee},
    {_l("powderblue"), 0xb0e0e6},
    {_l("lightblue"), 0xadd8ed},
    {_l("lightsteelblue"), 0xb0c4de},
    {_l("skyblue"), 0x87ceeb},
    {_l("lightskyblue"), 0x87cefa},
    {_l("cyan"), 0x00ffff},
    {_l("aqua"), 0x00ff80},
    {_l("deepskyblue"), 0x00bfff},
    {_l("aquamarine"), 0x7fffd4},
    {_l("turquoise"), 0x40e0d0},
    {_l("darkturquoise"), 0x00ced1},
    {_l("lightseagreen"), 0x20b2aa},
    {_l("mediumturquoise"), 0x40e0dd},
    {_l("mediumaquamarine"), 0x66cdaa},
    {_l("cadetblue"), 0x5f9ea0},
    {_l("teal"), 0x008080},
    {_l("darkcyan"), 0x008b8b},
    {_l("cornflowerblue"), 0x6495ed},
    {_l("dodgerblue"), 0x1e90ff},
    {_l("steelblue"), 0x4682b4},
    {_l("royalblue"), 0x4169e1},
    {_l("blue"), 0x0000ff},
    {_l("mediumblue"), 0x0000cd},
    {_l("mediumslateblue"), 0x7b68ee},
    {_l("slateblue"), 0x6a5acd},
    {_l("darkslateblue"), 0x483d8b},
    {_l("darkblue"), 0x00008b},
    {_l("midnightblue"), 0x191970},
    {_l("navy"), 0x000080},

    {_l("palegreen"), 0x98fb98},
    {_l("lightgreen"), 0x90ee90},
    {_l("mediumspringgreen"), 0x00fa9a},
    {_l("springgreen"), 0x00ff7f},
    {_l("chartreuse"), 0x7fff00},
    {_l("lawngreen"), 0x7cfc00},
    {_l("lime"), 0x00ff00},
    {_l("limegreen"), 0x32cd32},
    {_l("greenyellow"), 0xadff2f},
    {_l("yellowgreen"), 0x9acd32},
    {_l("darkseagreen"), 0x8fbc8f},
    {_l("mediumseagreen"), 0x3cb371},
    {_l("seagreen"), 0x2e8b57},
    {_l("olivedrab"), 0x6b8e23},
    {_l("forestgreen"), 0x228b22},
    {_l("green"), 0x008000},
    {_l("darkkhaki"), 0xbdb76b},
    {_l("olive"), 0x808000},
    {_l("darkolivegreen"), 0x556b2f},
    {_l("darkgreen"), 0x006400},

    {_l("floralwhite"), 0xfffaf0},
    {_l("seashell"), 0xfff5ee},
    {_l("ivory"), 0xfffff0},
    {_l("beige"), 0xf5f5dc},
    {_l("cornsilk"), 0xfff8dc},
    {_l("lemonchiffon"), 0xfffacd},
    {_l("lightyellow"), 0xffffe0},
    {_l("lightgoldenrodyellow"), 0xfafad2},
    {_l("papayawhip"), 0xffefd5},
    {_l("blanchedalmond"), 0xffedcd},
    {_l("palegoldenrod"), 0xeee8aa},
    {_l("khaki"), 0xf0eb8c},
    {_l("bisque"), 0xffe4c4},
    {_l("moccasin"), 0xffe4b5},
    {_l("navajowhite"), 0xffdead},
    {_l("peachpuff"), 0xffdab9},
    {_l("yellow"), 0xffff00},
    {_l("gold"), 0xffd700},
    {_l("wheat"), 0xf5deb3},
    {_l("orange"), 0xffa500},
    {_l("darkorange"), 0xff8c00},

    {_l("oldlace"), 0xfdf5e6},
    {_l("linen"), 0xfaf0e6},
    {_l("antiquewhite"), 0xfaebd7},
    {_l("lightsalmon"), 0xffa07a},
    {_l("darksalmon"), 0xe9967a},
    {_l("salmon"), 0xfa8072},
    {_l("lightcoral"), 0xf08080},
    {_l("indianred"), 0xcd5c5c},
    {_l("coral"), 0xff7f50},
    {_l("tomato"), 0xff6347},
    {_l("orangered"), 0xff4500},
    {_l("red"), 0xff0000},
    {_l("crimson"), 0xdc143c},
    {_l("firebrick"), 0xb22222},
    {_l("maroon"), 0x800000},
    {_l("darkred"), 0x8b0000},

    {_l("lavender"), 0xe6e6fe},
    {_l("lavenderblush"), 0xfff0f5},
    {_l("mistyrose"), 0xffe4e1},
    {_l("thistle"), 0xd8bfd8},
    {_l("pink"), 0xffc0cb},
    {_l("lightpink"), 0xffb6c1},
    {_l("palevioletred"), 0xdb7093},
    {_l("hotpink"), 0xff69b4},
    {_l("fuchsia"), 0xff00ee},
    {_l("magenta"), 0xff00ff},
    {_l("mediumvioletred"), 0xc71585},
    {_l("deeppink"), 0xff1493},
    {_l("plum"), 0xdda0dd},
    {_l("violet"), 0xee82ee},
    {_l("orchid"), 0xda70d6},
    {_l("mediumorchid"), 0xba55d3},
    {_l("mediumpurple"), 0x9370db},
    {_l("purple"), 0x9370db},
    {_l("blueviolet"), 0x8a2be2},
    {_l("darkviolet"), 0x9400d3},
    {_l("darkorchid"), 0x9932cc},

    {_l("tan"), 0xd2b48c},
    {_l("burlywood"), 0xdeb887},
    {_l("sandybrown"), 0xf4a460},
    {_l("peru"), 0xcd853f},
    {_l("goldenrod"), 0xdaa520},
    {_l("darkgoldenrod"), 0xb8860b},
    {_l("chocolate"), 0xd2691e},
    {_l("rosybrown"), 0xbc8f8f},
    {_l("sienna"), 0xa0522d},
    {_l("saddlebrown"), 0x8b4513},
    {_l("brown"), 0xa52a2a},
    {NULL,0}
   };
 for (int i=0;colors[i].name;i++)
  insert(std::make_pair(colors[i].name,colors[i].color));
}
bool ThtmlColors::getColor(const ffstring &name,COLORREF *color) const
{
 const_iterator c=find(name);
 return c==end()?false:(*color=c->second,true);
}
void ThtmlColors::getColor(const ffstring &name,COLORREF *color,COLORREF def) const
{
 const_iterator c=find(name);
 *color=c==end()?def:c->second;
}
