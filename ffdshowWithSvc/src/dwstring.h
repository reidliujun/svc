//=============================================================================
// File:       dwstring.h
// Contents:   Declarations for DwString
// Maintainer: Doug Sauder <dwsauder@fwb.gulf.net>
// WWW:        http://www.fwb.gulf.net/~dwsauder/mimepp.html
//
// Copyright (c) 1996, 1997 Douglas W. Sauder
// All rights reserved.
//
// IN NO EVENT SHALL DOUGLAS W. SAUDER BE LIABLE TO ANY PARTY FOR DIRECT,
// INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF
// THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF DOUGLAS W. SAUDER
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// DOUGLAS W. SAUDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT
// NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
// BASIS, AND DOUGLAS W. SAUDER HAS NO OBLIGATION TO PROVIDE MAINTENANCE,
// SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
//
//=============================================================================

#ifndef DW_STRING_H
#define DW_STRING_H

#include "char_t.h"

#include "safe_bool.h"

//=============================================================================
// DwStringRep is an implementation class that should not be used externally.
//=============================================================================

template<class tchar> struct DwStringRep
{
 DwStringRep(tchar* aBuf, size_t aSize);
 //DwStringRep(FILE* aFile, size_t aSize);
 ~DwStringRep();
 // void* operator new(size_t);
 // void operator delete(void*, size_t);
 size_t mSize;
 tchar* mBuffer;
 int mRefCount, mPageMod;
//private:
 // memory management
 // DwStringRep* mNext;
 // static DwStringRep* theirPool;
 // static int theirPoolCount;
public:
 void CheckInvariants() const;
};


//=============================================================================
//+ Name DwString -- String class
//+ Description
//. {\tt DwString} is the workhorse of the MIME++ library.  Creating, parsing,
//. or otherwise manipulating MIME messages is basically a matter of
//. manipulating strings.  {\tt DwString} provides all the basic functionality
//. required of a string object, including copying, comparing, concatenating,
//. and so on.
//.
//. {\tt DwString} is similar to the {\tt string} class that is part of
//. the proposed ANSI standard C++ library.  Some of the member functions
//. present in the ANSI {\tt string} are not present in {\tt DwString}:
//. mostly these are the functions that deal with iterators.  {\tt DwString}
//. also includes some member functions and class utility functions that
//. are not a part of the ANSI {\tt string} class.  These non-ANSI
//. functions are easy to distinguish: they all begin with upper-case
//. letters, and all ANSI functions begin with lower-case letters.  The
//. library classes themselves use only the ANSI {\tt string} functions.
//. At some point in the future, MIME++ will probably allow the option to
//. substitute the ANSI {\tt string} class for {\tt DwString}.
//.
//. {\tt DwString} makes extensive use of copy-on-write, even when extracting
//. substrings.  It is this feature that distiguishes {\tt DwString} from most
//. other string classes.  {\tt DwString} also handles binary data, which can
//. contain embedded NUL characters.
//=============================================================================
//+ Noentry _copy _replace Length AsCharBuf Substring Prefix Suffix Prepend
//+ Noentry Append Insert Replace Delete mRep mStart mLength sEmptyString
//+ Noentry ~DwString

class CUnknown;

