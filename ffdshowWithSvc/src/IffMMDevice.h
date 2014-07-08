#define __RPC__in
#define __RPC__out

#if (_MSC_VER<1400) // VC 7.1 or lower
	#ifndef __in
	#define __in
	#endif

	#ifndef __deref_out
	#define __deref_out
	#endif

	#ifndef __in_opt
	#define __in_opt
	#endif

	#ifndef __out
	#define __out
	#endif
#endif

#ifndef PROPERTYKEY_DEFINED
#define PROPERTYKEY_DEFINED
typedef struct _tagpropertykey
    {
    GUID fmtid;
    DWORD pid;
    } 	PROPERTYKEY;

#endif

// This is the standard C layout of the PROPVARIANT.
typedef struct tagPROPVARIANT * LPPROPVARIANT;

#ifndef _REFPROPVARIANT_DEFINED
#define _REFPROPVARIANT_DEFINED
#ifdef __cplusplus
#define REFPROPVARIANT const PROPVARIANT &
#else
#define REFPROPVARIANT const PROPVARIANT * __MIDL_CONST
#endif
#endif

#ifndef REFPROPERTYKEY
#ifdef __cplusplus
#define REFPROPERTYKEY const PROPERTYKEY &
#else // !__cplusplus
#define REFPROPERTYKEY const PROPERTYKEY * __MIDL_CONST
#endif // __cplusplus
#endif //REFPROPERTYKEY

#ifdef DEFINE_PROPERTYKEY
#undef DEFINE_PROPERTYKEY
#endif

#ifdef INITGUID
#define DEFINE_PROPERTYKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const PROPERTYKEY DECLSPEC_SELECTANY name = { { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }, pid }
#else
#define DEFINE_PROPERTYKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const PROPERTYKEY name
#endif // INITGUID

#ifndef IsEqualPropertyKey
#define IsEqualPropertyKey(a, b)   (((a).pid == (b).pid) && IsEqualIID((a).fmtid, (b).fmtid) )
#endif  // IsEqualPropertyKey



DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_FormFactor, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 0); 
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_ControlPanelPageProvider, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 1); 
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_Association, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 2);
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_PhysicalSpeakers, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 3);
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_GUID, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 4);
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_Disable_SysFx, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 5);
#define ENDPOINT_SYSFX_ENABLED          0x00000000  // System Effects are enabled.
#define ENDPOINT_SYSFX_DISABLED         0x00000001  // System Effects are disabled.
DEFINE_PROPERTYKEY(PKEY_AudioEndpoint_FullRangeSpeakers, 0x1da5d803, 0xd492, 0x4edd, 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e, 6);
DEFINE_PROPERTYKEY(PKEY_AudioEngine_DeviceFormat, 0xf19f064d, 0x82c, 0x4e27, 0xbc, 0x73, 0x68, 0x82, 0xa1, 0xbb, 0x8e, 0x4c, 0); 
//
// _NAME
//

DEFINE_PROPERTYKEY(PKEY_NAME,                          0xb725f130, 0x47ef, 0x101a, 0xa5, 0xf1, 0x02, 0x60, 0x8c, 0x9e, 0xeb, 0xac, 10);    // DEVPROP_TYPE_STRING

//
// Device properties
// These PKEYs correspond to the old setupapi SPDRP_XXX properties
//
DEFINE_PROPERTYKEY(PKEY_Device_DeviceDesc,             0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 2);     // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_HardwareIds,            0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 3);     // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_Device_CompatibleIds,          0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 4);     // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_Device_Service,                0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 6);     // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_Class,                  0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 9);     // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_ClassGuid,              0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 10);    // DEVPROP_TYPE_GUID
DEFINE_PROPERTYKEY(PKEY_Device_Driver,                 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 11);    // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_ConfigFlags,            0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 12);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_Manufacturer,           0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 13);    // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_FriendlyName,           0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14);    // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_LocationInfo,           0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 15);    // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_PDOName,                0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 16);    // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_Capabilities,           0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 17);    // DEVPROP_TYPE_UNINT32
DEFINE_PROPERTYKEY(PKEY_Device_UINumber,               0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 18);    // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_UpperFilters,           0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 19);    // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_Device_LowerFilters,           0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 20);    // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_Device_BusTypeGuid,            0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 21);    // DEVPROP_TYPE_GUID
DEFINE_PROPERTYKEY(PKEY_Device_LegacyBusType,          0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 22);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_BusNumber,              0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 23);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_EnumeratorName,         0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 24);    // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_Security,               0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 25);    // DEVPROP_TYPE_SECURITY_DESCRIPTOR
DEFINE_PROPERTYKEY(PKEY_Device_SecuritySDS,            0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 26);    // DEVPROP_TYPE_SECURITY_DESCRIPTOR_STRING
DEFINE_PROPERTYKEY(PKEY_Device_DevType,                0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 27);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_Exclusive,              0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 28);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_Characteristics,        0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 29);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_Address,                0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 30);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_UINumberDescFormat,     0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 31);    // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_PowerData,              0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 32);    // DEVPROP_TYPE_BINARY
DEFINE_PROPERTYKEY(PKEY_Device_RemovalPolicy,          0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 33);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_RemovalPolicyDefault,   0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 34);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_RemovalPolicyOverride,  0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 35);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_InstallState,           0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 36);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_LocationPaths,          0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 37);    // DEVPROP_TYPE_STRING_LIST

