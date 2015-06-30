// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HC3DSTORAGE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HC3DSTORAGE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef HC3DSTORAGE_EXPORTS
#define HC3DSTORAGE_API __declspec(dllexport)
#else
#define HC3DSTORAGE_API __declspec(dllimport)
#endif

// This class is exported from the hc3dStorage.dll
class HC3DSTORAGE_API Chc3dStorage {
public:
	Chc3dStorage(void);
	// TODO: add your methods here.
};

extern HC3DSTORAGE_API int nhc3dStorage;

HC3DSTORAGE_API int fnhc3dStorage(void);
