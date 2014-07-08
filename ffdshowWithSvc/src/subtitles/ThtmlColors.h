#ifndef _THTMLCOLORS_H_
#define _THTMLCOLORS_H_

class ThtmlColors : std::hash_map<ffstring,COLORREF>
{
public:
 ThtmlColors(void);
 bool getColor(const ffstring &name,COLORREF *color) const;
 void getColor(const ffstring &name,COLORREF *color,COLORREF def) const;
};

#endif