//
// Device properties
// These PKEYs correspond to a device's status and problem code
//
DEFINE_PROPERTYKEY(PKEY_Device_DevNodeStatus,          0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 2);     // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_ProblemCode,            0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 3);     // DEVPROP_TYPE_UINT32

//
// Device properties
// These PKEYs correspond to device relations
//
DEFINE_PROPERTYKEY(PKEY_Device_EjectionRelations,      0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 4);     // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_Device_RemovalRelations,       0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 5);     // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_Device_PowerRelations,         0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 6);     // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_Device_BusRelations,           0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 7);     // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_Device_Parent,                 0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 8);     // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_Children,               0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 9);     // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_Device_Siblings,               0x4340a6c5, 0x93fa, 0x4706, 0x97, 0x2c, 0x7b, 0x64, 0x80, 0x08, 0xa5, 0xa7, 10);    // DEVPROP_TYPE_STRING_LIST

//
// Other Device properties
//
DEFINE_PROPERTYKEY(PKEY_Device_Reported,               0x80497100, 0x8c73, 0x48b9, 0xaa, 0xd9, 0xce, 0x38, 0x7e, 0x19, 0xc5, 0x6e, 2);     // DEVPROP_TYPE_BOOLEAN
DEFINE_PROPERTYKEY(PKEY_Device_Legacy,                 0x80497100, 0x8c73, 0x48b9, 0xaa, 0xd9, 0xce, 0x38, 0x7e, 0x19, 0xc5, 0x6e, 3);     // DEVPROP_TYPE_BOOLEAN
DEFINE_PROPERTYKEY(PKEY_Device_InstanceId,             0x78c34fc8, 0x104a, 0x4aca, 0x9e, 0xa4, 0x52, 0x4d, 0x52, 0x99, 0x6e, 0x57, 256);   // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Numa_Proximity_Domain,         0x540b947e, 0x8b40, 0x45bc, 0xa8, 0xa2, 0x6a, 0x0b, 0x89, 0x4c, 0xbd, 0xa2, 1);     // DEVPROP_TYPE_UINT32

//
// Device driver properties
//
DEFINE_PROPERTYKEY(PKEY_Device_DriverDate,             0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 2);      // DEVPROP_TYPE_FILETIME
DEFINE_PROPERTYKEY(PKEY_Device_DriverVersion,          0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 3);      // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_DriverDesc,             0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 4);      // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_DriverInfPath,          0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 5);      // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_DriverInfSection,       0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 6);      // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_DriverInfSectionExt,    0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 7);      // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_MatchingDeviceId,       0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 8);      // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_DriverProvider,         0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 9);      // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_DriverPropPageProvider, 0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 10);     // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_DriverCoInstallers,     0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 11);     // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_Device_ResourcePickerTags,     0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 12);     // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_ResourcePickerExceptions, 0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 13); // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_Device_DriverRank,             0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 14);     // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_DriverLogoLevel,        0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 15);     // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_Device_NoConnectSound,         0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 17);     // DEVPROP_TYPE_BOOLEAN
DEFINE_PROPERTYKEY(PKEY_Device_GenericDriverInstalled, 0xa8b865dd, 0x2e3d, 0x4094, 0xad, 0x97, 0xe5, 0x93, 0xa7, 0xc, 0x75, 0xd6, 18);     // DEVPROP_TYPE_BOOLEAN


//
// Device properties that were set by the driver package that was installed
// on the device.
//
DEFINE_PROPERTYKEY(PKEY_DrvPkg_Model,                  0xcf73bb51, 0x3abf, 0x44a2, 0x85, 0xe0, 0x9a, 0x3d, 0xc7, 0xa1, 0x21, 0x32, 2);     // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DrvPkg_VendorWebSite,          0xcf73bb51, 0x3abf, 0x44a2, 0x85, 0xe0, 0x9a, 0x3d, 0xc7, 0xa1, 0x21, 0x32, 3);     // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DrvPkg_DetailedDescription,    0xcf73bb51, 0x3abf, 0x44a2, 0x85, 0xe0, 0x9a, 0x3d, 0xc7, 0xa1, 0x21, 0x32, 4);     // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DrvPkg_DocumentationLink,      0xcf73bb51, 0x3abf, 0x44a2, 0x85, 0xe0, 0x9a, 0x3d, 0xc7, 0xa1, 0x21, 0x32, 5);     // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DrvPkg_Icon,                   0xcf73bb51, 0x3abf, 0x44a2, 0x85, 0xe0, 0x9a, 0x3d, 0xc7, 0xa1, 0x21, 0x32, 6);     // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_DrvPkg_BrandingIcon,           0xcf73bb51, 0x3abf, 0x44a2, 0x85, 0xe0, 0x9a, 0x3d, 0xc7, 0xa1, 0x21, 0x32, 7);     // DEVPROP_TYPE_STRING_LIST