template<class tchar> class DwString :public safe_bool< DwString<tchar> >
{
private:
 void init1(const tchar*);
 void init2(const DwString<tchar>& aStr, size_t aPos, size_t aLen);
public:
    static CUnknown* WINAPI CreateInstance(LPUNKNOWN punk,HRESULT *phr)
     {
      return NULL;
     }

    static const size_t npos;
    //. {\tt npos} is assigned the value (size_t)-1.

    DwString();
    DwString(const DwString<char>& aStr, size_t aPos=0, size_t aLen=npos);
    DwString(const DwString<wchar_t>& aStr, size_t aPos=0, size_t aLen=npos);
    DwString(const tchar* aBuf, size_t aLen);
    //DwString(FILE* aFile , size_t aLen);
    DwString(const char* aCstr);
    DwString(const wchar_t* aCstr);
    DwString(size_t aLen, tchar aChar);
    DwString(tchar* aBuf, size_t aSize, size_t aStart, size_t aLen);
    //. The first constructor is the default constructor, which sets the
    //. {\tt DwString} object's contents to be empty.
    //.
    //. The second constructor is the copy constructor, which copies at most
    //. {\tt aLen} characters beginning at position
    //. {\tt aPos} from {\tt aStr} to the new {\tt DwString} object.  It will
    //. not copy more characters than what are available in {\tt aStr}.
    //. {\tt aPos} must be less than or equal to {\tt aStr.size()}.
    //.
    //. The third constructor copies {\tt aLen} characters from the buffer
    //. {\tt aBuf} into the new {\tt DwString} object. {\tt aBuf} need not be
    //. NUL-terminated and may contain NUL characters.
    //.
    //. The fourth constructor copies the contents of the NUL-terminated string
    //. {\tt aCstr} into the new {\tt DwString} object.
    //.
    //. The fifth constructor sets the contents of the new {\tt DwString}
    //. object to be the character {\tt aChar} repeated {\tt aLen} times.
    //.
    //. The sixth constructor is an {\it advanced} constructor that sets
    //. the contents of the new {\tt DwString} object to the {\tt aLen}
    //. characters starting at offset {\tt aStart} in the buffer {\tt aBuf}.
    //. {\tt aSize} is the allocated size of {\tt aBuf}.
    //. This constructor is provided for efficiency in setting a new
    //. {\tt DwString}'s contents from a large buffer.  It is efficient
    //. because no copying takes place.  Instead, {\tt aBuf} becomes the
    //. buffer used internally by the {\tt DwString} object, which
    //. takes responsibility for deleting the buffer.
    //. Because {\tt DwString} will free the buffer using {\tt delete []},
    //. the buffer should have been allocated using {\tt new}.
    //. See also: TakeBuffer(), and ReleaseBuffer().

    ~DwString();

    static DwString intToStr(int i);

    DwString& operator = (const DwString& aStr)
     {
      return assign(aStr);
     }
    inline DwString& operator = (const char* aCstr);
    inline DwString& operator = (const wchar_t* aCstr);

    DwString& operator = (tchar aChar)
     {
      return assign(1, aChar);
     }
    //. Assigns the contents of the operand to this string.
    //. {\tt aCstr} must point to a NUL-terminated array of characters
    //. (a C string).
    //. Returns {\tt *this}.

    size_t size() const
     {
      return mLength;
     }
    //. Returns the number of characters in this string's contents.  This
    //. member function is identical to {\tt length()}

    size_t length() const
     {
      return mLength;
     }
    //. Returns the number of characters in this string's contents.  This
    //. member function is identical to {\tt size()}

    size_t max_size() const;
    //. Returns the maximum length that this string can ever attain.

    void resize(size_t aLen, tchar aChar);
    void resize(size_t aLen);
    //. Changes the length of this string. If the string shortened, the final
    //. characters are truncated. If the string is expanded, the added
    //. characters will be NULs or the character specified by {\tt aChar}.

    size_t capacity() const
     {
      return mRep->mSize - 1;
     }
    //. Returns the size of the internal buffer used for this string, which
    //. will always be greater than or equal to the length of the string.

    void reserve(size_t aSize);
    //. If {\tt aSize} is greater than the current capacity of this string,
    //. this member function will increase the capacity to be at least
    //. {\tt aSize}.

    void clear();
    //. Sets this string's contents to be empty.

    inline bool empty() const
     {
      return mLength == 0;
     }
    //. Returns a true value if and only if the contents of this string
    //. are empty.
    bool boolean_test() const
     {
      return !empty();
     }

    const tchar& operator [] (size_t aPos) const
     {
      return at(aPos);
     }
    tchar& operator [] (size_t aPos)
     {
      return at(aPos);
     }
    //. Returns {\tt DwString::at(aPos) const} or {\tt DwString::at(aPos)}.
    //. Note that the non-const version always assumes that the contents
    //. will be modified and therefore always copies a shared internal
    //. buffer before it returns.

    const tchar& at(size_t aPos) const
     {
      // Returning const tchar& instead of tchar will allow us to use DwString::at()
      // in the following way:
      //    if (&s.at(1) == ' ') { /* ... */ }
      assert(aPos <= mLength);
      if (aPos < mLength) {
          return data()[aPos];
      }
      else if (aPos == mLength) {
          return sEmptyRep->mBuffer[0];
      }
      else {
          // This "undefined behavior"
          // Normally, this will not occur.  The assert() macro will catch it,
          // or at some point we may throw an exception.
          return data()[0];
      }
     }
    tchar& at(size_t aPos)
     {
      assert(aPos < mLength);
      if (aPos < mLength) {
          return (tchar&) c_str()[aPos];
      }
      else {
          // This is "undefined behavior"
          assert(0);
          return (tchar&) c_str()[0];
      }
     }
    //. Returns the character at position {\tt aPos} in the string's contents.
    //. The non-const version returns an lvalue that may be assigned to.
    //. Note that the non-const version always assumes that the contents
    //. will be modified and therefore always copies a shared internal
    //. buffer before it returns.

    DwString& operator += (const DwString& aStr)
     {
      return append(aStr);
     }
    DwString& operator += (const tchar* aCstr)
     {
      return append(aCstr);
     }
    DwString& operator += (tchar aChar)
     {
      return append(1, aChar);
     }
    //. Appends the contents of the operand to this string.
    //. {\tt aCstr} must point to a NUL-terminated array of characters
    //. (a C string).
    //. Returns {\tt *this}.

    DwString& append(const DwString& aStr);
    DwString& append(const DwString& aStr, size_t aPos, size_t aLen);
    DwString& append(const tchar* aBuf, size_t aLen);
    DwString& append(const tchar* aCstr);
    DwString& append(size_t aLen, tchar aChar);
    //. Appends characters to (the end of) this string.
    //. Returns {\tt *this}.
    //.
    //. The first version appends all of the characters from {\tt aStr}.
    //.
    //. The second version appends at most {\tt aLen} characters from
    //. {\tt aStr} beginning at position {\tt aPos}.  {\tt aPos} must be
    //. less than or equal to {\tt aStr.size()}.  The function will not
    //. append more characters than what are available in {\tt aStr}.
    //.
    //. The third version appends {\tt aLen} characters from {\tt aBuf},
    //. which is not assumed to be NUL-terminated and can contain embedded
    //. NULs.
    //.
    //. The fourth version appends characters from the NUL-terminated
    //. string {\tt aCstr}.
    //.
    //. The fifth version appends {\tt aChar} repeated {\tt aLen} times.

    DwString& assign(const DwString& aStr);
    DwString& assign(const DwString& aStr, size_t aPos, size_t aLen);
    DwString& assign(const tchar* aBuf, size_t aLen);
    DwString& assign(const tchar* aCstr);
    DwString& assign(size_t aLen, tchar aChar);
    //. Assigns characters to this string.
    //. Returns {\tt *this}.
    //.
    //. The first version assigns all of the characters from {\tt aStr}.
    //.
    //. The second version assigns at most {\tt aLen} characters from
    //. {\tt aStr} beginning at position {\tt aPos}.  {\tt aPos} must be
    //. less than or equal to {\tt aStr.size()}.  The function will not
    //. assign more characters than what are available in {\tt aStr}.
    //.
    //. The third version assigns {\tt aLen} characters from {\tt aBuf},
    //. which is not assumed to be NUL-terminated and can contain embedded
    //. NULs.
    //.
    //. The fourth version assigns characters from the NUL-terminated
    //. string {\tt aCstr}.
    //.
    //. The fifth version assigns {\tt aChar} repeated {\tt aLen} times.

    DwString& insert(size_t aPos1, const DwString& aStr);
    DwString& insert(size_t aPos1, const DwString& aStr, size_t aPos2,
        size_t aLen2);
    DwString& insert(size_t aPos1, const tchar* aBuf, size_t aLen2);
    DwString& insert(size_t aPos1, const tchar* aCstr);
    DwString& insert(size_t aPos1, size_t aLen2, tchar aChar);
    //. Inserts characters into this string beginning at position {\tt aPos1}.
    //. Returns {\tt *this}.
    //.
    //. The first version inserts all of the characters from {\tt aStr}.
    //.
    //. The second version inserts at most {\tt aLen2} characters from
    //. {\tt aStr} beginning at position {\tt aPos2}.  {\tt aPos1} must be
    //. less than or equal to {\tt aStr.size()}.  The function will not
    //. assign more characters than what are available in {\tt aStr}.
    //.
    //. The third version inserts {\tt aLen2} characters from {\tt aBuf},
    //. which is not assumed to be NUL-terminated and can contain embedded
    //. NULs.
    //.
    //. The fourth version inserts characters from the NUL-terminated
    //. string {\tt aCstr}.
    //.
    //. The fifth version inserts {\tt aChar} repeated {\tt aLen2} times.

    DwString& erase(size_t aPos=0, size_t aLen=npos);
    //. Erases (removes) at most {\tt aLen} characters beginning at position
    //. {\tt aPos} from this string.
    //. The function will not erase more characters than what are
    //. available.
    //. Returns {\tt *this}.

    DwString& replace(size_t aPos1, size_t aLen1, const DwString& aStr);
    DwString& replace(size_t aPos1, size_t aLen1, const DwString& aStr,
        size_t aPos2, size_t aLen2);
    DwString& replace(size_t aPos1, size_t aLen1, const tchar* aBuf,
        size_t aLen2);
    DwString& replace(size_t aPos1, size_t aLen1, const tchar* aCstr);
    DwString& replace(size_t aPos1, size_t aLen1, size_t aLen2, tchar aChar);
    //. Removes {\tt aLen1} characters beginning at position {\tt aPos1}
    //. and inserts other characters.
    //. Returns {\tt *this}.
    //.
    //. The first version inserts all of the characters from {\tt aStr}.
    //.
    //. The second version inserts at most {\tt aLen2} characters from
    //. {\tt aStr} beginning at position {\tt aPos2}.  {\tt aPos1} must be
    //. less than or equal to {\tt aStr.size()}.  The function will not
    //. assign more characters than what are available in {\tt aStr}.
    //.
    //. The third version inserts {\tt aLen2} characters from {\tt aBuf},
    //. which is not assumed to be NUL-terminated and can contain embedded
    //. NULs.
    //.
    //. The fourth version inserts characters from the NUL-terminated
    //. string {\tt aCstr}.
    //.
    //. The fifth version inserts {\tt aChar} repeated {\tt aLen2} times.

    size_t copy(tchar* aBuf, size_t aLen, size_t aPos=0) const;
    //. Copies at most {\tt aLen} characters beginning at position {\tt aPos}
    //. from this string to the buffer pointed to by {\tt aBuf}.
    //. Returns the number of characters copied.

    void swap(DwString& aStr);
    //. Swaps the contents of this string and {\tt aStr}.

    const tchar* c_str() const
     {
      if (mRep->mRefCount > 1 && mRep != sEmptyRep) {
          DwString* xthis = (DwString*) this;
          xthis->_copy();
      }
      else
       if (mRep->mRefCount==1)
        mRep->mBuffer[mStart+mLength]='\0';
      return &mRep->mBuffer[mStart];
     }
    const tchar* data() const
     {
      return &mRep->mBuffer[mStart];
     }
    //. These member functions permit access to the internal buffer used
    //. by the {\tt DwString} object.  {\tt c_str()} returns a NUL-terminated
    //. string suitable for use in C library functions.  {\tt data()}
    //. returns a pointer to the internal buffer, which may not be
    //. NUL-terminated.
    //.
    //. {\tt c_str()} may copy the internal buffer in order to place the
    //. terminating NUL.  This is not a violation of the const declaration:
    //. it is a logical const, not a bit-representation const.  It could
    //. have the side effect of invalidating a pointer previously returned
    //. by {\tt c_str()} or {\tt data()}.
    //.
    //. The characters in the returned string should not be modified, and
    //. should be considered invalid after any call to a non-const member
    //. function or another call to {\tt c_str()}.

    size_t find(const DwString& aStr, size_t aPos=0) const;
    size_t find(const tchar* aBuf, size_t aPos, size_t aLen) const;
    size_t find(const tchar* aCstr, size_t aPos=0) const;
    size_t find(tchar aChar, size_t aPos=0) const;
    //. Performs a forward search for a sequence of characters in the
    //. {\tt DwString} object.  The return value is the position of the
    //. sequence in the string if found, or {\tt DwString::npos} if not
    //. found.
    //.
    //. The first version searches beginning at position {\tt aPos} for
    //. the sequence of characters in {\tt aStr}.
    //.
    //. The second version searches beginning at position {\tt aPos} for
    //. the sequence of {\tt aLen} characters in {\tt aBuf}, which need not
    //. be NUL-terminated and can contain embedded NULs.
    //.
    //. The third version searches beginning at position {\tt aPos} for
    //. the sequence of characters in the NUL-terminated string {\tt aCstr}.
    //.
    //. The fourth version searches beginning at position {\tt aPos} for
    //. the character {\tt aChar}.

    size_t rfind(const DwString& aStr, size_t aPos=npos) const;
    size_t rfind(const tchar* aBuf, size_t aPos, size_t aLen) const;
    size_t rfind(const tchar* aCstr, size_t aPos=npos) const;
    size_t rfind(tchar aChar, size_t aPos=npos) const;
    //. Performs a reverse search for a sequence of characters in the
    //. {\tt DwString} object.  The return value is the position of the
    //. sequence in the string if found, or {\tt DwString::npos} if not
    //. found.
    //.
    //. The first version searches beginning at position {\tt aPos} for
    //. the sequence of characters in {\tt aStr}.
    //.
    //. The second version searches beginning at position {\tt aPos} for
    //. the sequence of {\tt aLen} characters in {\tt aBuf}, which need not
    //. be NUL-terminated and can contain embedded NULs.
    //.
    //. The third version searches beginning at position {\tt aPos} for
    //. the sequence of characters in the NUL-terminated string {\tt aCstr}.
    //.
    //. The fourth version searches beginning at position {\tt aPos} for
    //. the character {\tt aChar}.

    size_t find_first_of(const DwString& aStr, size_t aPos=0) const;
    size_t find_first_of(const tchar* aBuf, size_t aPos, size_t aLen) const;
    size_t find_first_of(const tchar* aCstr, size_t aPos=0) const;
    //. Performs a forward search beginning at position {\tt aPos} for
    //. the first occurrence of any character from a specified set of
    //. characters.  The return value is the position of the character
    //. if found, or {\tt DwString::npos} if not found.
    //.
    //. The first version searches for any character in the string {\tt aStr}.
    //.
    //. The second version searches for any of the {\tt aLen} characters in
    //. {\tt aBuf}.
    //.
    //. The third version searches for any character in the NUL-terminated
    //. string {\tt aCstr}.

    size_t find_last_of(const DwString& aStr, size_t aPos=npos) const;
    size_t find_last_of(const tchar* aBuf, size_t aPos, size_t aLen) const;
    size_t find_last_of(const tchar* aCstr, size_t aPos=npos) const;
    //. Performs a reverse search beginning at position {\tt aPos} for
    //. the first occurrence of any character from a specified set of
    //. characters.  If {\tt aPos} is greater than or equal to the number
    //. of characters in the string, then the search starts at the end
    //. of the string.  The return value is the position of the character
    //. if found, or {\tt DwString::npos} if not found.
    //.
    //. The first version searches for any character in the string {\tt aStr}.
    //.
    //. The second version searches for any of the {\tt aLen} characters in
    //. {\tt aBuf}.
    //.
    //. The third version searches for any character in the NUL-terminated
    //. string {\tt aCstr}.

    size_t find_first_not_of(const DwString& aStr, size_t aPos=0) const;
    size_t find_first_not_of(const tchar* aBuf, size_t aPos, size_t aLen) const;
    size_t find_first_not_of(const tchar* aCstr, size_t aPos=0) const;
    //. Performs a forward search beginning at position {\tt aPos} for
    //. the first occurrence of any character {\it not} in a specified set
    //. of characters.  The return value is the position of the character
    //. if found, or {\tt DwString::npos} if not found.
    //.
    //. The first version searches for any character not in the string
    //. {\tt aStr}.
    //.
    //. The second version searches for any character not among the
    //. {\tt aLen} characters in {\tt aBuf}.
    //.
    //. The third version searches for any character not in the NUL-terminated
    //. string {\tt aCstr}.

    size_t find_last_not_of(const DwString& aStr, size_t aPos=npos) const;
    size_t find_last_not_of(const tchar* aBuf, size_t aPos, size_t aLen) const;
    size_t find_last_not_of(const tchar* aCstr, size_t aPos=npos) const;
    //. Performs a reverse search beginning at position {\tt aPos} for
    //. the first occurrence of any character {\it not} in a specified set
    //. of characters.  If {\tt aPos} is greater than or equal to the number
    //. of characters in the string, then the search starts at the end
    //. of the string.  The return value is the position of the character
    //. if found, or {\tt DwString::npos} if not found.
    //.
    //. The first version searches for any character not in the string
    //. {\tt aStr}.
    //.
    //. The second version searches for any character not among the
    //. {\tt aLen} characters in {\tt aBuf}.
    //.
    //. The third version searches for any character not in the NUL-terminated
    //. string {\tt aCstr}.

    DwString substr(size_t aPos=0, size_t aLen=npos) const;
    //. Returns a string that contains at most {\tt aLen} characters from
    //. the {\tt DwString} object beginning at position {\tt aPos}.  The
    //. returned substring will not contain more characters than what are
    //. available in the superstring {\tt DwString} object.

    int compare(const DwString& aStr) const;
    int compare(size_t aPos1, size_t aLen1, const DwString& aStr) const;
    int compare(size_t aPos1, size_t aLen1, const DwString& aStr,
        size_t aPos2, size_t aLen2) const;
    int compare(const tchar* aCstr) const;
    int compare(size_t aPos1, size_t aLen1, const tchar* aBuf, size_t aLen2=npos) const;
    //. These member functions compare a sequence of characters to this
    //. {\tt DwString} object, or a segment of this {\tt DwString} object.
    //. They return -1, 0, or 1, depending on whether this {\tt DwString}
    //. object is less than, equal to, or greater than the compared sequence
    //. of characters, respectively.
    //.
    //. The first version compares {\tt aStr} to this string.
    //.
    //. The second version compares {\tt aStr} to the segment of this string
    //. of length {\tt aLen} beginning at position {\tt aPos}.
    //.
    //. The third version compares the {tt aLen2} characters beginning at
    //. position {\tt aPos2} in {\tt aStr} with the {\tt aLen1} characters
    //. beginning at position {\tt aPos1} in this {\tt DwString} object.
    //.
    //. The fourth version compares the NUL-terminated string {\tt aCstr}
    //. to this {\tt DwString}.
    //.
    //. The fifth version compares the {\tt aLen2} characters in {\tt aBuf}
    //. with this {\tt DwString}.

    // Non-ANSI member functions

    DwString& ConvertToLowerCase();
    DwString& ConvertToUpperCase();
    //. Converts this {\tt DwString} object's characters to all lower case or
    //. all upper case.

    DwString& Trim();
    //. Removes all white space from the beginning and the end of this
    //. {\tt DwString} object.  White space characters include ASCII HT,
    //. LF, and SPACE.

    //void WriteTo(std::ostream& aStrm) const;
    //. Writes the contents of this {\tt DwString} object to the stream
    //. {\tt aStrm}.

    int RefCount() const
     {
      return mRep->mRefCount;
     }
    //. This {\it advanced} member function returns the number of
    //. references to the internal buffer used by the {\tt DwString} object.

    void TakeBuffer(tchar* aBuf, size_t aSize, size_t aStart, size_t aLen);
    //. This {\it advanced} member function sets the contents of the
    //. {\tt DwString} object to the {\tt aLen} characters starting at
    //. offset {\tt aStart} in the buffer {\tt aBuf}.  {\tt aSize} is
    //. the allocated size of {\tt aBuf}.
    //. This member function is provided for efficiency in setting a
    //. {\tt DwString}'s contents from a large buffer.  It is efficient
    //. because no copying takes place.  Instead, {\tt aBuf} becomes the
    //. buffer used internally by the {\tt DwString} object, which
    //. takes responsibility for deleting the buffer.
    //. Because DwString will free the buffer using {\tt delete []}, the
    //. buffer should have been allocated using {\tt new}.
    //. See also: ReleaseBuffer().

    void ReleaseBuffer(tchar** aBuf, size_t* aSize, size_t* aStart, size_t* aLen);
    //. This {\it advanced} member function is the symmetric opposite of
    //. {\tt TakeBuffer()}, to the extent that such an opposite is possible.
    //. It provides a way to ``export'' the buffer used internally by the
    //. {\tt DwString} object.
    //. Note, however, that because of the copy-on-modify feature of
    //. {\tt DwString}, the {\tt DwString} object may not have sole
    //. ownership of its internal buffer.  When that is case,
    //. {\tt ReleaseBuffer()} will return a copy of the buffer.  You can check
    //. to see if the internal buffer is shared by calling {\tt RefCount()}.
    //. On return from this member function, the {\tt DwString} object will
    //. have valid, but empty, contents.
    //. It is recommended that you use this function only on rare occasions
    //. where you need to export efficiently a large buffer.

    void CopyTo(DwString* aStr) const;
    //. This {\it advanced} member function copies this {\tt DwString}
    //. object to {\tt aStr}.  This member
    //. function is different from the assignment operator, because it
    //. physically copies the buffer instead of just duplicating a reference
    //. to it.

protected:

    DwStringRep<tchar>* mRep;
    size_t  mStart;
    size_t  mLength;

    void _copy();
    void _replace(size_t aPos1, size_t aLen1, const tchar* aBuf, size_t aLen2);
    void _replace(size_t aPos1, size_t aLen1, size_t aLen2, tchar aChar);

private:
    static const size_t kEmptyBufferSize;
    static tchar sEmptyBuffer[];
    static DwStringRep<tchar>* sEmptyRep;
public:

  static inline void mem_free(tchar* buf)
   {
    assert(buf != 0);
    if (buf && buf != sEmptyBuffer)
        delete [] buf;
   }

    //virtual void PrintDebugInfo(std::ostream& aStrm) const;
    //. Prints debugging information about the object to {\tt aStrm}.
    //.
    //. This member function is available only in the debug version of
    //. the library.

	static void CALLBACK GlobalInitialization( BOOL bLoading, const CLSID *rclsid);
};

