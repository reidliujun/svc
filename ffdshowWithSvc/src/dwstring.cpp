//=============================================================================
// File:       dwstring.cpp
// Contents:   Definitions for DwString
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

#include "stdafx.h"
#include "dwstring.h"

#if defined (DW_DEBUG_VERSION)
#define DBG_STMT(x) x;
#else
#define DBG_STMT(x) ;
#endif

#if defined (DW_DEBUG_VERSION) || defined (DW_DEVELOPMENT_VERSION)
#define DEV_STMT(x) x;
#else
#define DEV_STMT(x) ;
#endif

template<class tchar> static int dw_strcasecmp(const tchar* s1, size_t len1, const tchar* s2, size_t len2)
{
    assert(s1 != 0);
    assert(s2 != 0);
    size_t len = std::min(len1, len2);
    for (size_t i=0; i < len; ++i) {
        int c1 = tolower(s1[i]);
        int c2 = tolower(s2[i]);
        if (c1 < c2) {
            return -1;
        }
        else if (c1 > c2) {
            return 1;
        }
    }
    if (len1 < len2) {
        return -1;
    }
    else if (len1 > len2) {
        return 1;
    }
    return 0;
}


template<class tchar> static int dw_strcmp(const tchar* s1, size_t len1, const tchar* s2, size_t len2)
{
    assert(s1 != 0);
    assert(s2 != 0);
    size_t len = std::min(len1, len2);
    for (size_t i=0; i < len; ++i) {
        if (s1[i] < s2[i]) {
            return -1;
        }
        else if (s1[i] > s2[i]) {
            return 1;
        }
    }
    if (len1 < len2) {
        return -1;
    }
    else if (len1 > len2) {
        return 1;
    }
    return 0;
}


// Copy

template<class tchar> inline void mem_copy(const tchar* src, size_t n, tchar* dest)
{
    assert(src != 0);
    assert(dest != 0);
    assert(src != dest);
    if (n == 0 || src == dest || !src || !dest) return;
    memmove(dest, src, n*sizeof(tchar));
}

// Allocate buffer whose size is a power of 2

template<class tchar> static tchar* mem_alloc(size_t* aSize)
{
    assert(aSize != 0);
    // minimum size is 32
    size_t size = 32;
    while (size < *aSize) {
        size <<= 1;
    }
    *aSize = 0;
    tchar* buf = new tchar[size];
    if (buf != 0)
        *aSize = size;
    return buf;
}

template<class tchar> static inline DwStringRep<tchar>* new_rep_reference(DwStringRep<tchar>* rep)
{
    assert(rep != 0);
    ++rep->mRefCount;
    return rep;
}


template<class tchar> static inline void delete_rep_safely(DwStringRep<tchar>* rep)
{
    assert(rep != 0);
#if defined(DW_DEBUG_VERSION) || defined(DW_DEVELOPMENT_VERSION)
    if (rep->mRefCount <= 0) {
        std::cerr << "Error: attempt to delete a DwStringRep "
            "with ref count <= 0" << std::endl;
        std::cerr << "(Possibly 'delete' was called twice for same object)"
            << std::endl;
        abort();
    }
#endif //  defined(DW_DEBUG_VERSION) || defined(DW_DEVELOPMENT_VERSION)
    --rep->mRefCount;
    if (rep->mRefCount == 0) {
        delete rep;
    }
}


//--------------------------------------------------------------------------


//DwStringRep* DwStringRep::theirPool = NULL;
//int DwStringRep::theirPoolCount = 0;


// DwStringRep takes ownership of the buffer passed as an argument

template<class tchar> DwStringRep<tchar>::DwStringRep(tchar* aBuf, size_t aSize)
{
    assert(aBuf != 0);
    mSize = aSize;
    mBuffer = aBuf;
    mRefCount = 1;
    mPageMod = 0;
}
/*
DwStringRep::DwStringRep(FILE* aFile, size_t aSize)
{
    assert(aFile != 0);
    static int pagesize = -1;
    if (pagesize < 0)
	pagesize = getpagesize();
    int tell = ftell(aFile);
    mPageMod = tell % pagesize;
    mSize = aSize;
    mRefCount = 1;

    mBuffer = (char *)mmap(0, aSize + mPageMod, PROT_READ, MAP_SHARED, fileno(aFile), tell - mPageMod) + mPageMod;
    ++mPageMod;
    if (mBuffer == MAP_FAILED) {
	mBuffer = 0;
	mSize = 0;
	mPageMod = 0;
    }
}
*/

template<class tchar> DwStringRep<tchar>::~DwStringRep()
{
#if defined (DW_DEBUG_VERSION) || defined (DW_DEVELOPMENT_VERSION)
    if (mBuffer == 0) {
        std::cerr << "DwStringRep destructor called for bad DwStringRep object"
            << std::endl;
        std::cerr << "(Possibly 'delete' was called twice for same object)"
            << std::endl;
        abort();
    }
#endif //  defined (DW_DEBUG_VERSION) || defined (DW_DEVELOPMENT_VERSION)
/*
    if (mPageMod) {
	--mPageMod;
	munmap(mBuffer - mPageMod, mSize + mPageMod);
    } else */{
	DwString<tchar>::mem_free(mBuffer);
    }
    //DEV_STMT(mBuffer = 0)
}


template<class tchar> void DwStringRep<tchar>::CheckInvariants() const
{
#if defined (DW_DEBUG_VERSION)
    assert(mBuffer != 0);
    assert(mSize > 0);
    assert(mRefCount > 0);
#endif // defined (DW_DEBUG_VERSION)
}


// Efficient memory management.  May be used at some point in the future.

#if 0
void* DwStringRep::operator new(size_t sz)
{
    void* rep;
    if (theirPoolCount > 0) {
        --theirPoolCount;
        rep = theirPool;
        theirPool = theirPool->mNext;
    }
    else {
        rep = new char[sz];
    }
    return rep;
}


void DwStringRep::operator delete(void* aRep, size_t)
{
    if (theirPoolCount < 200) {
        DwStringRep* rep = (DwStringRep*) aRep;
        ++theirPoolCount;
        rep->mNext = theirPool;
        theirPool = rep;
    }
    else {
        delete [] (char*) aRep;
    }
}
#endif


//--------------------------------------------------------------------------

template<class tchar> const size_t DwString<tchar>::kEmptyBufferSize = 4;
template<> char DwString<char>::sEmptyBuffer[]="    ";
template<> wchar_t DwString<wchar_t>::sEmptyBuffer[]=L"    ";
template<class tchar> DwStringRep<tchar>* DwString<tchar>::sEmptyRep = NULL;
template<class tchar> const size_t DwString<tchar>::npos = (size_t) -1;

template<class tchar> void CALLBACK DwString<tchar>::GlobalInitialization( BOOL bLoading, const CLSID *)
{
	if ( !bLoading && sEmptyRep && (sEmptyRep->mRefCount == 1) )
	{
		delete sEmptyRep;
		sEmptyRep = NULL;
	}
}


template<class tchar> DwString<tchar>::DwString()
{
    if (sEmptyRep == 0) {
        sEmptyBuffer[0] = 0;
        sEmptyRep = new DwStringRep<tchar>(sEmptyBuffer, kEmptyBufferSize);
        assert(sEmptyRep != 0);
    }
    DBG_STMT(sEmptyRep->CheckInvariants())
    mRep = new_rep_reference(sEmptyRep);
    mStart = 0;
    mLength = 0;
}

template<> DwString<char>::DwString(const wchar_t* aCstr)
{
 init1(text<char>(aCstr));
}
template<> DwString<wchar_t>::DwString(const char* aCstr)
{
 init1(text<wchar_t>(aCstr));
}

