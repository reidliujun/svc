#ifndef _CSSAUTH_H_
#define _CSSAUTH_H_

extern void CSSkey1(int varient, byte const *challenge, byte *key);
extern void CSSkey2(int varient, byte const *challenge, byte *key);
extern void CSSbuskey(int varient, byte const *challenge, byte *key);

#endif