template<class tchar> DwString<tchar> operator + (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2);
template<class tchar> DwString<tchar> operator + (const tchar* aCstr, const DwString<tchar>& aStr2);
template<class tchar> DwString<tchar> operator + (tchar aChar, const DwString<tchar>& aStr2);
template<class tchar> DwString<tchar> operator + (const DwString<tchar>& aStr1, const tchar* aCstr);
template<class tchar> DwString<tchar> operator + (const DwString<tchar>& aStr1, tchar aChar);

template<class tchar> bool operator == (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2);
template<class tchar> bool operator == (const DwString<tchar>& aStr1, const tchar* aCstr);
template<class tchar> bool operator == (const tchar* aCstr, const DwString<tchar>& aStr2);

template<class tchar> bool operator != (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2);
template<class tchar> bool operator != (const DwString<tchar>& aStr1, const tchar* aCstr);
template<class tchar> bool operator != (const tchar* aCstr, const DwString<tchar>& aStr2);

template<class tchar> bool operator < (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2);
template<class tchar> bool operator < (const DwString<tchar>& aStr1, const tchar* aCstr);
template<class tchar> bool operator < (const tchar* aCstr, const DwString<tchar>& aStr2);

template<class tchar> bool operator > (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2);
template<class tchar> bool operator > (const DwString<tchar>& aStr1, const tchar* aCstr);
template<class tchar> bool operator > (const tchar* aCstr, const DwString<tchar>& aStr2);

