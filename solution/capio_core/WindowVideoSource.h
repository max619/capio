#pragma once

#include "VideoSource.h"

namespace cpio
{
	struct WindowVideoSourceImpl;

	class CAPIO_EXPORT WindowVideoSource : public VideoSource
	{
	public:
		WindowVideoSource(cpio::WindowHandle handle);
		WindowVideoSource(cpio::WindowHandle handle, cpio::PixelFormat fmt);
		~WindowVideoSource() override;
		cpio::UniquePtr<cpio::Image> GrabFrame() override;
		void SetRoi(cpio::Rectangle<uint32_t> roi) override;
		cpio::Size<uint32_t> GetSize() override;

	private:
		WindowVideoSourceImpl* impl;
	};
}
