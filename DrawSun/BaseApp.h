// Windows Header Files:
#include <windows.h>

// D2D
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwrite")
#pragma comment(lib, "Windowscodecs")

template<class Interface>
inline void SafeRelease(
	Interface **ppInterfaceToRelease
	)
{
	if (*ppInterfaceToRelease != NULL)
	{
		(*ppInterfaceToRelease)->Release();

		(*ppInterfaceToRelease) = NULL;
	}
}

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

#define FRAME_RATE 33.3333

class BaseApp
{
public:
	BaseApp();
	~BaseApp();

	// Register the window class and call methods for instantiating drawing resources
	HRESULT Initialize();

	// Process and dispatch messages
	void RunMessageLoop();

private:
	// Initialize device-independent resources.
	HRESULT CreateDeviceIndependentResources();

	// Initialize device-dependent resources.
	HRESULT CreateDeviceResources(HWND);

	// Release device-dependent resource.
	//void DiscardDeviceResources();

	// Draw content.
	HRESULT OnRender();

	// Handle keyboard.
	HRESULT OnKeyDown(WPARAM);

	// Handle keyboard.
	HRESULT OnChar(WPARAM);

	// Resize the render target.
	void OnResize(
		UINT width,
		UINT height
		);

	// The windows procedure.
	static LRESULT CALLBACK WndProc(
		HWND hWnd,
		UINT message,
		WPARAM wParam,
		LPARAM lParam
		);


	HWND m_hwnd;
	ID2D1Factory * m_pD2DFactory = NULL;
	ID2D1HwndRenderTarget* m_pD2DHRT = NULL;
	IDWriteFactory* m_pDWriteFactory;
	
	IDWriteTextFormat* m_pDWriteTextFormat;

	ID2D1SolidColorBrush* m_pD2DSolidBrush01;
	ID2D1SolidColorBrush* m_pD2DSolidBrush02;
	ID2D1SolidColorBrush* m_pD2DSolidBrush03;
	ID2D1SolidColorBrush* m_pD2DSolidBrush04;
	ID2D1SolidColorBrush* m_pD2DSolidBrush05;
	ID2D1LinearGradientBrush* m_pLinearGradientBrush01;

	IWICImagingFactory* m_pWICImagingFactory = NULL;

	IWICBitmapDecoder* pWICBitmapDecoder = NULL;
	IWICFormatConverter* pWICFormatConverter = NULL;
	IWICBitmapFrameDecode* pWICBitmapFrameDecode = NULL;

	ID2D1Bitmap* m_pD2DBitMap01 = NULL;
	ID2D1Bitmap* m_pD2DBitMap02 = NULL;

	DWORD dwCurrentTime;
	DWORD dwLastUpdateTime;
	DWORD dwElapsedTime;

};