template<class tchar> void DwString<tchar>::init2(const DwString& aStr, size_t aPos, size_t aLen)
{
    assert(aPos <= aStr.mLength);
    if (sEmptyRep == 0) {
        sEmptyBuffer[0] = 0;
        sEmptyRep = new DwStringRep<tchar>(sEmptyBuffer, kEmptyBufferSize);
        assert(sEmptyRep != 0);
    }
    DBG_STMT(aStr.CheckInvariants())
    size_t pos = std::min(aPos, aStr.mLength);
    size_t len = std::min(aLen, aStr.mLength - pos);
    if (len > 0) {
        mRep = new_rep_reference(aStr.mRep);
        mStart = aStr.mStart + pos;
        mLength = len;
    }
    else // if (len == 0)
    {
        mRep = new_rep_reference(sEmptyRep);
        mStart = 0;
        mLength = 0;
    }
}
template<> DwString<char>::DwString(const DwString<char> &aStr,size_t aPos,size_t aLen)
{
 init2(aStr,aPos,aLen);
}
template<> DwString<char>::DwString(const DwString<wchar_t> &aStr,size_t aPos,size_t aLen)
{
 init2(aStr.c_str(),aPos,aLen); //TODO: optimize
}
template<> DwString<wchar_t>::DwString(const DwString<wchar_t> &aStr,size_t aPos,size_t aLen)
{
 init2(aStr,aPos,aLen);
}
template<> DwString<wchar_t>::DwString(const DwString<char> &aStr,size_t aPos,size_t aLen)
{
 init2(aStr.c_str(),aPos,aLen);
}

template<class tchar> DwString<tchar>::DwString(const tchar* aBuf, size_t aLen)
{
    assert(aBuf != 0);
    assert(aLen != (size_t)-1);
    if (sEmptyRep == 0) {
        sEmptyBuffer[0] = 0;
        sEmptyRep = new DwStringRep<tchar>(sEmptyBuffer, kEmptyBufferSize);
        assert(sEmptyRep != 0);
    }
    DBG_STMT(sEmptyRep->CheckInvariants())
    // Set valid values, in case an exception is thrown
    mRep = new_rep_reference(sEmptyRep);
    mStart = 0;
    mLength = 0;
    _replace(0, mLength, aBuf, aLen);
}

template<class tchar> void DwString<tchar>::init1(const tchar *aCstr)
{
    assert(aCstr != 0);
    if (sEmptyRep == 0) {
        sEmptyBuffer[0] = 0;
        sEmptyRep = new DwStringRep<tchar>(sEmptyBuffer, kEmptyBufferSize);
        assert(sEmptyRep != 0);
    }
    DBG_STMT(sEmptyRep->CheckInvariants())
    // Set valid values, in case an exception is thrown
    mRep = new_rep_reference(sEmptyRep);
    mStart = 0;
    mLength = 0;
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    _replace(0, mLength, aCstr, len);
}
template<> DwString<char>::DwString(const char* aCstr)
{
 init1(aCstr);
}
template<> DwString<wchar_t>::DwString(const wchar_t* aCstr)
{
 init1(aCstr);
}

template<class tchar> DwString<tchar>::DwString(size_t aLen, tchar aChar)
{
    assert(aLen != (size_t)-1);
    if (sEmptyRep == 0) {
        sEmptyBuffer[0] = 0;
        sEmptyRep = new DwStringRep<tchar>(sEmptyBuffer, kEmptyBufferSize);
        assert(sEmptyRep != 0);
    }
    DBG_STMT(sEmptyRep->CheckInvariants())
    // Set valid values, in case an exception is thrown
    mRep = new_rep_reference(sEmptyRep);
    mStart = 0;
    mLength = 0;
    _replace(0, mLength, aLen, aChar);
}


template<class tchar> DwString<tchar>::DwString(tchar* aBuf, size_t aSize, size_t aStart, size_t aLen)
{
    assert(aBuf != 0);
    assert(aSize > 0);
    assert(aLen < aSize);
    assert(aStart < aSize - aLen);
    if (sEmptyRep == 0) {
        sEmptyBuffer[0] = 0;
        sEmptyRep = new DwStringRep<tchar>(sEmptyBuffer, kEmptyBufferSize);
        assert(sEmptyRep != 0);
    }
    DBG_STMT(sEmptyRep->CheckInvariants())
    // Set valid values, in case an exception is thrown
    mRep = new_rep_reference(sEmptyRep);
    mStart = 0;
    mLength = 0;
    DwStringRep<tchar>* rep = new DwStringRep<tchar>(aBuf, aSize);
    assert(rep != 0);
    if (rep != 0) {
        mRep = rep;
        mStart = aStart;
        mLength = aLen;
    }
    else /* if (rep == 0) */ {
        delete [] aBuf;
    }
}


template<class tchar> DwString<tchar>::~DwString()
{
    assert(mRep != 0);
    if (mRep != sEmptyRep)
        delete_rep_safely(mRep);
    DEV_STMT(mRep = 0)
}


template<class tchar> size_t DwString<tchar>::max_size() const
{
    return ((size_t)-1) - 1;
}


template<class tchar> void DwString<tchar>::resize(size_t aLen, tchar aChar)
{
    // making string shorter?
    if (aLen < mLength) {
        mLength = aLen;
        if (mRep->mRefCount == 1) {
            mRep->mBuffer[mStart + aLen] = 0;
        }
    }
    // expanding string
    else if (aLen > mLength) {
        _replace(mLength, 0, aLen-mLength, aChar);
    }
}


template<class tchar> void DwString<tchar>::resize(size_t aLen)
{
    resize(aLen, 0);
}


template<class tchar> void DwString<tchar>::reserve(size_t aSize)
{
    if (mRep->mRefCount == 1 && aSize < mRep->mSize && mRep != sEmptyRep) {
        return;
    }
    size_t size = aSize + 1;
    tchar* newBuf = mem_alloc<tchar>(&size);
    assert(newBuf != 0);
    if (newBuf != 0) {
        tchar* to = newBuf;
        const tchar* from = mRep->mBuffer + mStart;
        mem_copy(from, mLength, to);
        to[mLength] = 0;
        DwStringRep<tchar>* rep= new DwStringRep<tchar>(newBuf, size);
        assert(rep != 0);
        if (rep != 0) {
            delete_rep_safely(mRep);
            mRep = rep;
            mStart = 0;
        }
        else {
             mem_free(newBuf);
        }
    }
}

template<class tchar> void DwString<tchar>::clear()
{
    assign(_L(""));
}


template<class tchar> DwString<tchar>& DwString<tchar>::append(const DwString<tchar>& aStr)
{
    return append(aStr, 0, aStr.mLength);
}