template<class tchar> bool operator <= (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2);
template<class tchar> bool operator <= (const DwString<tchar>& aStr1, const tchar* aCstr);
template<class tchar> bool operator <= (const tchar* aCstr, const DwString<tchar>& aStr2);

template<class tchar> bool operator >= (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2);
template<class tchar> bool operator >= (const DwString<tchar>& aStr1, const tchar* aCstr);
template<class tchar> bool operator >= (const tchar* aCstr, const DwString<tchar>& aStr2);

template<class tchar> int DwStrcasecmp(const DwString<tchar>& aStr1, const DwString<tchar>& aStr2);
template<class tchar> int DwStrcasecmp(const DwString<tchar>& aStr1, const tchar* aCstr);
template<class tchar> int DwStrcasecmp(const tchar* aCstr, const DwString<tchar>& aStr2);

template<class tchar> int DwStrncasecmp(const DwString<tchar>& aStr1, const DwString<tchar>& aStr2, size_t aLen);
template<class tchar> int DwStrncasecmp(const DwString<tchar>& aStr, const tchar* aCstr, size_t aLen);
template<class tchar> int DwStrncasecmp(const tchar* aCstr, const DwString<tchar>& aStr, size_t aLen);

template<class tchar> int DwStrcmp(const DwString<tchar>& aStr1, const DwString<tchar>& aStr2);
template<class tchar> int DwStrcmp(const DwString<tchar>& aStr, const tchar* aCstr);
template<class tchar> int DwStrcmp(const tchar* aCstr, const DwString<tchar>& aStr);