//
// Device setup class properties
// These PKEYs correspond to the old setupapi SPCRP_XXX properties
//
DEFINE_PROPERTYKEY(PKEY_DeviceClass_UpperFilters,      0x4321918b, 0xf69e, 0x470d, 0xa5, 0xde, 0x4d, 0x88, 0xc7, 0x5a, 0xd2, 0x4b, 19);    // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_DeviceClass_LowerFilters,      0x4321918b, 0xf69e, 0x470d, 0xa5, 0xde, 0x4d, 0x88, 0xc7, 0x5a, 0xd2, 0x4b, 20);    // DEVPROP_TYPE_STRING_LIST
DEFINE_PROPERTYKEY(PKEY_DeviceClass_Security,          0x4321918b, 0xf69e, 0x470d, 0xa5, 0xde, 0x4d, 0x88, 0xc7, 0x5a, 0xd2, 0x4b, 25);    // DEVPROP_TYPE_SECURITY_DESCRIPTOR
DEFINE_PROPERTYKEY(PKEY_DeviceClass_SecuritySDS,       0x4321918b, 0xf69e, 0x470d, 0xa5, 0xde, 0x4d, 0x88, 0xc7, 0x5a, 0xd2, 0x4b, 26);    // DEVPROP_TYPE_SECURITY_DESCRIPTOR_STRING
DEFINE_PROPERTYKEY(PKEY_DeviceClass_DevType,           0x4321918b, 0xf69e, 0x470d, 0xa5, 0xde, 0x4d, 0x88, 0xc7, 0x5a, 0xd2, 0x4b, 27);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_DeviceClass_Exclusive,         0x4321918b, 0xf69e, 0x470d, 0xa5, 0xde, 0x4d, 0x88, 0xc7, 0x5a, 0xd2, 0x4b, 28);    // DEVPROP_TYPE_UINT32
DEFINE_PROPERTYKEY(PKEY_DeviceClass_Characteristics,   0x4321918b, 0xf69e, 0x470d, 0xa5, 0xde, 0x4d, 0x88, 0xc7, 0x5a, 0xd2, 0x4b, 29);    // DEVPROP_TYPE_UINT32

//
// Device setup class properties
// These PKEYs correspond to registry values under the device class GUID key
//
DEFINE_PROPERTYKEY(PKEY_DeviceClass_Name,              0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 2);  // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DeviceClass_ClassName,         0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 3);  // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DeviceClass_Icon,              0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 4);  // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DeviceClass_ClassInstaller,    0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 5);  // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DeviceClass_PropPageProvider,  0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 6);  // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DeviceClass_NoInstallClass,    0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 7);  // DEVPROP_TYPE_BOOLEAN
DEFINE_PROPERTYKEY(PKEY_DeviceClass_NoDisplayClass,    0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 8);  // DEVPROP_TYPE_BOOLEAN
DEFINE_PROPERTYKEY(PKEY_DeviceClass_SilentInstall,     0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 9);  // DEVPROP_TYPE_BOOLEAN
DEFINE_PROPERTYKEY(PKEY_DeviceClass_NoUseClass,        0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 10); // DEVPROP_TYPE_BOOLEAN
DEFINE_PROPERTYKEY(PKEY_DeviceClass_DefaultService,    0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 11); // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DeviceClass_IconPath,          0x259abffc, 0x50a7, 0x47ce, 0xaf, 0x8, 0x68, 0xc9, 0xa7, 0xd7, 0x33, 0x66, 12); // DEVPROP_TYPE_STRING_LIST

//
// Other Device setup class properties
//
DEFINE_PROPERTYKEY(PKEY_DeviceClass_ClassCoInstallers, 0x713d1703, 0xa2e2, 0x49f5, 0x92, 0x14, 0x56, 0x47, 0x2e, 0xf3, 0xda, 0x5c, 2); // DEVPROP_TYPE_STRING_LIST

