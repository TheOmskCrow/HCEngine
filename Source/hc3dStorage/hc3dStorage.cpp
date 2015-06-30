// hc3dStorage.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "hc3dStorage.h"


// This is an example of an exported variable
HC3DSTORAGE_API int nhc3dStorage=0;

// This is an example of an exported function.
HC3DSTORAGE_API int fnhc3dStorage(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see hc3dStorage.h for the class definition
Chc3dStorage::Chc3dStorage()
{
	return;
}