template<class tchar> int DwStrncmp(const DwString<tchar>& aStr1, const DwString<tchar>& aStr2, size_t aLen);
template<class tchar> int DwStrncmp(const DwString<tchar>& aStr, const tchar* aCstr, size_t aLen);
template<class tchar> int DwStrncmp(const tchar* aCstr, const DwString<tchar>& aStr, size_t aLen);

template<class tchar> void DwStrcpy(DwString<tchar>& aStrDest, const DwString<tchar>& aStrSrc);
template<class tchar> void DwStrcpy(DwString<tchar>& aStrDest, const tchar* aCstrSrc);
template<class tchar> void DwStrcpy(tchar* aCstrDest, const DwString<tchar>& aStrSrc);

template<class tchar> void DwStrncpy(DwString<tchar>& aStrDest, const DwString<tchar>& aStrSrc, size_t aLen);
template<class tchar> void DwStrncpy(DwString<tchar>& aStrDest, const tchar* aCstrSrc, size_t aLen);
template<class tchar> void DwStrncpy(tchar* aCstrDest, const DwString<tchar>& aStrSrc, size_t aLen);

template<class tchar> tchar* DwStrdup(const DwString<tchar>& aStr);

typedef DwString<wchar_t> ffwstring;
typedef std::vector<ffwstring> wstrings;
#ifdef UNICODE
 typedef DwString<wchar_t> ffstring;
 typedef std::vector<ffstring> strings;
