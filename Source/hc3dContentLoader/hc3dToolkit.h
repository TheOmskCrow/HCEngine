#pragma once

#ifdef HC3D_EXPORTS
#define HC3D_API __declspec(dllexport) 
#else
#define HC3D_API __declspec(dllimport) 
#endif

#include <cstdlib>
#include <glew.h>
#include <freeglut.h>
#include <gl\GL.h>
#include <gl\GLU.h>
#include <glext.h>
#include <comutil.h>

namespace hc3d {
	class hc3dToolkit
	{
	public:
		// Gets avaliable CPU number
		HC3D_API static DWORD_PTR GetNumCPUs() {
			SYSTEM_INFO m_si = { 0, };
			GetSystemInfo(&m_si);
			return (DWORD_PTR)m_si.dwNumberOfProcessors;
		}

		// Checks file existence
		HC3D_API static bool FileExists(bstr_t &file)
		{
			WIN32_FIND_DATA FindFileData;
			HANDLE handle = FindFirstFile(file.GetBSTR(), &FindFileData);
			int found = handle != INVALID_HANDLE_VALUE;
			if (found) FindClose(handle);
			return found != 0;
		}
	};
}

