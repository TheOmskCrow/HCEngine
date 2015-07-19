#pragma once
#include "dllstorage.h"
#include "Storage\Material.h"
#include "Storage\ShaderLib.h"
#include "Renderer\Renderer.h"

// This class is exported from the hc3dStorage.dll
class HC3DSTORAGE_API Chc3dStorage {
public:
	Chc3dStorage(void);
	// TODO: add your methods here.
};

extern HC3DSTORAGE_API int nhc3dStorage;

HC3DSTORAGE_API int fnhc3dStorage(void);