#else
 typedef DwString<char> ffstring;
 typedef std::vector<ffstring> strings;
#endif

template<class T> struct tchar_traits {};
template<> struct tchar_traits<char>
{
 typedef DwString<char> ffstring;
 typedef std::vector<ffstring> strings;
 typedef unsigned char uchar_t;
 typedef wchar_t other_char_t;
 typedef unsigned char toupper_t;

 static __forceinline int isspace(int c) {return ::isspace(c);}
 static __forceinline int isupper(int c) {return ::isupper(c);}
 static __forceinline int toupper(int c) {return ::toupper(c);}

 typedef int (*Tsscanf)(const char *,const char*, ...);
 static Tsscanf sscanf() {return ::sscanf;}

 typedef int (*Tsprintf)(char *,const char*, ...);
 static Tsprintf sprintf() {return ::sprintf;}

 typedef int (*Tsnprintf)(char *,size_t,const char*, ...);
 static Tsnprintf snprintf() {return ::_snprintf;}

 typedef int (*Tfprintf)(FILE *, const char *, ...);
 static Tfprintf fprintf() {return ::fprintf;}
};
template<> struct tchar_traits<wchar_t>
{
 typedef DwString<wchar_t> ffstring;
 typedef std::vector<ffstring> strings;
 typedef unsigned short uchar_t;
 typedef char other_char_t;
 typedef wint_t toupper_t;

 static __forceinline int isspace(wint_t c) {return ::iswspace(c);}
 static __forceinline int isupper(wint_t c) {return ::iswupper(c);}
 static __forceinline int toupper(wint_t c) {return ::towupper(c);}

 typedef int (*Tsscanf)(const wchar_t *,const wchar_t*, ...);
 static Tsscanf sscanf() {return ::swscanf;}

 typedef int (*Tsprintf)(wchar_t *,const wchar_t*, ...);
 static Tsprintf sprintf() {return ::swprintf;}

 typedef int (*Tsnprintf)(wchar_t *,size_t,const wchar_t*, ...);
 static Tsnprintf snprintf() {return ::_snwprintf;}

 typedef int (*Tfprintf)(FILE *, const wchar_t *, ...);
 static Tfprintf fprintf() {return ::fwprintf;}
};

