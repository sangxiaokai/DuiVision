// Windows DWM wrapper
#ifndef __DUIWINDWMWRAPPER_H__
#define __DUIWINDWMWRAPPER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct DUI_DWM_MARGINS
{
	int cxLeftWidth;      // width of left border that retains its size
	int cxRightWidth;     // width of right border that retains its size
	int cyTopHeight;      // height of top border that retains its size
	int cyBottomHeight;   // height of bottom border that retains its size
};

#define TYPEDEF_WRAPPER_POINTER(val, type, proc)\
	const int duiWrapper##proc = val;\
	const LPCSTR duiWrapperProc##proc = #proc;\
	typedef type (__stdcall* PFN##proc)

TYPEDEF_WRAPPER_POINTER(0, HRESULT, DwmDefWindowProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
TYPEDEF_WRAPPER_POINTER(1, HRESULT, DwmIsCompositionEnabled)( BOOL*);
TYPEDEF_WRAPPER_POINTER(2, HRESULT, DwmExtendFrameIntoClientArea)(HWND, const DUI_DWM_MARGINS*);

// The CDuiWinDwmWrapper class wraps the dwp api.
class CDuiWinDwmWrapper
{
private:
	// Shared data of wrappers
	class CSharedData
	{
	public:

		CSharedData();

	public:
		LPVOID m_ptrWrappers[3];   // Wrapper pointer
		HMODULE m_hDwmDll;        // Handle to the theme dll.
	};

public:

	CDuiWinDwmWrapper();
	~CDuiWinDwmWrapper();

public:

	HRESULT DefWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
	BOOL IsCompositionEnabled();
	HRESULT ExtendFrameIntoClientArea(HWND hWnd, int cxLeftWidth, int cyTopHeight, int cxRightWidth, int cyBottomHeight);

public:
	void LogicalToPhysicalPoint(HWND hWnd, LPPOINT lpPoint);
	void SetProcessDPIAware();

private:
	CSharedData& GetSharedData() const;
};

#endif // __DUIWINDWMWRAPPER_H__