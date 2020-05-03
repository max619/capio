#pragma once
#include "CapioCore.h"
#include "Frame.h"

namespace cpio
{
	class VideoSource
	{
	public:
		virtual ~VideoSource();
		virtual cpio::UniquePtr<cpio::Image> GrabFrame();
		virtual void SetRoi(cpio::Rectangle<uint32_t> roi);
		virtual cpio::Size<uint32_t> GetSize();

	protected:
		VideoSource();
	};
}