struct ffstring_iless
{
 bool operator ()(const ffstring &s1,const ffstring &s2) const
  {
   return stricmp(s1.c_str(),s2.c_str())<0;
  }
};

namespace std
{
 template<> struct hash<DwString<char> >
  {
   size_t operator()(const DwString<char> &__s) const;
  };
 template<> struct hash<DwString<wchar_t> >
  {
   size_t operator()(const DwString<wchar_t> &__s) const;
  };
}

#if defined(__INTEL_COMPILER) || defined(__GNUC__) || (_MSC_VER>=1300)
template<class T> static inline const T* T_L(const char *t1,const wchar_t *t2);
template<> inline const char* T_L(const char *t1,const wchar_t *t2) {return t1;}
template<> inline const wchar_t* T_L(const char *t1,const wchar_t *t2) {return t2;}
template<class T> static inline const T T_L(const char t1,const wchar_t t2);
template<> inline const char T_L(const char t1,const wchar_t t2) {return t1;}
template<> inline const wchar_t T_L(const char t1,const wchar_t t2) {return t2;}
#undef _L
#define _L(x) T_L<tchar>(x,L##x)

//===================================== passtring =====================================

template<class tchar> class passtring :protected DwString<tchar>
{
private:
 static tchar chr;
public:
 static const size_t npos=0;
 passtring(void) {}
 passtring(const DwString<tchar> &Istr):DwString<tchar>(Istr) {}
 passtring(const tchar *Istr):DwString<tchar>(Istr) {}
 const tchar& operator [] (int aPos) const
  {
   return (1<=aPos && aPos<=size())?this->at(aPos-1):chr;
  }
 tchar& operator [] (int aPos)
  {
   return (1<=aPos && aPos<=size())?this->at(aPos-1):chr;
  }
 void erase(size_t aPos=0, size_t aLen=npos)
  {
   DwString<tchar>::erase(aPos-1,aLen);
  }
 int find(const DwString<tchar>& aStr, size_t aPos=1) const
  {
   return (int)DwString<tchar>::find(aStr,aPos-1)+1;
  }
 int find(tchar aStr, size_t aPos=1) const
  {
   return (int)DwString<tchar>::find(aStr,aPos-1)+1;
  }
 void replace(size_t aPos1, size_t aLen1, const DwString<tchar>& aStr)
  {
   DwString<tchar>::replace(aPos1-1,aLen1,aStr);
  }
 int size() const
  {
   return (int)DwString<tchar>::size();
  }
 void insert(size_t aPos1, const tchar* aCstr)
  {
   DwString<tchar>::insert(aPos1-1,aCstr);
  }
 passtring copy(int pos,int len) const
  {
   if (pos>size() || len<0)
    return passtring();
   else
    return this->substr(pos-1,len);
  }
 void append(const passtring& aStr)
  {
   DwString<tchar>::append(aStr);
  }
 bool empty() const
  {
   return DwString<tchar>::empty();
  }
 const tchar* c_str() const
  {
   return DwString<tchar>::c_str();
  }
 void clear()
  {
   DwString<tchar>::clear();
  }
 void ConvertToLowerCase()
  {
   DwString<tchar>::ConvertToLowerCase();
  }
};
template<class tchar> inline passtring<tchar> operator + (const passtring<tchar> &aStr1, const passtring<tchar> &aStr2)
{
 passtring<tchar> s1(aStr1);
 s1.append(aStr2);
 return s1;
}
template<class tchar> inline passtring<tchar> operator + (const passtring<tchar> &aStr1, const tchar *aStr2)
{
 passtring<tchar> s1(aStr1);
 s1.append(aStr2);
 return s1;
}
template<class tchar> inline passtring<tchar> operator + (const tchar *aStr1, const passtring<tchar> &aStr2)
{
 passtring<tchar> s1(aStr1);
 s1.append(aStr2);
 return s1;
}
template<class tchar> bool operator == (const passtring<tchar>& aStr1, const passtring<tchar>& aStr2)
{
 return DwString<tchar>(aStr1.c_str()) == DwString<tchar>(aStr2.c_str());
}

template<> inline DwString<char>& DwString<char>::operator=(const char *aCstr) {return assign(aCstr);}
template<> inline DwString<char>& DwString<char>::operator=(const wchar_t *aCstr) {return assign((const char*)text<char>(aCstr));}
template<> inline DwString<wchar_t>& DwString<wchar_t>::operator=(const wchar_t *aCstr) {return assign(aCstr);}
template<> inline DwString<wchar_t>& DwString<wchar_t>::operator=(const char *aCstr) {return assign((const wchar_t*)text<wchar_t>(aCstr));}

#endif

#endif
