#ifndef _IVMRFFDSHOW9_H_
#define _IVMRFFDSHOW9_H_

#include "stdafx.h"

// support "Queue output samples" of ffdshow
// IVMRffdshow9 is implemented in CVMR9AllocatorPresenter in Media Player Classic.
// We can use this to check MPC version.

MIDL_INTERFACE("A273C7F6-25D4-46b0-B2C8-4F7FADC44E37")
IVMRffdshow9 : public IUnknown
{
public:
	virtual STDMETHODIMP support_ffdshow(void) = 0;
};

#endif