//
// Device interface properties
//
DEFINE_PROPERTYKEY(PKEY_DeviceInterface_FriendlyName,  0x026e516e, 0xb814, 0x414b, 0x83, 0xcd, 0x85, 0x6d, 0x6f, 0xef, 0x48, 0x22, 2); // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DeviceInterface_Enabled,       0x026e516e, 0xb814, 0x414b, 0x83, 0xcd, 0x85, 0x6d, 0x6f, 0xef, 0x48, 0x22, 3); // DEVPROP_TYPE_BOOLEAN
DEFINE_PROPERTYKEY(PKEY_DeviceInterface_ClassGuid,     0x026e516e, 0xb814, 0x414b, 0x83, 0xcd, 0x85, 0x6d, 0x6f, 0xef, 0x48, 0x22, 4); // DEVPROP_TYPE_GUID

//
// Device interface class properties
//
DEFINE_PROPERTYKEY(PKEY_DeviceInterfaceClass_DefaultInterface,  0x14c83a99, 0x0b3f, 0x44b7, 0xbe, 0x4c, 0xa1, 0x78, 0xd3, 0x99, 0x05, 0x64, 2); // DEVPROP_TYPE_STRING

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IMMNotificationClient_FWD_DEFINED__
#define __IMMNotificationClient_FWD_DEFINED__
typedef interface IMMNotificationClient IMMNotificationClient;
#endif 	/* __IMMNotificationClient_FWD_DEFINED__ */


#ifndef __IMMDevice_FWD_DEFINED__
#define __IMMDevice_FWD_DEFINED__
typedef interface IMMDevice IMMDevice;
#endif 	/* __IMMDevice_FWD_DEFINED__ */


#ifndef __IMMDeviceCollection_FWD_DEFINED__
#define __IMMDeviceCollection_FWD_DEFINED__
typedef interface IMMDeviceCollection IMMDeviceCollection;
#endif 	/* __IMMDeviceCollection_FWD_DEFINED__ */


#ifndef __IMMEndpoint_FWD_DEFINED__
#define __IMMEndpoint_FWD_DEFINED__
typedef interface IMMEndpoint IMMEndpoint;
#endif 	/* __IMMEndpoint_FWD_DEFINED__ */


#ifndef __IMMDeviceEnumerator_FWD_DEFINED__
#define __IMMDeviceEnumerator_FWD_DEFINED__
typedef interface IMMDeviceEnumerator IMMDeviceEnumerator;
#endif 	/* __IMMDeviceEnumerator_FWD_DEFINED__ */


#ifndef __IMMDeviceActivator_FWD_DEFINED__
#define __IMMDeviceActivator_FWD_DEFINED__
typedef interface IMMDeviceActivator IMMDeviceActivator;
#endif 	/* __IMMDeviceActivator_FWD_DEFINED__ */


#ifndef __MMDeviceEnumerator_FWD_DEFINED__
#define __MMDeviceEnumerator_FWD_DEFINED__

#ifdef __cplusplus
typedef class MMDeviceEnumerator MMDeviceEnumerator;
#else
typedef struct MMDeviceEnumerator MMDeviceEnumerator;
#endif /* __cplusplus */
#endif 	/* __MMDeviceEnumerator_FWD_DEFINED__ */

#ifndef __IPropertyStore_FWD_DEFINED__
#define __IPropertyStore_FWD_DEFINED__
typedef interface IPropertyStore IPropertyStore;
#endif 	/* __IPropertyStore_FWD_DEFINED__ */

/*#ifdef __cplusplus
extern "C"{
#endif */

/* interface __MIDL_itf_mmdeviceapi_0000_0000 */
/* [local] */ 

#define E_NOTFOUND HRESULT_FROM_WIN32(ERROR_NOT_FOUND)
#define E_UNSUPPORTED_TYPE HRESULT_FROM_WIN32(ERROR_UNSUPPORTED_TYPE)
#define DEVICE_STATE_ACTIVE      0x00000001
#define DEVICE_STATE_DISABLED    0x00000002
#define DEVICE_STATE_NOTPRESENT  0x00000004
#define DEVICE_STATE_UNPLUGGED   0x00000008
#define DEVICE_STATEMASK_ALL     0x0000000f



typedef struct tagDIRECTX_AUDIO_ACTIVATION_PARAMS
    {
    DWORD cbDirectXAudioActivationParams;
    GUID guidAudioSession;
    DWORD dwAudioStreamFlags;
    } 	DIRECTX_AUDIO_ACTIVATION_PARAMS;

typedef struct tagDIRECTX_AUDIO_ACTIVATION_PARAMS *PDIRECTX_AUDIO_ACTIVATION_PARAMS;