template<class tchar> DwString<tchar>& DwString<tchar>::append(const DwString<tchar>& aStr, size_t aPos, size_t aLen)
{
    assert(aPos <= aStr.mLength);
    size_t pos = std::min(aPos, aStr.mLength);
    size_t len = std::min(aLen, aStr.mLength - pos);
    if (&aStr == this) {
        DwString<tchar> temp(aStr);
        _replace(mLength, 0, &temp.mRep->mBuffer[temp.mStart+pos], len);
    }
    else {
        _replace(mLength, 0, &aStr.mRep->mBuffer[aStr.mStart+pos], len);
    }
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::append(const tchar* aBuf, size_t aLen)
{
    assert(aBuf != 0);
    if (aBuf != 0) {
        _replace(mLength, 0, aBuf, aLen);
    }
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::append(const tchar* aCstr)
{
    assert(aCstr != 0);
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    _replace(mLength, 0, aCstr, len);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::append(size_t aLen, tchar aChar)
{
    _replace(mLength, 0, aLen, aChar);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::assign(const DwString<tchar>& aStr)
{
    if (this != &aStr) {
        assign(aStr, 0, aStr.mLength);
    }
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::assign(const DwString<tchar>& aStr, size_t aPos, size_t aLen)
{
    assert(aPos <= aStr.mLength);
    size_t pos = std::min(aPos, aStr.mLength);
    size_t len = std::min(aLen, aStr.mLength - pos);
    if (mRep == aStr.mRep) {
        mStart = aStr.mStart + pos;
        mLength = len;
    }
    else {
        delete_rep_safely(mRep);
        mRep = new_rep_reference(aStr.mRep);
        mStart = aStr.mStart + pos;
        mLength = len;
    }
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::assign(const tchar* aBuf, size_t aLen)
{
    assert(aBuf != 0);
    assert(aLen != (size_t)-1);
    _replace(0, mLength, aBuf, aLen);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::assign(const tchar* aCstr)
{
    assert(aCstr != 0);
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    _replace(0, mLength, aCstr, len);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::assign(size_t aLen, tchar aChar)
{
    assert(aLen != (size_t)-1);
    _replace(0, mLength, aLen, aChar);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::insert(size_t aPos, const DwString<tchar>& aStr)
{
    return insert(aPos, aStr, 0, aStr.mLength);
}


template<class tchar> DwString<tchar>& DwString<tchar>::insert(size_t aPos1, const DwString<tchar>& aStr, size_t aPos2, size_t aLen2)
{
    assert(aPos1 <= mLength);
    assert(aPos2 <= aStr.mLength);
    size_t pos2 = std::min(aPos2, aStr.mLength);
    size_t len2 = std::min(aLen2, aStr.mLength - pos2);
    if (&aStr == this) {
        DwString<tchar> temp(aStr);
        _replace(aPos1, 0, &temp.mRep->mBuffer[temp.mStart+pos2], len2);
    }
    else {
        _replace(aPos1, 0, &aStr.mRep->mBuffer[aStr.mStart+pos2], len2);
    }
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::insert(size_t aPos, const tchar* aBuf, size_t aLen)
{
    assert(aBuf != 0);
    _replace(aPos, 0, aBuf, aLen);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::insert(size_t aPos, const tchar* aCstr)
{
    assert(aCstr != 0);
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    _replace(aPos, 0, aCstr, len);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::insert(size_t aPos, size_t aLen, tchar aChar)
{
    _replace(aPos, 0, aLen, aChar);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::erase(size_t aPos, size_t aLen)
{
    assert(aPos <= mLength);
    size_t pos = std::min(aPos, mLength);
    size_t len = std::min(aLen, mLength - pos);
    _replace(pos, len, _L(""), 0);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::replace(size_t aPos1, size_t aLen1, const DwString<tchar>& aStr)
{
    return replace(aPos1, aLen1, aStr, 0, aStr.mLength);
}


template<class tchar> DwString<tchar>& DwString<tchar>::replace(size_t aPos1, size_t aLen1, const DwString<tchar>& aStr, size_t aPos2, size_t aLen2)
{
    assert(aPos2 <= aStr.mLength);
    size_t pos2 = std::min(aPos2, aStr.mLength);
    size_t len2 = std::min(aLen2, aStr.mLength - pos2);
    if (&aStr == this) {
        DwString<tchar> temp(aStr);
        _replace(aPos1, aLen1, &temp.mRep->mBuffer[temp.mStart+pos2], len2);
    }
    else {
        _replace(aPos1, aLen1, &aStr.mRep->mBuffer[aStr.mStart+pos2], len2);
    }
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::replace(size_t aPos1, size_t aLen1, const tchar* aBuf, size_t aLen2)
{
    _replace(aPos1, aLen1, aBuf, aLen2);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::replace(size_t aPos1, size_t aLen1, const tchar* aCstr)
{
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    _replace(aPos1, aLen1, aCstr, len2);
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::replace(size_t aPos1, size_t aLen1, size_t aLen2, tchar aChar)
{
    _replace(aPos1, aLen1, aLen2, aChar);
    return *this;
}


template<class tchar> size_t DwString<tchar>::copy(tchar* aBuf, size_t aLen, size_t aPos) const
{
    assert(aPos <= mLength);
    assert(aBuf != 0);
    size_t pos = std::min(aPos, mLength);
    size_t len = std::min(aLen, mLength - pos);
    tchar* to = aBuf;
    const tchar* from = mRep->mBuffer + mStart + pos;
    mem_copy(from, len, to);
    return len;
}


template<class tchar> void DwString<tchar>::swap(DwString<tchar>& aStr)
{
    DwStringRep<tchar>* rep = mRep;
    mRep = aStr.mRep;
    aStr.mRep = rep;
    size_t n = mStart;
    mStart = aStr.mStart;
    aStr.mStart = n;
    n = mLength;
    mLength = aStr.mLength;
    aStr.mLength = n;
}


template<class tchar> size_t DwString<tchar>::find(const DwString<tchar>& aStr, size_t aPos) const
{
    return find(&aStr.mRep->mBuffer[aStr.mStart], aPos, aStr.mLength);
}


template<class tchar> size_t DwString<tchar>::find(const tchar* aBuf, size_t aPos, size_t aLen) const
{
    assert(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (aLen > mLength) return (size_t)-1;
    if (aPos > mLength-aLen) return (size_t)-1;
    if (aLen == 0) return aPos;
    const tchar* buf = mRep->mBuffer + mStart;
    for (size_t i=aPos; i <= mLength-aLen; ++i) {
        size_t k = i;
        size_t j = 0;
        while (j < aLen && aBuf[j] == buf[k]) {
            ++j; ++k;
        }
        if (j == aLen) return i;
    }
    return (size_t)-1;
}


template<class tchar> size_t DwString<tchar>::find(const tchar* aCstr, size_t aPos) const
{
    assert(aCstr != 0);
    if (aCstr == 0) return (size_t)-1;
    size_t len = strlen(aCstr);
    return find(aCstr, aPos, len);
}


template<class tchar> size_t DwString<tchar>::find(tchar aChar, size_t aPos) const
{
    if (aPos >= mLength) return (size_t)-1;
    const tchar* buf = mRep->mBuffer + mStart;
    for (size_t i=aPos; i < mLength; ++i) {
        if (buf[i] == aChar) return i;
    }
    return (size_t)-1;
}


template<class tchar> size_t DwString<tchar>::rfind(const DwString<tchar>& aStr, size_t aPos) const
{
    return rfind(&aStr.mRep->mBuffer[aStr.mStart], aPos, aStr.mLength);
}


template<class tchar> size_t DwString<tchar>::rfind(const tchar* aBuf, size_t aPos, size_t aLen) const
{
    assert(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (aLen > mLength) return (size_t)-1;
    size_t pos = std::min(aPos, mLength - aLen);
    if (aLen == 0) return pos;
    const tchar* buf = mRep->mBuffer + mStart;
    for (size_t i=0; i <= pos; ++i) {
        size_t k = pos - i;
        size_t j = 0;
        while (j < aLen && aBuf[j] == buf[k]) {
            ++j; ++k;
        }
        if (j == aLen) return pos - i;
    }
    return (size_t)-1;
}


template<class tchar> size_t DwString<tchar>::rfind(const tchar* aCstr, size_t aPos) const
{
    assert(aCstr != 0);
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    return rfind(aCstr, aPos, len);
}


template<class tchar> size_t DwString<tchar>::rfind(tchar aChar, size_t aPos) const
{
    size_t pos = std::min(aPos, mLength - 1);
    const tchar* buf = mRep->mBuffer + mStart;
    for (size_t i=0; i <= pos; ++i) {
        size_t k = pos - i;
        if (buf[k] == aChar) return k;
    }
    return (size_t)-1;
}


template<class tchar> size_t DwString<tchar>::find_first_of(const DwString<tchar>& aStr, size_t aPos) const
{
    return find_first_of(&aStr.mRep->mBuffer[aStr.mStart], aPos, aStr.mLength);
}


template<class tchar> size_t DwString<tchar>::find_first_of(const tchar* aBuf, size_t aPos, size_t aLen) const
{
    assert(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (aPos >= mLength) return (size_t)-1;
    if (aLen == 0) return aPos;
    tchar table[256];
    memset(table, 0, sizeof(table));
    for (size_t j=0; j < aLen; ++j) {
        table[aBuf[j]&0xff] = 1;
    }
    const tchar* buf = mRep->mBuffer + mStart;
    for (size_t i=aPos; i < mLength; ++i) {
        if (table[buf[i]&0xff]) return i;
    }
    return (size_t)-1;
}


template<class tchar> size_t DwString<tchar>::find_first_of(const tchar* aCstr, size_t aPos) const
{
    assert(aCstr != 0);
    if (aCstr == 0) return (size_t)-1;
    size_t len = strlen(aCstr);
    return find_first_of(aCstr, aPos, len);
}


template<class tchar> size_t DwString<tchar>::find_last_of(const DwString<tchar>& aStr, size_t aPos) const
{
    return find_last_of(&aStr.mRep->mBuffer[aStr.mStart], aPos, aStr.mLength);
}


template<class tchar> size_t DwString<tchar>::find_last_of(const tchar* aBuf, size_t aPos, size_t aLen) const
{
    assert(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (mLength == 0) return (size_t)-1;
    size_t pos = std::min(aPos, mLength - 1);
    if (aLen == 0) return pos;
    tchar table[256];
    memset(table, 0, sizeof(table));
    for (size_t j=0; j < aLen; ++j) {
        table[aBuf[j]&0xff] = 1;
    }
    const tchar* buf = mRep->mBuffer + mStart;
    for (size_t k=0; k <= pos; ++k) {
        size_t i = pos - k;
        if (table[buf[i]&0xff]) return i;
    }
    return (size_t)-1;
}


template<class tchar> size_t DwString<tchar>::find_last_of(const tchar* aCstr, size_t aPos) const
{
    assert(aCstr != 0);
    if (aCstr == 0) return (size_t)-1;
    size_t len = strlen(aCstr);
    return find_last_of(aCstr, aPos, len);
}


template<class tchar> size_t DwString<tchar>::find_first_not_of(const DwString<tchar>& aStr, size_t aPos) const
{
    return find_first_not_of(&aStr.mRep->mBuffer[aStr.mStart], aPos, aStr.mLength);
}


template<class tchar> size_t DwString<tchar>::find_first_not_of(const tchar* aBuf, size_t aPos, size_t aLen) const
{
    assert(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (aPos >= mLength) return (size_t)-1;
    if (aLen == 0) return (size_t)-1;
    tchar table[256];
    memset(table, 1, sizeof(table));
    for (size_t j=0; j < aLen; ++j) {
        table[aBuf[j]&0xff] = 0;
    }
    const tchar* buf = mRep->mBuffer + mStart;
    for (size_t i=aPos; i < mLength; ++i) {
        if (table[buf[i]&0xff]) return i;
    }
    return (size_t)-1;
}


template<class tchar> size_t DwString<tchar>::find_first_not_of(const tchar* aCstr, size_t aPos) const
{
    assert(aCstr != 0);
    if (aCstr == 0) return (size_t)-1;
    size_t len = strlen(aCstr);
    return find_first_not_of(aCstr, aPos, len);
}


template<class tchar> size_t DwString<tchar>::find_last_not_of(const DwString<tchar>& aStr, size_t aPos) const
{
    return find_last_not_of(&aStr.mRep->mBuffer[aStr.mStart], aPos, aStr.mLength);
}


template<class tchar> size_t DwString<tchar>::find_last_not_of(const tchar* aBuf, size_t aPos, size_t aLen) const
{
    assert(aBuf != 0);
    if (aBuf == 0) return (size_t)-1;
    if (mLength == 0) return (size_t)-1;
    size_t pos = std::min(aPos, mLength - 1);
    if (aLen == 0) return (size_t)-1;
    tchar table[256];
    memset(table, 1, sizeof(table));
    for (size_t j=0; j < aLen; ++j) {
        table[aBuf[j]&0xff] = 0;
    }
    const tchar* buf = mRep->mBuffer + mStart;
    for (size_t k=0; k <= pos; ++k) {
        size_t i = pos - k;
        if (table[buf[i]&0xff]) return i;
    }
    return (size_t)-1;
}


template<class tchar> size_t DwString<tchar>::find_last_not_of(const tchar* aCstr, size_t aPos) const
{
    assert(aCstr != 0);
    if (aCstr == 0) return (size_t)-1;
    size_t len = strlen(aCstr);
    return find_last_not_of(aCstr, aPos, len);
}


template<class tchar> DwString<tchar> DwString<tchar>::substr(size_t aPos, size_t aLen) const
{
    assert(aPos <= mLength);
    size_t pos = std::min(aPos, mLength);
    size_t len = std::min(aLen, mLength - pos);
    return DwString<tchar>(*this, pos, len);
}


template<class tchar> int DwString<tchar>::compare(const DwString<tchar>& aStr) const
{
    return compare(0, mLength, aStr, 0, aStr.mLength);
}


template<class tchar> int DwString<tchar>::compare(size_t aPos1, size_t aLen1, const DwString<tchar>& aStr) const
{
    return compare(aPos1, aLen1, aStr, 0, aStr.mLength);
}


template<class tchar> int DwString<tchar>::compare(size_t aPos1, size_t aLen1, const DwString<tchar>& aStr,
        size_t aPos2, size_t aLen2) const
{
    assert(aPos1 <= mLength);
    assert(aPos2 <= aStr.mLength);
    size_t pos1 = std::min(aPos1, mLength);
    const tchar* buf1 = mRep->mBuffer + mStart + pos1;
    size_t len1 = std::min(aLen1, mLength - pos1);
    size_t pos2 = std::min(aPos2, aStr.mLength);
    const tchar* buf2 = aStr.mRep->mBuffer + aStr.mStart + pos2;
    size_t len2 = std::min(aLen2, aStr.mLength - pos2);
    size_t len = std::min(len1, len2);
    int r = strncmp(buf1, buf2, len);
    if (r == 0) {
        if (len1 < len2)
            r = -1;
        else if (len1 > len2) {
            r = 1;
        }
    }
    return r;
}


template<class tchar> int DwString<tchar>::compare(const tchar* aCstr) const
{
    assert(aCstr != 0);
    size_t len = (aCstr) ? strlen(aCstr) : 0;
    return compare(0, mLength, aCstr, len);
}


template<class tchar> int DwString<tchar>::compare(size_t aPos1, size_t aLen1, const tchar* aBuf, size_t aLen2) const
{
    assert(aBuf != 0);
    assert(aPos1 <= mLength);
    if (aBuf == 0) {
        return (aLen1 > 0) ? 1 : 0;
    }
    size_t pos1 = std::min(aPos1, mLength);
    const tchar* buf1 = mRep->mBuffer + mStart + pos1;
    size_t len1 = std::min(aLen1, mLength - pos1);
    const tchar* buf2 = aBuf;
    size_t len2 = aLen2;
    size_t len = std::min(len1, len2);
    int r = strncmp(buf1, buf2, len);
    if (r == 0) {
        if (len1 < len2)
            r = -1;
        else if (len1 > len2) {
            r = 1;
        }
    }
    return r;
}


template<class tchar> DwString<tchar>& DwString<tchar>::ConvertToLowerCase()
{
    if (mRep->mRefCount > 1) {
        _copy();
    }
    tchar* buf = mRep->mBuffer + mStart;
    for (size_t i=0; i < mLength; ++i) {
        buf[i] = (tchar) tolower(buf[i]);
    }
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::ConvertToUpperCase()
{
    if (mRep->mRefCount > 1) {
        _copy();
    }
    tchar* buf = mRep->mBuffer + mStart;
    for (size_t i=0; i < mLength; ++i) {
        buf[i] = (tchar) toupper(buf[i]);
    }
    return *this;
}


template<class tchar> DwString<tchar>& DwString<tchar>::Trim()
{
    const tchar* buf = mRep->mBuffer + mStart;
    size_t i = 0;
    while (mLength > 0) {
        if (isspace(buf[i])) {
            ++mStart;
            --mLength;
            ++i;
        }
        else {
            break;
        }
    }
    buf = mRep->mBuffer + mStart;
    i = mLength - 1;
    while (mLength > 0) {
        if (isspace(buf[i])) {
            --mLength;
            --i;
        }
        else {
            break;
        }
    }
    if (mLength == 0) {
        assign(_L(""));
    }
 return *this;
}

/*
void DwString<tchar>::WriteTo(std::ostream& aStrm) const
{
    const char* buf = mRep->mBuffer + mStart;
    for (size_t i=0; i < mLength; ++i) {
        aStrm << buf[i];
    }
}
*/

template<class tchar> void DwString<tchar>::TakeBuffer(tchar* aBuf, size_t aSize, size_t aStart, size_t aLen)
{
    assert(aBuf != 0);
    DwStringRep<tchar>* rep = new DwStringRep<tchar>(aBuf, aSize);
    assert(rep != 0);
    if (rep) {
        delete_rep_safely(mRep);
        mRep = rep;
        mStart = aStart;
        mLength = aLen;
    }
}


template<class tchar> void DwString<tchar>::ReleaseBuffer(tchar** aBuf, size_t* aSize, size_t* aStart, size_t* aLen)
{
    assert(aBuf != 0);
    assert(aSize != 0);
    assert(aStart != 0);
    assert(aLen != 0);
    if (mRep->mRefCount == 1) {
        *aBuf = mRep->mBuffer;
        *aSize = mRep->mSize;
    }
    else {
        size_t size = mRep->mSize;
        tchar* buf = new tchar [size];
        assert(buf != 0);
        if (buf != 0) {
            mem_copy(mRep->mBuffer, size, buf);
            *aBuf = buf;
            *aSize = size;
        }
        else {
            // If not throwing an exception, recover as best we can
            *aBuf = 0;
            *aSize = 0;
            *aStart = mStart = 0;
            *aLen = mLength = 0;
            return;
        }
    }
    *aStart = mStart;
    *aLen = mLength;
    mRep = new_rep_reference(sEmptyRep);
    mStart = 0;
    mLength = 0;
}


template<class tchar> void DwString<tchar>::CopyTo(DwString<tchar>* aStr) const
{
    assert(aStr != 0);
    if (!aStr) return;
    size_t len = mLength;
    size_t size = len + 1;
    tchar* buf = mem_alloc<tchar>(&size);
    assert(buf != 0);
    if (buf != 0) {
        mem_copy(mRep->mBuffer+mStart, len, buf);
        buf[len] = 0;
        DwStringRep<tchar>* rep = new DwStringRep<tchar>(buf, size);
        assert(rep != 0);
        if (rep != 0) {
            aStr->mRep = rep;
            delete_rep_safely(aStr->mRep);
            aStr->mStart = 0;
            aStr->mLength = len;
        }
    }
}


template<class tchar> void DwString<tchar>::_copy()
{
    if (mRep->mRefCount > 1) {
        size_t size = mLength + 1;
        tchar* newBuf = mem_alloc<tchar>(&size);
        assert(newBuf != 0);
        if (newBuf != 0) {
            tchar* to = newBuf;
            const tchar* from = mRep->mBuffer + mStart;
            mem_copy(from, mLength, to);
            to[mLength] = 0;
            DwStringRep<tchar>* rep = new DwStringRep<tchar>(newBuf, size);
            assert(rep != 0);
            if (rep != 0) {
                delete_rep_safely(mRep);
                mRep = rep;
                mStart = 0;
            }
            else /* if (rep == 0) */ {
                mem_free(newBuf);
                mLength = 0;
            }
        }
        else /* if (newBuf == 0) */ {
            mLength = 0;
        }
    }
}


template<class tchar> void DwString<tchar>::_replace(size_t aPos1, size_t aLen1, const tchar* aBuf, size_t aLen2)
{
    assert(aPos1 <= mLength);
    assert(aBuf != 0);
    size_t pos1 = std::min(aPos1, mLength);
    size_t len1 = std::min(aLen1, mLength - pos1);
    assert(mStart + mLength - len1 < ((size_t)-1) - aLen2);
    size_t len2 = std::min(aLen2, ((size_t)-1) - (mStart + mLength - len1));
    size_t i;
    tchar* to;
    const tchar* from;
    size_t newLen = (mLength - len1) + len2;
    // Is new string empty?
    if (newLen == 0 || aBuf == 0) {
        if (mRep != sEmptyRep) {
            delete_rep_safely(mRep);
            mRep = new_rep_reference(sEmptyRep);
            mStart = 0;
            mLength = 0;
        }
    }
    // Is buffer shared?  Is buffer too small?
    else if (mRep->mRefCount > 1 || newLen >= mRep->mSize) {
        size_t size = newLen + 1;
        tchar* newBuf = mem_alloc<tchar>(&size);
        assert(newBuf != 0);
        if (newBuf != 0) {
            to = newBuf;
            from = mRep->mBuffer + mStart;
            for (i=0; i < pos1; ++i) *to++ = *from++;
            from = aBuf;
            for (i=0; i < len2; ++i) *to++ = *from++;
            from = mRep->mBuffer + mStart + pos1 + len1;
            for (i=0; i < mLength - pos1 - len1; ++i) *to++ = *from++;
            *to = 0;
            DwStringRep<tchar>* rep = new DwStringRep<tchar>(newBuf, size);
            assert(rep != 0);
            if (rep != 0) {
                delete_rep_safely(mRep);
                mRep = rep;
                mStart = 0;
                mLength = newLen;
            }
        }
    }
    // Is the replacement smaller than the replaced?
    else if (len2 < len1) {
        to = mRep->mBuffer + mStart + pos1;
        from = aBuf;
        for (i=0; i < len2; ++i) *to++ = *from++;
        from = mRep->mBuffer + mStart + pos1 + len1;
        for (i=0; i < mLength - pos1 - len1; ++i) *to++ = *from++;
        *to = 0;
        mLength = newLen;
    }
    // Is there enough room at end of buffer?
    else if (mStart + newLen < mRep->mSize) {
        to = mRep->mBuffer + mStart + newLen;
        from = mRep->mBuffer + mStart + mLength - 1;
        *to-- = 0;
        for (i=0; i < mLength-pos1-len1; ++i) *to-- = *from--;
        from = aBuf + (len2 - 1);
        for (i=0; i < len2; ++i) *to-- = *from--;
        mLength = newLen;
    }
    // Is there enough room at beginning of buffer?
    else if (len2 - len1 <= mStart) {
        to = mRep->mBuffer + mStart - (len2 - len1);
        from = mRep->mBuffer + mStart;
        for (i=0; i < pos1; ++i) *to++ = *from++;
        from = aBuf;
        for (i=0; i < len2; ++i) *to++ = *from++;
        mStart -= len2 - len1;
        mLength = newLen;
    }
    // There's enough room, but we must move characters.
    else {
        to = mRep->mBuffer + newLen;
        from = mRep->mBuffer + mStart + mLength - 1;
        *to-- = 0;
        for (i=0; i < mLength-pos1-len1; ++i) *to-- = *from--;
        to = mRep->mBuffer;
        from = mRep->mBuffer + mStart;
        for (i=0; i < pos1; ++i) *to++ = *from++;
        from = aBuf;
        for (i=0; i < len2; ++i) *to++ = *from++;
        mStart = 0;
        mLength = newLen;
    }
}


template<class tchar> void DwString<tchar>::_replace(size_t aPos1, size_t aLen1, size_t aLen2, tchar aChar)
{
    assert(aPos1 <= mLength);
    size_t pos1 = std::min(aPos1, mLength);
    size_t len1 = std::min(aLen1, mLength - pos1);
    assert(mStart + mLength - len1 < ((size_t)-1) - aLen2);
    size_t len2 = std::min(aLen2, ((size_t)-1) - (mStart + mLength - len1));
    size_t i;
    tchar* to;
    const tchar* from;
    size_t newLen = mLength - len1 + len2;
    // Is new string empty?
    if (newLen == 0) {
        if (mRep != sEmptyRep) {
            delete_rep_safely(mRep);
            mRep = new_rep_reference(sEmptyRep);
            mStart = 0;
            mLength = 0;
        }
    }
    // Is buffer shared?  Is buffer too small?
    else if (mRep->mRefCount > 1 || newLen >= mRep->mSize) {
        size_t size = newLen + 1;
        tchar* newBuf = mem_alloc<tchar>(&size);
        assert(newBuf != 0);
        if (newBuf != 0) {
            to = newBuf;
            from = mRep->mBuffer + mStart;
            for (i=0; i < pos1; ++i) *to++ = *from++;
            for (i=0; i < len2; ++i) *to++ = aChar;
            from = mRep->mBuffer + mStart + pos1 + len1;
            for (i=0; i < mLength - pos1 - len1; ++i) *to++ = *from++;
            *to = 0;
            DwStringRep<tchar>* rep = new DwStringRep<tchar>(newBuf, size);
            assert(rep != 0);
            if (rep != 0) {
                delete_rep_safely(mRep);
                mRep = rep;
                mStart = 0;
                mLength = newLen;
            }
        }
    }
    // Is the replacement smaller than the replaced?
    else if (len2 < len1) {
        to = mRep->mBuffer + mStart + pos1;
        for (i=0; i < len2; ++i) *to++ = aChar;
        from = mRep->mBuffer + mStart + pos1 + len1;
        for (i=0; i < mLength - pos1 - len1; ++i) *to++ = *from++;
        *to = 0;
        mLength = newLen;
    }
    // Is there enough room at end of buffer?
    else if (mStart + newLen < mRep->mSize) {
        to = mRep->mBuffer + mStart + newLen;
        from = mRep->mBuffer + mStart + mLength - 1;
        *to-- = 0;
        for (i=0; i < mLength-pos1-len1; ++i) *to-- = *from--;
        for (i=0; i < len2; ++i) *to-- = aChar;
        mLength = newLen;
    }
    // Is there enough room at beginning of buffer?
    else if (len2 - len1 <= mStart) {
        to = mRep->mBuffer + mStart - (len2 - len1);
        from = mRep->mBuffer + mStart;
        for (i=0; i < pos1; ++i) *to++ = *from++;
        for (i=0; i < len2; ++i) *to++ = aChar;
        mStart -= len2 - len1;
        mLength = newLen;
    }
    // There's enough room, but we must move characters.
    else {
        to = mRep->mBuffer + newLen;
        from = mRep->mBuffer + mStart + mLength - 1;
        *to-- = 0;
        for (i=0; i < mLength-pos1-len1; ++i) *to-- = *from--;
        to = mRep->mBuffer;
        from = mRep->mBuffer + mStart;
        for (i=0; i < pos1; ++i) *to++ = *from++;
        for (i=0; i < len2; ++i) *to++ = aChar;
        mStart = 0;
        mLength = newLen;
    }
}

/*
#if defined (DW_DEBUG_VERSION)
void DwString<tchar>::PrintDebugInfo(std::ostream& aStrm) const
{
    aStrm <<
    "----------------- Debug info for DwString<tchar> class ----------------\n";
    aStrm << "Id:               " << ClassName() << ", " << ObjectId() << "\n";
    aStrm << "Rep:              " << (void*) mRep << "\n";
    aStrm << "Buffer:           " << (void*) mRep->mBuffer << "\n";
    aStrm << "Buffer size:      " << mRep->mSize << "\n";
    aStrm << "Start:            " << mStart << "\n";
    aStrm << "Length:           " << mLength << "\n";
    aStrm << "Contents:         ";
    for (size_t i=0; i < mLength && i < 64; ++i) {
        aStrm << mRep->mBuffer[mStart+i];
    }
    aStrm << endl;
}
#else
void DwString<tchar>::PrintDebugInfo(std::ostream& ) const {}
#endif // defined (DW_DEBUG_VERSION)
*/

template<class tchar> DwString<tchar> operator + (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2)
{
    DwString<tchar> str(aStr1);
    str.append(aStr2);
    return str;
}


template<class tchar> DwString<tchar> operator + (const tchar* aCstr, const DwString<tchar>& aStr2)
{
    DwString<tchar> str(aCstr);
    str.append(aStr2);
    return str;
}


template<class tchar> DwString<tchar> operator + (tchar aChar, const DwString<tchar>& aStr2)
{
    DwString<tchar> str(1, aChar);
    str.append(aStr2);
    return str;
}


template<class tchar> DwString<tchar> operator + (const DwString<tchar>& aStr1, const tchar* aCstr)
{
    DwString<tchar> str(aStr1);
    str.append(aCstr);
    return str;
}


template<class tchar> DwString<tchar> operator + (const DwString<tchar>& aStr1, tchar aChar)
{
    DwString<tchar> str(aStr1);
    str.append(1, aChar);
    return str;
}


template<class tchar> bool operator == (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2)
{
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r == 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator == (const DwString<tchar>& aStr1, const tchar* aCstr)
{
    assert(aCstr != 0);
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r == 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator == (const tchar* aCstr, const DwString<tchar>& aStr2)
{
    assert(aCstr != 0);
    const tchar* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r == 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator != (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2)
{
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r == 0) ? 0 : 1;
    return !!r;
}


template<class tchar> bool operator != (const DwString<tchar>& aStr1, const tchar* aCstr)
{
    assert(aCstr != 0);
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r == 0) ? 0 : 1;
    return !!r;
}


template<class tchar> bool operator != (const tchar* aCstr, const DwString<tchar>& aStr2)
{
    assert(aCstr != 0);
    const tchar* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r == 0) ? 0 : 1;
    return !!r;
}


template<class tchar> bool operator < (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2)
{
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r < 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator < (const DwString<tchar>& aStr1, const tchar* aCstr)
{
    assert(aCstr != 0);
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r < 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator < (const tchar* aCstr, const DwString<tchar>& aStr2)
{
    assert(aCstr != 0);
    const tchar* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r < 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator > (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2)
{
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r > 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator > (const DwString<tchar>& aStr1, const tchar* aCstr)
{
    assert(aCstr != 0);
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r > 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator > (const tchar* aCstr, const DwString<tchar>& aStr2)
{
    assert(aCstr != 0);
    const tchar* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r > 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator <= (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2)
{
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r <= 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator <= (const DwString<tchar>& aStr1, const tchar* aCstr)
{
    assert(aCstr != 0);
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r <= 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator <= (const tchar* aCstr, const DwString<tchar>& aStr2)
{
    assert(aCstr != 0);
    const tchar* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r <= 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator >= (const DwString<tchar>& aStr1, const DwString<tchar>& aStr2)
{
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r >= 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator >= (const DwString<tchar>& aStr1, const tchar* aCstr)
{
    assert(aCstr != 0);
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r >= 0) ? 1 : 0;
    return !!r;
}


template<class tchar> bool operator >= (const tchar* aCstr, const DwString<tchar>& aStr2)
{
    assert(aCstr != 0);
    const tchar* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    int r = dw_strcmp(s1, len1, s2, len2);
    r = (r >= 0) ? 1 : 0;
    return !!r;
}

template<class tchar> int DwStrcasecmp(const DwString<tchar>& aStr1, const DwString<tchar>& aStr2)
{
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    return dw_strcasecmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrcasecmp(const DwString<tchar>& aStr, const tchar* aCstr)
{
    assert(aCstr != 0);
    const tchar* s1 = aStr.data();
    size_t len1 = aStr.length();
    const tchar* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    return dw_strcasecmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrcasecmp(const tchar* aCstr, const DwString<tchar>& aStr)
{
    assert(aCstr != 0);
    const tchar* s1 = aCstr;
    size_t len1 =  (aCstr) ? strlen(aCstr) : 0;
    const tchar* s2 = aStr.data();
    size_t len2 = aStr.length();
    return dw_strcasecmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrncasecmp(const DwString<tchar>& aStr1, const DwString<tchar>& aStr2, size_t n)
{
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    len1 = std::min(len1, n);
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    len2 = std::min(len2, n);
    return dw_strcasecmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrncasecmp(const DwString<tchar>& aStr, const tchar* aCstr, size_t n)
{
    assert(aCstr != 0);
    const tchar* s1 = aStr.data();
    size_t len1 = aStr.length();
    len1 = std::min(len1, n);
    const tchar* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    len2 = std::min(len2, n);
    return dw_strcasecmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrncasecmp(const tchar* aCstr, const DwString<tchar>& aStr, size_t n)
{
    assert(aCstr != 0);
    const tchar* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    len1 = std::min(len1, n);
    const tchar* s2 = aStr.data();
    size_t len2 = aStr.length();
    len2 = std::min(len2, n);
    return dw_strcasecmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrcmp(const DwString<tchar>& aStr1, const DwString<tchar>& aStr2)
{
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    return dw_strcmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrcmp(const DwString<tchar>& aStr, const tchar* aCstr)
{
    assert(aCstr != 0);
    const tchar* s1 = aStr.data();
    size_t len1 = aStr.length();
    const tchar* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    return dw_strcmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrcmp(const tchar* aCstr, const DwString<tchar>& aStr)
{
    assert(aCstr != 0);
    const tchar* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    const tchar* s2 = aStr.data();
    size_t len2 = aStr.length();
    return dw_strcmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrncmp(const DwString<tchar>& aStr1, const DwString<tchar>& aStr2, size_t n)
{
    const tchar* s1 = aStr1.data();
    size_t len1 = aStr1.length();
    len1 = std::min(len1, n);
    const tchar* s2 = aStr2.data();
    size_t len2 = aStr2.length();
    len2 = std::min(len2, n);
    return dw_strcmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrncmp(const DwString<tchar>& aStr, const tchar* aCstr, size_t n)
{
    assert(aCstr != 0);
    const tchar* s1 = aStr.data();
    size_t len1 = aStr.length();
    len1 = std::min(len1, n);
    const tchar* s2 = aCstr;
    size_t len2 = (aCstr) ? strlen(aCstr) : 0;
    len2 = std::min(len2, n);
    return dw_strcmp(s1, len1, s2, len2);
}


template<class tchar> int DwStrncmp(const tchar* aCstr, const DwString<tchar>& aStr, size_t n)
{
    assert(aCstr != 0);
    const tchar* s1 = aCstr;
    size_t len1 = (aCstr) ? strlen(aCstr) : 0;
    len1 = std::min(len1, n);
    const tchar* s2 = aStr.data();
    size_t len2 = aStr.length();
    len2 = std::min(len2, n);
    return dw_strcmp(s1, len1, s2, len2);
}


template<class tchar> void DwStrcpy(DwString<tchar>& aStrDest, const DwString<tchar>& aStrSrc)
{
    aStrDest.assign(aStrSrc);
}


template<class tchar> void DwStrcpy(DwString<tchar>& aStrDest, const tchar* aCstrSrc)
{
    aStrDest.assign(aCstrSrc);
}


template<class tchar> void DwStrcpy(tchar* aCstrDest, const DwString<tchar>& aStrSrc)
{
    assert(aCstrDest != 0);
    const tchar* buf = aStrSrc.data();
    size_t len = aStrSrc.length();
    mem_copy(buf, len, aCstrDest);
    aCstrDest[len] = 0;
}


template<class tchar> void DwStrncpy(DwString<tchar>& aStrDest, const DwString<tchar>& aStrSrc, size_t n)
{
    aStrDest.assign(aStrSrc, 0, n);
}


template<class tchar> void DwStrncpy(DwString<tchar>& aStrDest, const tchar* aCstrSrc, size_t n)
{
    aStrDest.assign(aCstrSrc, 0, n);
}


template<class tchar> void DwStrncpy(tchar* aCstrDest, const DwString<tchar>& aStrSrc, size_t n)
{
    assert(aCstrDest != 0);
    const tchar* buf = aStrSrc.data();
    size_t len = aStrSrc.length();
    len = std::min(len, n);
    mem_copy(buf, len, aCstrDest);
    for (size_t i=len; i < n; ++i) {
        aCstrDest[i] = 0;
    }
}


template<class tchar> tchar* DwStrdup(const DwString<tchar>& aStr)
{
    size_t len = aStr.length();
    tchar* buf = new tchar[len+1];
    assert(buf != 0);
    if (buf != 0) {
        DwStrncpy(buf, aStr, len);
        buf[len] = 0;
    }
    return buf;
}

template<class tchar> DwString<tchar> DwString<tchar>::intToStr(int i)
{
 tchar s[20];
 return DwString(_itoa(i,s,10));
}

DwString<char> toansi(const DwString<wchar_t> &u)
{
 char *a=unicode16toAnsi(u.c_str());
 DwString<char> s(a);
 free(a);
 return s;
}

template<std::size_t=sizeof(std::size_t)> struct Fnv_hash
{
 static std::size_t hash(const char* first, std::size_t length)
  {
   std::size_t result = 0;
   for (; length > 0; --length)
    result = (result * 131) + *first++;
   return result;
  }
};
template<> struct Fnv_hash<4>
{
 static std::size_t hash(const char* first, std::size_t length)
  {
   std::size_t result = 2166136261UL;
   for (; length > 0; --length)
    {
     result ^= (std::size_t)*first++;
     result *= 16777619UL;
    }
   return result;
  }
};
#ifdef WIN64
template<> struct Fnv_hash<8>
{
 static std::size_t hash(const char* first, std::size_t length)
  {
   std::size_t result = 14695981039346656037ULL;
   for (; length > 0; --length)
    {
     result ^= (std::size_t)*first++;
     result *= 1099511628211ULL;
    }
   return result;
  }
};
#endif

size_t std::hash< DwString<char> >::operator()(const DwString<char> &s) const
{
 return Fnv_hash<>::hash(s.data(),s.size());
}
size_t std::hash< DwString<wchar_t> >::operator()(const DwString<wchar_t> &s) const
{
 return Fnv_hash<>::hash((const char*)s.data(),s.size()*sizeof(wchar_t));
}

template class DwString<char>;
template DwString<char> operator + (const DwString<char>& aStr1, const DwString<char>& aStr2);
template DwString<char> operator + (const char* aCstr, const DwString<char>& aStr2);
template DwString<char> operator + (char aChar, const DwString<char>& aStr2);
template DwString<char> operator + (const DwString<char>& aStr1, const char* aCstr);
template DwString<char> operator + (const DwString<char>& aStr1, char aChar);
template bool operator == (const DwString<char>& aStr1, const DwString<char>& aStr2);
template bool operator == (const DwString<char>& aStr1, const char* aCstr);
template bool operator == (const char* aCstr, const DwString<char>& aStr2);
template bool operator != (const DwString<char>& aStr1, const DwString<char>& aStr2);
template bool operator != (const DwString<char>& aStr1, const char* aCstr);
template bool operator != (const char* aCstr, const DwString<char>& aStr2);
template bool operator < (const DwString<char>& aStr1, const DwString<char>& aStr2);
template bool operator < (const DwString<char>& aStr1, const char* aCstr);
template bool operator < (const char* aCstr, const DwString<char>& aStr2);
template bool operator > (const DwString<char>& aStr1, const DwString<char>& aStr2);
template bool operator > (const DwString<char>& aStr1, const char* aCstr);
template bool operator > (const char* aCstr, const DwString<char>& aStr2);
template bool operator <= (const DwString<char>& aStr1, const DwString<char>& aStr2);
template bool operator <= (const DwString<char>& aStr1, const char* aCstr);
template bool operator <= (const char* aCstr, const DwString<char>& aStr2);
template bool operator >= (const DwString<char>& aStr1, const DwString<char>& aStr2);
template bool operator >= (const DwString<char>& aStr1, const char* aCstr);
template bool operator >= (const char* aCstr, const DwString<char>& aStr2);
template int DwStrcasecmp(const DwString<char>& aStr1, const DwString<char>& aStr2);
template int DwStrcasecmp(const DwString<char>& aStr1, const char* aCstr);
template int DwStrcasecmp(const char* aCstr, const DwString<char>& aStr2);
template int DwStrncasecmp(const DwString<char>& aStr1, const DwString<char>& aStr2, size_t aLen);
template int DwStrncasecmp(const DwString<char>& aStr, const char* aCstr, size_t aLen);
template int DwStrncasecmp(const char* aCstr, const DwString<char>& aStr, size_t aLen);
template int DwStrcmp(const DwString<char>& aStr1, const DwString<char>& aStr2);
template int DwStrcmp(const DwString<char>& aStr, const char* aCstr);
template int DwStrcmp(const char* aCstr, const DwString<char>& aStr);
template int DwStrncmp(const DwString<char>& aStr1, const DwString<char>& aStr2, size_t aLen);
template int DwStrncmp(const DwString<char>& aStr, const char* aCstr, size_t aLen);
template int DwStrncmp(const char* aCstr, const DwString<char>& aStr, size_t aLen);
template void DwStrcpy(DwString<char>& aStrDest, const DwString<char>& aStrSrc);
template void DwStrcpy(DwString<char>& aStrDest, const char* aCstrSrc);
template void DwStrcpy(char* aCstrDest, const DwString<char>& aStrSrc);
template void DwStrncpy(DwString<char>& aStrDest, const DwString<char>& aStrSrc, size_t aLen);
template void DwStrncpy(DwString<char>& aStrDest, const char* aCstrSrc, size_t aLen);
template void DwStrncpy(char* aCstrDest, const DwString<char>& aStrSrc, size_t aLen);
template char* DwStrdup(const DwString<char>& aStr);

template class DwString<wchar_t>;
template DwString<wchar_t> operator + (const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2);
template DwString<wchar_t> operator + (const wchar_t* aCstr, const DwString<wchar_t>& aStr2);
template DwString<wchar_t> operator + (wchar_t aChar, const DwString<wchar_t>& aStr2);
template DwString<wchar_t> operator + (const DwString<wchar_t>& aStr1, const wchar_t* aCstr);
template DwString<wchar_t> operator + (const DwString<wchar_t>& aStr1, wchar_t aChar);
template bool operator == (const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2);
template bool operator == (const DwString<wchar_t>& aStr1, const wchar_t* aCstr);
template bool operator == (const wchar_t* aCstr, const DwString<wchar_t>& aStr2);
template bool operator != (const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2);
template bool operator != (const DwString<wchar_t>& aStr1, const wchar_t* aCstr);
template bool operator != (const wchar_t* aCstr, const DwString<wchar_t>& aStr2);
template bool operator < (const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2);
template bool operator < (const DwString<wchar_t>& aStr1, const wchar_t* aCstr);
template bool operator < (const wchar_t* aCstr, const DwString<wchar_t>& aStr2);
template bool operator > (const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2);
template bool operator > (const DwString<wchar_t>& aStr1, const wchar_t* aCstr);
template bool operator > (const wchar_t* aCstr, const DwString<wchar_t>& aStr2);
template bool operator <= (const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2);
template bool operator <= (const DwString<wchar_t>& aStr1, const wchar_t* aCstr);
template bool operator <= (const wchar_t* aCstr, const DwString<wchar_t>& aStr2);
template bool operator >= (const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2);
template bool operator >= (const DwString<wchar_t>& aStr1, const wchar_t* aCstr);
template bool operator >= (const wchar_t* aCstr, const DwString<wchar_t>& aStr2);
template int DwStrcasecmp(const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2);
template int DwStrcasecmp(const DwString<wchar_t>& aStr1, const wchar_t* aCstr);
template int DwStrcasecmp(const wchar_t* aCstr, const DwString<wchar_t>& aStr2);
template int DwStrncasecmp(const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2, size_t aLen);
template int DwStrncasecmp(const DwString<wchar_t>& aStr, const wchar_t* aCstr, size_t aLen);
template int DwStrncasecmp(const wchar_t* aCstr, const DwString<wchar_t>& aStr, size_t aLen);
template int DwStrcmp(const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2);
template int DwStrcmp(const DwString<wchar_t>& aStr, const wchar_t* aCstr);
template int DwStrcmp(const wchar_t* aCstr, const DwString<wchar_t>& aStr);
template int DwStrncmp(const DwString<wchar_t>& aStr1, const DwString<wchar_t>& aStr2, size_t aLen);
template int DwStrncmp(const DwString<wchar_t>& aStr, const wchar_t* aCstr, size_t aLen);
template int DwStrncmp(const wchar_t* aCstr, const DwString<wchar_t>& aStr, size_t aLen);
template void DwStrcpy(DwString<wchar_t>& aStrDest, const DwString<wchar_t>& aStrSrc);
template void DwStrcpy(DwString<wchar_t>& aStrDest, const wchar_t* aCstrSrc);
template void DwStrcpy(wchar_t* aCstrDest, const DwString<wchar_t>& aStrSrc);
template void DwStrncpy(DwString<wchar_t>& aStrDest, const DwString<wchar_t>& aStrSrc, size_t aLen);
template void DwStrncpy(DwString<wchar_t>& aStrDest, const wchar_t* aCstrSrc, size_t aLen);
template void DwStrncpy(wchar_t* aCstrDest, const DwString<wchar_t>& aStrSrc, size_t aLen);
template wchar_t* DwStrdup(const DwString<wchar_t>& aStr);

//============================= passtring ===========================
template<> char passtring<char>::chr='\0';
template<> wchar_t passtring<wchar_t>::chr=L'\0';
