#include "WindowVideoSource.h"

struct  cpio::WindowVideoSourceImpl
{
	cpio::PixelFormat pixFmt;
	bool isRoiSet;
	cpio::Rectangle<uint32_t> roi;
#ifdef _MSC_VER
	HWND windowHandle;
	HDC source;
	HDC hdcMem;
	HBITMAP bitmap;
	void* bitmapBufferLocation;
	RECT rcClient;
	BITMAPINFO bitmapInfo;
#endif
};

int GetWinPixFmt(cpio::PixelFormat fmt)
{
	switch (fmt)
	{
	case cpio::PixelFormat::RGB24:
		return BI_RGB;
	default:
		ThrowException(cpio::CapioException, "Unsupported pixel format");
		break;
	}
}

void ReallocBuffer(cpio::WindowVideoSourceImpl* impl)
{
#ifdef _MSC_VER
	if (impl->bitmap)
	{
		DeleteObject(impl->bitmap);
		impl->bitmap = nullptr;
		impl->bitmapBufferLocation = nullptr;
	}

	CAPI_Assert(impl->hdcMem);

	memset(&impl->bitmapInfo, 0, sizeof(impl->bitmapInfo));

	LONG width = impl->isRoiSet ? impl->roi.size.width : (impl->rcClient.right - impl->rcClient.left);
	LONG height = impl->isRoiSet ? impl->roi.size.height : (impl->rcClient.bottom - impl->rcClient.top);

	impl->bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	impl->bitmapInfo.bmiHeader.biWidth = width;
	impl->bitmapInfo.bmiHeader.biHeight = height;
	impl->bitmapInfo.bmiHeader.biPlanes = 1;
	impl->bitmapInfo.bmiHeader.biBitCount = cpio::GetBpp(impl->pixFmt);
	impl->bitmapInfo.bmiHeader.biCompression = GetWinPixFmt(impl->pixFmt);
	impl->bitmapInfo.bmiHeader.biXPelsPerMeter = 9600;
	impl->bitmapInfo.bmiHeader.biYPelsPerMeter = 9600;

	impl->bitmap = CreateDIBSection(impl->hdcMem, &impl->bitmapInfo, DIB_RGB_COLORS, &impl->bitmapBufferLocation, nullptr, 0);

	CAPI_Assert(impl->bitmap);
#endif
}

cpio::WindowVideoSource::WindowVideoSource(cpio::WindowHandle handle) : WindowVideoSource(handle, cpio::PixelFormat::RGB24)
{

}

cpio::WindowVideoSource::WindowVideoSource(cpio::WindowHandle handle, cpio::PixelFormat fmt)
{
	cpio::WindowVideoSourceImpl* res = new cpio::WindowVideoSourceImpl();
	memset(res, 0, sizeof(cpio::WindowVideoSourceImpl));

#ifdef _MSC_VER
	res->windowHandle = handle;
	res->pixFmt = fmt;
	res->source = GetDC(handle);
	if (!res->source)
		ThrowException(cpio::CapioException, "Unable to instantiate HDC");

	res->hdcMem = CreateCompatibleDC(res->source);
	if (!res->hdcMem)
	{
		ReleaseDC(handle, res->source);
		ThrowException(cpio::CapioException, "Unable to create compatible DC");
	}

	/*if (SetStretchBltMode(res->source, HALFTONE) != S_OK)
		ThrowException(cpio::CapioException, "Unable to SetStretchBltMode");*/

	if (handle != nullptr)
	{
		if (!GetClientRect(handle, &res->rcClient))
		{
			ReleaseDC(handle, res->source);
			DeleteDC(res->hdcMem);
			ThrowException(cpio::CapioException, "Unable to Get Client Rect");
		}
	}
	else
	{
		res->rcClient.right = static_cast<LONG>(GetSystemMetrics(SM_CXVIRTUALSCREEN));
		res->rcClient.bottom = static_cast<LONG>(GetSystemMetrics(SM_CYVIRTUALSCREEN));
	}
#endif

	impl = res;
}

cpio::WindowVideoSource::~WindowVideoSource()
{
	if (impl != nullptr)
	{
#ifdef _MSC_VER
		if (impl->source)
		{
			ReleaseDC(impl->windowHandle, impl->source);
			impl->source = nullptr;
		}

		if (impl->hdcMem)
		{
			DeleteDC(impl->hdcMem);
			impl->hdcMem = nullptr;
		}

		if (impl->bitmap != nullptr)
		{
			DeleteObject(impl->bitmap);
			impl->bitmap = nullptr;
			impl->bitmapBufferLocation = nullptr;
		}
#endif
		delete impl;
		impl = nullptr;
	}
}

cpio::UniquePtr<cpio::Image> cpio::WindowVideoSource::GrabFrame()
{
	CAPI_Assert(impl);

	if (!impl->bitmap)
		ReallocBuffer(impl);

	int x = impl->isRoiSet ? (impl->rcClient.left + impl->roi.topLeft.x) : impl->rcClient.left;
	int y = impl->isRoiSet ? (impl->rcClient.top + impl->roi.topLeft.y) : impl->rcClient.top;
	int w = impl->isRoiSet ? impl->roi.size.width : (impl->rcClient.right - impl->rcClient.left);
	int h = impl->isRoiSet ? impl->roi.size.height : (impl->rcClient.bottom - impl->rcClient.top);

	CAPI_Assert(impl->bitmapInfo.bmiHeader.biWidth == w && impl->bitmapInfo.bmiHeader.biHeight == h);

	if (!BitBlt(impl->hdcMem, x, y, w, h, impl->hdcMem, 0, 0, SRCCOPY))
	{
		ThrowException(cpio::CapioException, "Failed to BitBlt frame");
	}

	auto img = cpio::MakeUnique<cpio::Image>(cpio::Size<uint32_t>(w, h), impl->pixFmt);
	if (!GetDIBits(impl->hdcMem, impl->bitmap, 0, impl->bitmapInfo.bmiHeader.biHeight,
		img->GetImageBuffer()->GetData(), &impl->bitmapInfo, DIB_RGB_COLORS))
		ThrowException(cpio::CapioException, "Failed to GetDIBits");

	return img;
}

void cpio::WindowVideoSource::SetRoi(cpio::Rectangle<uint32_t> roi)
{
	CAPI_Assert(impl);

	impl->roi = roi;
	impl->isRoiSet = true;

	if (impl->bitmap)
	{
		ReallocBuffer(impl);
	}
}

cpio::Size<uint32_t> cpio::WindowVideoSource::GetSize()
{
	CAPI_Assert(impl);

	return cpio::Size<uint32_t>(impl->rcClient.right - impl->rcClient.left, impl->rcClient.bottom - impl->rcClient.top);
}