typedef /* [public][public][public][public][public] */ 
enum __MIDL___MIDL_itf_mmdeviceapi_0000_0000_0001
    {	eRender	= 0,
	eCapture	= ( eRender + 1 ) ,
	eAll	= ( eCapture + 1 ) ,
	EDataFlow_enum_count	= ( eAll + 1 ) 
    } 	EDataFlow;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_mmdeviceapi_0000_0000_0002
    {	eConsole	= 0,
	eMultimedia	= ( eConsole + 1 ) ,
	eCommunications	= ( eMultimedia + 1 ) ,
	ERole_enum_count	= ( eCommunications + 1 ) 
    } 	ERole;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_mmdeviceapi_0000_0000_0003
    {	RemoteNetworkDevice	= 0,
	Speakers	= ( RemoteNetworkDevice + 1 ) ,
	LineLevel	= ( Speakers + 1 ) ,
	Headphones	= ( LineLevel + 1 ) ,
	Microphone	= ( Headphones + 1 ) ,
	Headset	= ( Microphone + 1 ) ,
	Handset	= ( Headset + 1 ) ,
	UnknownDigitalPassthrough	= ( Handset + 1 ) ,
	SPDIF	= ( UnknownDigitalPassthrough + 1 ) ,
	HDMI	= ( SPDIF + 1 ) ,
	UnknownFormFactor	= ( HDMI + 1 ) 
    } 	EndpointFormFactor;



extern RPC_IF_HANDLE __MIDL_itf_mmdeviceapi_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mmdeviceapi_0000_0000_v0_0_s_ifspec;


#ifndef __IPropertyStore_INTERFACE_DEFINED__
#define __IPropertyStore_INTERFACE_DEFINED__
/* interface IPropertyStore */
/* [unique][object][helpstring][uuid] */ 


