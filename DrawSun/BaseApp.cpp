#include <windows.h>
#include <wchar.h>
#include <math.h>
#include "BaseApp.h"
BaseApp::BaseApp() : m_pD2DFactory(NULL), m_pD2DHRT(NULL), 
m_pD2DSolidBrush01(NULL), m_pD2DSolidBrush02(NULL), m_pD2DSolidBrush03(NULL), m_pD2DSolidBrush04(NULL), m_pD2DSolidBrush05(NULL), 
m_pLinearGradientBrush01(NULL),
m_pDWriteFactory(NULL), m_pDWriteTextFormat(NULL),
m_pWICImagingFactory(NULL), m_pD2DBitMap01(NULL), m_pD2DBitMap02(NULL)
{
}

BaseApp::~BaseApp()
{
	SafeRelease(&m_pD2DBitMap01);
	SafeRelease(&m_pD2DBitMap02);
	SafeRelease(&m_pWICImagingFactory);
	SafeRelease(&m_pDWriteTextFormat);
	SafeRelease(&m_pDWriteFactory);
	SafeRelease(&m_pLinearGradientBrush01);
	SafeRelease(&m_pD2DSolidBrush05);
	SafeRelease(&m_pD2DSolidBrush04);
	SafeRelease(&m_pD2DSolidBrush03);
	SafeRelease(&m_pD2DSolidBrush02);
	SafeRelease(&m_pD2DSolidBrush01);
	SafeRelease(&m_pD2DFactory);
	SafeRelease(&m_pD2DHRT);
}


void BaseApp::RunMessageLoop()
{
	MSG msg;

	while (TRUE) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Handle your timer related here
		dwCurrentTime = GetTickCount();
		dwElapsedTime = dwCurrentTime - dwLastUpdateTime;
		if (dwElapsedTime > FRAME_RATE) {
			dwLastUpdateTime = dwCurrentTime;
			// Your other frame related variable here.
			// i.e. frameNumber++
			// reset it back if it's over the limit of the frames
			InvalidateRect(m_hwnd, NULL, TRUE);
		}
		if (msg.message == WM_QUIT) break;
	}

}

HRESULT BaseApp::CreateDeviceIndependentResources()
{
	HRESULT hr = S_OK;

	// Create a Direct2D factory.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	// Create a DirectWrite factory.
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
	// Create WIC Imaging factory.
	hr = CoCreateInstance(CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWICImagingFactory),
		(void**)(&m_pWICImagingFactory)
		);

	return hr;
}

HRESULT BaseApp::CreateDeviceResources(HWND hWnd)
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hWnd, &rc);

	hr = m_pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
		hWnd,
		D2D1::SizeU(
		rc.right - rc.left,
		rc.bottom - rc.top)
		),
		&m_pD2DHRT
		);

	hr = m_pD2DHRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White),
		&m_pD2DSolidBrush01);

	hr = m_pD2DHRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::White, 0.3f),
		&m_pD2DSolidBrush02);

	hr = m_pD2DHRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red),
		&m_pD2DSolidBrush03);

	hr = m_pD2DHRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&m_pD2DSolidBrush04);

	hr = m_pD2DHRT->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Blue),
		&m_pD2DSolidBrush05);

	// Create an array of gradient stops to put in the gradient stop
	// collection that will be used in the gradient brush.
	ID2D1GradientStopCollection *pGradientStops = NULL;

	D2D1_GRADIENT_STOP gradientStops[2];
	gradientStops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow, 1);
	gradientStops[0].position = 0.0f;
	gradientStops[1].color = D2D1::ColorF(D2D1::ColorF::ForestGreen, 1);
	gradientStops[1].position = 1.0f;
	// Create the ID2D1GradientStopCollection from a previously
	// declared array of D2D1_GRADIENT_STOP structs.
	hr = m_pD2DHRT->CreateGradientStopCollection(
		gradientStops,
		2,
		D2D1_GAMMA_2_2,
		D2D1_EXTEND_MODE_CLAMP,
		&pGradientStops
		);

	if (SUCCEEDED(hr))
	{
		hr = m_pD2DHRT->CreateLinearGradientBrush(
			D2D1::LinearGradientBrushProperties(
			D2D1::Point2F(0, 0),
			D2D1::Point2F(150, 150)),
			pGradientStops,
			&m_pLinearGradientBrush01
			);
	}

	hr = m_pDWriteFactory->CreateTextFormat(
		L"Bookman Old Style",
		NULL,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		24.0f,
		L"en-us",
		&m_pDWriteTextFormat
		);
	// Center align (horizontally) the text.
	if (SUCCEEDED(hr))
	{
		hr = m_pDWriteTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		hr = m_pDWriteTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	IWICBitmapDecoder* pWICBitmapDecoder = NULL;
	IWICFormatConverter* pWICFormatConverter = NULL;
	IWICBitmapFrameDecode* pWICBitmapFrameDecode = NULL;

	//// Bitmap 1

	//hr = m_pWICImagingFactory->CreateDecoderFromFilename(
	//	L"E:\\images\\fireheart_actorsprites1.png",
	//	NULL,
	//	GENERIC_READ,
	//	WICDecodeMetadataCacheOnLoad,
	//	&pWICBitmapDecoder);

	//hr = m_pWICImagingFactory->CreateFormatConverter(&pWICFormatConverter);

	//hr = pWICBitmapDecoder->GetFrame(0, &pWICBitmapFrameDecode);

	//hr = pWICFormatConverter->Initialize(
	//	pWICBitmapFrameDecode,
	//	GUID_WICPixelFormat32bppPBGRA,
	//	WICBitmapDitherTypeNone,
	//	NULL, 0.f,
	//	WICBitmapPaletteTypeMedianCut
	//	);

	//hr = m_pD2DHRT->CreateBitmapFromWicBitmap(
	//	pWICFormatConverter,
	//	NULL,
	//	&m_pD2DBitMap01
	//	);

	//// Bitmap 2
	//hr = m_pWICImagingFactory->CreateDecoderFromFilename(
	//	L"E:\\images\\TileB.png",
	//	NULL,
	//	GENERIC_READ,
	//	WICDecodeMetadataCacheOnLoad,
	//	&pWICBitmapDecoder);

	//hr = m_pWICImagingFactory->CreateFormatConverter(&pWICFormatConverter);

	//hr = pWICBitmapDecoder->GetFrame(0, &pWICBitmapFrameDecode);

	//hr = pWICFormatConverter->Initialize(
	//	pWICBitmapFrameDecode,
	//	GUID_WICPixelFormat32bppPBGRA,
	//	WICBitmapDitherTypeNone,
	//	NULL, 0.f,
	//	WICBitmapPaletteTypeMedianCut
	//	);

	//hr = m_pD2DHRT->CreateBitmapFromWicBitmap(
	//	pWICFormatConverter,
	//	NULL,
	//	&m_pD2DBitMap02
	//	);
	
	return hr;
}

HRESULT BaseApp::Initialize()
{
	HRESULT hr;

	//get the dpi information
	HDC screen = GetDC(0);
	ReleaseDC(0, screen);

	// Initialize device-indpendent resources, such
	// as the Direct2D factory.
	hr = CreateDeviceIndependentResources();

	if (SUCCEEDED(hr))
	{

		// Register the window class.
		WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = BaseApp::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = sizeof(LONG_PTR);
		wcex.hInstance = HINST_THISCOMPONENT;
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.hCursor = LoadCursor(NULL, IDI_APPLICATION);
		wcex.lpszClassName = "Base App Window Class";

		RegisterClassEx(&wcex);


		// Create the window.
		m_hwnd = CreateWindow(
			"Base App Window Class",
			"DubboIT - Draw Sun",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			0,
			400,
			400,
			NULL,
			NULL,
			HINST_THISCOMPONENT,
			this
			);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			hr = CreateDeviceResources(m_hwnd);
			ShowWindow(m_hwnd, SW_SHOWNORMAL);
			UpdateWindow(m_hwnd);
		}

	}

	return hr;
}

int WINAPI WinMain(
	HINSTANCE /* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPSTR /* lpCmdLine */,
	int /* nCmdShow */
	)
{
	// Use HeapSetInformation to specify that the process should
	// terminate if the heap manager detects an error in any heap used
	// by the process.
	// The return value is ignored, because we want to continue running in the
	// unlikely event that HeapSetInformation fails.
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			BaseApp app;

			if (SUCCEEDED(app.Initialize()))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}

	return 0;
}

LRESULT CALLBACK BaseApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		BaseApp *pBaseApp = (BaseApp *)pcs->lpCreateParams;

		::SetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA,
			PtrToUlong(pBaseApp)
			);

		result = 1;
	}
	else
	{
		BaseApp *pBaseApp = reinterpret_cast<BaseApp *>(static_cast<LONG_PTR>(
			::GetWindowLongPtrW(
			hwnd,
			GWLP_USERDATA
			)));

		bool wasHandled = false;
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);
		if (pBaseApp)
		{
			switch (message)
			{
			case WM_KEYDOWN:
				pBaseApp->OnKeyDown(wParam);
				InvalidateRect(hwnd, NULL, FALSE);
				break;
			case WM_CHAR:
				pBaseApp->OnChar(wParam);
				InvalidateRect(hwnd, NULL, FALSE);
				break;
			case WM_SIZE:
				pBaseApp->OnResize(width, height);
				result = 0;
				wasHandled = true;
				break;
			case WM_DISPLAYCHANGE:
				InvalidateRect(hwnd, NULL, FALSE);
				result = 0;
				wasHandled = true;
				break;
			case WM_PAINT:
				pBaseApp->OnRender();
				ValidateRect(hwnd, NULL);
				result = 0;
				wasHandled = true;
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				result = 1;
				wasHandled = true;
				break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}

HRESULT BaseApp::OnRender()
{
	HRESULT hr = S_OK;

	float nCentreX{ 150.f };
	float nCentreY{ 150.f };
	float nRadius{ 50.f };
	float nX1{ 0.f };
	float nY1{ 0.f };
	float nX2{ 0.f };
	float nY2{ 0.f };
	float nGap{ 5.f };
	float nRayLength{ 10.f };
	float nAngle{ 0 };

	m_pD2DHRT->BeginDraw();

	m_pD2DHRT->SetTransform(D2D1::Matrix3x2F::Identity());
	m_pD2DHRT->Clear(D2D1::ColorF(D2D1::ColorF::CadetBlue));

	m_pD2DHRT->DrawEllipse(D2D1::Ellipse(
				D2D1::Point2F(nCentreX, nCentreY), 
				nRadius, 
				nRadius), 
				m_pD2DSolidBrush01, 2.f);

	for (int i = 0; i != 360; i += 30) {
		nAngle = i;
		nX1 = (nRadius + nGap) * cos(nAngle * 3.1416 / 180) + nCentreX;
		nY1 = (nRadius + nGap) * sin(nAngle * 3.1416 / 180) + nCentreY;
		nX2 = (nRadius + nGap + nRayLength)  * cos(nAngle * 3.1416 / 180) + nCentreY;
		nY2 = (nRadius + nGap + nRayLength)  * sin(nAngle * 3.1416 / 180) + nCentreY;
		m_pD2DHRT->DrawLine(
			D2D1::Point2F(nX1, nY1),
			D2D1::Point2F(nX2, nY2),
			m_pD2DSolidBrush01, 2.f);
	}


	m_pD2DHRT->EndDraw();

	return hr;
}

void BaseApp::OnResize(UINT width, UINT height)
{
	if (m_pD2DHRT)
	{
		// Note: This method can fail, but it's okay to ignore the
		// error here, because the error will be returned again
		// the next time EndDraw is called.
		m_pD2DHRT->Resize(D2D1::SizeU(width, height));
	}
}

HRESULT BaseApp::OnKeyDown(WPARAM wparam) {
	HRESULT hr = S_OK;
	switch (wparam) {
	case VK_LEFT:
		break;
	case VK_RIGHT:
		break;
	case VK_UP:
		break;
	case VK_DOWN:
		break;
	}
	return hr;
}

HRESULT BaseApp::OnChar(WPARAM wparam) {
	HRESULT hr = S_OK;
	switch (wparam) {
	case '[':
		break;
	case ']':
		break;
	}
	return hr;
}