EXTERN_C const IID IID_IPropertyStore;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("886d8eeb-8cf2-4446-8d02-cdba1dbdcf99")
    IPropertyStore : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ __RPC__out DWORD *cProps) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ DWORD iProp,
            /* [out] */ __RPC__out PROPERTYKEY *pkey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetValue( 
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [out] */ __RPC__out PROPVARIANT *pv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetValue( 
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [in] */ __RPC__in REFPROPVARIANT propvar) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPropertyStoreVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPropertyStore * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPropertyStore * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPropertyStore * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IPropertyStore * This,
            /* [out] */ __RPC__out DWORD *cProps);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            IPropertyStore * This,
            /* [in] */ DWORD iProp,
            /* [out] */ __RPC__out PROPERTYKEY *pkey);
        
        HRESULT ( STDMETHODCALLTYPE *GetValue )( 
            IPropertyStore * This,
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [out] */ __RPC__out PROPVARIANT *pv);
        
        HRESULT ( STDMETHODCALLTYPE *SetValue )( 
            IPropertyStore * This,
            /* [in] */ __RPC__in REFPROPERTYKEY key,
            /* [in] */ __RPC__in REFPROPVARIANT propvar);
        
        HRESULT ( STDMETHODCALLTYPE *Commit )( 
            IPropertyStore * This);
        
        END_INTERFACE
    } IPropertyStoreVtbl;

    interface IPropertyStore
    {
        CONST_VTBL struct IPropertyStoreVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertyStore_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPropertyStore_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPropertyStore_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPropertyStore_GetCount(This,cProps)	\
    ( (This)->lpVtbl -> GetCount(This,cProps) ) 

#define IPropertyStore_GetAt(This,iProp,pkey)	\
    ( (This)->lpVtbl -> GetAt(This,iProp,pkey) ) 

#define IPropertyStore_GetValue(This,key,pv)	\
    ( (This)->lpVtbl -> GetValue(This,key,pv) ) 

#define IPropertyStore_SetValue(This,key,propvar)	\
    ( (This)->lpVtbl -> SetValue(This,key,propvar) ) 

#define IPropertyStore_Commit(This)	\
    ( (This)->lpVtbl -> Commit(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */

#endif 	/* __IPropertyStore_INTERFACE_DEFINED__ */


#ifndef __IMMNotificationClient_INTERFACE_DEFINED__
#define __IMMNotificationClient_INTERFACE_DEFINED__

/* interface IMMNotificationClient */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IMMNotificationClient;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7991EEC9-7E89-4D85-8390-6C703CEC60C0")
    IMMNotificationClient : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnDeviceStateChanged( 
            /* [in] */ 
            __in  LPCWSTR pwstrDeviceId,
            /* [in] */ 
            __in  DWORD dwNewState) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnDeviceAdded( 
            /* [in] */ 
            __in  LPCWSTR pwstrDeviceId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnDeviceRemoved( 
            /* [in] */ 
            __in  LPCWSTR pwstrDeviceId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged( 
            /* [in] */ 
            __in  EDataFlow flow,
            /* [in] */ 
            __in  ERole role,
            /* [in] */ 
            __in  LPCWSTR pwstrDefaultDeviceId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnPropertyValueChanged( 
            /* [in] */ 
            __in  LPCWSTR pwstrDeviceId,
            /* [in] */ 
            __in  const PROPERTYKEY key) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMMNotificationClientVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMMNotificationClient * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMMNotificationClient * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMMNotificationClient * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnDeviceStateChanged )( 
            IMMNotificationClient * This,
            /* [in] */ 
            __in  LPCWSTR pwstrDeviceId,
            /* [in] */ 
            __in  DWORD dwNewState);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnDeviceAdded )( 
            IMMNotificationClient * This,
            /* [in] */ 
            __in  LPCWSTR pwstrDeviceId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnDeviceRemoved )( 
            IMMNotificationClient * This,
            /* [in] */ 
            __in  LPCWSTR pwstrDeviceId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnDefaultDeviceChanged )( 
            IMMNotificationClient * This,
            /* [in] */ 
            __in  EDataFlow flow,
            /* [in] */ 
            __in  ERole role,
            /* [in] */ 
            __in  LPCWSTR pwstrDefaultDeviceId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnPropertyValueChanged )( 
            IMMNotificationClient * This,
            /* [in] */ 
            __in  LPCWSTR pwstrDeviceId,
            /* [in] */ 
            __in  const PROPERTYKEY key);
        
        END_INTERFACE
    } IMMNotificationClientVtbl;

    interface IMMNotificationClient
    {
        CONST_VTBL struct IMMNotificationClientVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMMNotificationClient_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMMNotificationClient_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMMNotificationClient_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMMNotificationClient_OnDeviceStateChanged(This,pwstrDeviceId,dwNewState)	\
    ( (This)->lpVtbl -> OnDeviceStateChanged(This,pwstrDeviceId,dwNewState) ) 

#define IMMNotificationClient_OnDeviceAdded(This,pwstrDeviceId)	\
    ( (This)->lpVtbl -> OnDeviceAdded(This,pwstrDeviceId) ) 

#define IMMNotificationClient_OnDeviceRemoved(This,pwstrDeviceId)	\
    ( (This)->lpVtbl -> OnDeviceRemoved(This,pwstrDeviceId) ) 

#define IMMNotificationClient_OnDefaultDeviceChanged(This,flow,role,pwstrDefaultDeviceId)	\
    ( (This)->lpVtbl -> OnDefaultDeviceChanged(This,flow,role,pwstrDefaultDeviceId) ) 

#define IMMNotificationClient_OnPropertyValueChanged(This,pwstrDeviceId,key)	\
    ( (This)->lpVtbl -> OnPropertyValueChanged(This,pwstrDeviceId,key) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMMNotificationClient_INTERFACE_DEFINED__ */


#ifndef __IMMDevice_INTERFACE_DEFINED__
#define __IMMDevice_INTERFACE_DEFINED__

/* interface IMMDevice */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IMMDevice;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D666063F-1587-4E43-81F1-B948E807363F")
    IMMDevice : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Activate( 
            /* [in] */ 
            __in  REFIID iid,
            /* [in] */ 
            __in  DWORD dwClsCtx,
            /* [unique][in] */ 
            __in_opt  PROPVARIANT *pActivationParams,
            /* [iid_is][out] */ 
            __out  void **ppInterface) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenPropertyStore( 
            /* [in] */ 
            __in  DWORD stgmAccess,
            /* [out] */ 
            __out  IPropertyStore **ppProperties) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetId( 
            /* [out] */ 
            __deref_out  LPWSTR *ppstrId) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetState( 
            /* [out] */ 
            __out  DWORD *pdwState) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMMDeviceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMMDevice * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMMDevice * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMMDevice * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Activate )( 
            IMMDevice * This,
            /* [in] */ 
            __in  REFIID iid,
            /* [in] */ 
            __in  DWORD dwClsCtx,
            /* [unique][in] */ 
            __in_opt  PROPVARIANT *pActivationParams,
            /* [iid_is][out] */ 
            __out  void **ppInterface);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenPropertyStore )( 
            IMMDevice * This,
            /* [in] */ 
            __in  DWORD stgmAccess,
            /* [out] */ 
            __out  IPropertyStore **ppProperties);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetId )( 
            IMMDevice * This,
            /* [out] */ 
            __deref_out  LPWSTR *ppstrId);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IMMDevice * This,
            /* [out] */ 
            __out  DWORD *pdwState);
        
        END_INTERFACE
    } IMMDeviceVtbl;

    interface IMMDevice
    {
        CONST_VTBL struct IMMDeviceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMMDevice_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMMDevice_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMMDevice_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMMDevice_Activate(This,iid,dwClsCtx,pActivationParams,ppInterface)	\
    ( (This)->lpVtbl -> Activate(This,iid,dwClsCtx,pActivationParams,ppInterface) ) 

#define IMMDevice_OpenPropertyStore(This,stgmAccess,ppProperties)	\
    ( (This)->lpVtbl -> OpenPropertyStore(This,stgmAccess,ppProperties) ) 

#define IMMDevice_GetId(This,ppstrId)	\
    ( (This)->lpVtbl -> GetId(This,ppstrId) ) 

#define IMMDevice_GetState(This,pdwState)	\
    ( (This)->lpVtbl -> GetState(This,pdwState) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMMDevice_INTERFACE_DEFINED__ */


#ifndef __IMMDeviceCollection_INTERFACE_DEFINED__
#define __IMMDeviceCollection_INTERFACE_DEFINED__

/* interface IMMDeviceCollection */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IMMDeviceCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0BD7A1BE-7A1A-44DB-8397-CC5392387B5E")
    IMMDeviceCollection : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCount( 
            /* [out] */ 
            __out  UINT *pcDevices) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ 
            __in  UINT nDevice,
            /* [out] */ 
            __out  IMMDevice **ppDevice) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMMDeviceCollectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMMDeviceCollection * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMMDeviceCollection * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMMDeviceCollection * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCount )( 
            IMMDeviceCollection * This,
            /* [out] */ 
            __out  UINT *pcDevices);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IMMDeviceCollection * This,
            /* [in] */ 
            __in  UINT nDevice,
            /* [out] */ 
            __out  IMMDevice **ppDevice);
        
        END_INTERFACE
    } IMMDeviceCollectionVtbl;

    interface IMMDeviceCollection
    {
        CONST_VTBL struct IMMDeviceCollectionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMMDeviceCollection_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMMDeviceCollection_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMMDeviceCollection_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMMDeviceCollection_GetCount(This,pcDevices)	\
    ( (This)->lpVtbl -> GetCount(This,pcDevices) ) 

#define IMMDeviceCollection_Item(This,nDevice,ppDevice)	\
    ( (This)->lpVtbl -> Item(This,nDevice,ppDevice) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMMDeviceCollection_INTERFACE_DEFINED__ */


#ifndef __IMMEndpoint_INTERFACE_DEFINED__
#define __IMMEndpoint_INTERFACE_DEFINED__

/* interface IMMEndpoint */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IMMEndpoint;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1BE09788-6894-4089-8586-9A2A6C265AC5")
    IMMEndpoint : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDataFlow( 
            /* [out] */ 
            __out  EDataFlow *pDataFlow) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMMEndpointVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMMEndpoint * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMMEndpoint * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMMEndpoint * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDataFlow )( 
            IMMEndpoint * This,
            /* [out] */ 
            __out  EDataFlow *pDataFlow);
        
        END_INTERFACE
    } IMMEndpointVtbl;

    interface IMMEndpoint
    {
        CONST_VTBL struct IMMEndpointVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMMEndpoint_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMMEndpoint_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMMEndpoint_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMMEndpoint_GetDataFlow(This,pDataFlow)	\
    ( (This)->lpVtbl -> GetDataFlow(This,pDataFlow) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMMEndpoint_INTERFACE_DEFINED__ */


#ifndef __IMMDeviceEnumerator_INTERFACE_DEFINED__
#define __IMMDeviceEnumerator_INTERFACE_DEFINED__

/* interface IMMDeviceEnumerator */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IMMDeviceEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A95664D2-9614-4F35-A746-DE8DB63617E6")
    IMMDeviceEnumerator : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnumAudioEndpoints( 
            /* [in] */ 
            __in  EDataFlow dataFlow,
            /* [in] */ 
            __in  DWORD dwStateMask,
            /* [out] */ 
            __out  IMMDeviceCollection **ppDevices) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDefaultAudioEndpoint( 
            /* [in] */ 
            __in  EDataFlow dataFlow,
            /* [in] */ 
            __in  ERole role,
            /* [out] */ 
            __out  IMMDevice **ppEndpoint) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetDevice( 
            /*  */ 
            __in  LPCWSTR pwstrId,
            /* [out] */ 
            __out  IMMDevice **ppDevice) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterEndpointNotificationCallback( 
            /* [in] */ 
            __in  IMMNotificationClient *pClient) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnregisterEndpointNotificationCallback( 
            /* [in] */ 
            __in  IMMNotificationClient *pClient) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMMDeviceEnumeratorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMMDeviceEnumerator * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMMDeviceEnumerator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMMDeviceEnumerator * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnumAudioEndpoints )( 
            IMMDeviceEnumerator * This,
            /* [in] */ 
            __in  EDataFlow dataFlow,
            /* [in] */ 
            __in  DWORD dwStateMask,
            /* [out] */ 
            __out  IMMDeviceCollection **ppDevices);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDefaultAudioEndpoint )( 
            IMMDeviceEnumerator * This,
            /* [in] */ 
            __in  EDataFlow dataFlow,
            /* [in] */ 
            __in  ERole role,
            /* [out] */ 
            __out  IMMDevice **ppEndpoint);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetDevice )( 
            IMMDeviceEnumerator * This,
            /*  */ 
            __in  LPCWSTR pwstrId,
            /* [out] */ 
            __out  IMMDevice **ppDevice);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterEndpointNotificationCallback )( 
            IMMDeviceEnumerator * This,
            /* [in] */ 
            __in  IMMNotificationClient *pClient);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterEndpointNotificationCallback )( 
            IMMDeviceEnumerator * This,
            /* [in] */ 
            __in  IMMNotificationClient *pClient);
        
        END_INTERFACE
    } IMMDeviceEnumeratorVtbl;

    interface IMMDeviceEnumerator
    {
        CONST_VTBL struct IMMDeviceEnumeratorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMMDeviceEnumerator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMMDeviceEnumerator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMMDeviceEnumerator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMMDeviceEnumerator_EnumAudioEndpoints(This,dataFlow,dwStateMask,ppDevices)	\
    ( (This)->lpVtbl -> EnumAudioEndpoints(This,dataFlow,dwStateMask,ppDevices) ) 

#define IMMDeviceEnumerator_GetDefaultAudioEndpoint(This,dataFlow,role,ppEndpoint)	\
    ( (This)->lpVtbl -> GetDefaultAudioEndpoint(This,dataFlow,role,ppEndpoint) ) 

#define IMMDeviceEnumerator_GetDevice(This,pwstrId,ppDevice)	\
    ( (This)->lpVtbl -> GetDevice(This,pwstrId,ppDevice) ) 

#define IMMDeviceEnumerator_RegisterEndpointNotificationCallback(This,pClient)	\
    ( (This)->lpVtbl -> RegisterEndpointNotificationCallback(This,pClient) ) 

#define IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(This,pClient)	\
    ( (This)->lpVtbl -> UnregisterEndpointNotificationCallback(This,pClient) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMMDeviceEnumerator_INTERFACE_DEFINED__ */


#ifndef __IMMDeviceActivator_INTERFACE_DEFINED__
#define __IMMDeviceActivator_INTERFACE_DEFINED__

/* interface IMMDeviceActivator */
/* [unique][helpstring][nonextensible][uuid][local][object] */ 


EXTERN_C const IID IID_IMMDeviceActivator;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3B0D0EA4-D0A9-4B0E-935B-09516746FAC0")
    IMMDeviceActivator : public IUnknown
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Activate( 
            /* [in] */ 
            __in  REFIID iid,
            /* [in] */ 
            __in  IMMDevice *pDevice,
            /* [in] */ 
            __in_opt  PROPVARIANT *pActivationParams,
            /* [iid_is][out] */ 
            __out  void **ppInterface) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMMDeviceActivatorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMMDeviceActivator * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMMDeviceActivator * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMMDeviceActivator * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Activate )( 
            IMMDeviceActivator * This,
            /* [in] */ 
            __in  REFIID iid,
            /* [in] */ 
            __in  IMMDevice *pDevice,
            /* [in] */ 
            __in_opt  PROPVARIANT *pActivationParams,
            /* [iid_is][out] */ 
            __out  void **ppInterface);
        
        END_INTERFACE
    } IMMDeviceActivatorVtbl;

    interface IMMDeviceActivator
    {
        CONST_VTBL struct IMMDeviceActivatorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMMDeviceActivator_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IMMDeviceActivator_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IMMDeviceActivator_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IMMDeviceActivator_Activate(This,iid,pDevice,pActivationParams,ppInterface)	\
    ( (This)->lpVtbl -> Activate(This,iid,pDevice,pActivationParams,ppInterface) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IMMDeviceActivator_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_mmdeviceapi_0000_0006 */
/* [local] */ 

typedef /* [public] */ struct __MIDL___MIDL_itf_mmdeviceapi_0000_0006_0001
    {
    LPARAM AddPageParam;
    IMMDevice *pEndpoint;
    IMMDevice *pPnpInterface;
    IMMDevice *pPnpDevnode;
    } 	AudioExtensionParams;



extern RPC_IF_HANDLE __MIDL_itf_mmdeviceapi_0000_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mmdeviceapi_0000_0006_v0_0_s_ifspec;


#ifndef __MMDeviceAPILib_LIBRARY_DEFINED__
#define __MMDeviceAPILib_LIBRARY_DEFINED__

/* library MMDeviceAPILib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MMDeviceAPILib;

EXTERN_C const CLSID CLSID_MMDeviceEnumerator;

#ifdef __cplusplus

class DECLSPEC_UUID("BCDE0395-E52F-467C-8E3D-C4579291692E")
MMDeviceEnumerator;
#endif
#endif /* __MMDeviceAPILib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */
