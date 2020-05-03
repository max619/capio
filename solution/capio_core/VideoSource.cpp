#include "VideoSource.h"

cpio::VideoSource::~VideoSource()
{
}

cpio::UniquePtr<cpio::Image> cpio::VideoSource::GrabFrame()
{
	ThrowException(cpio::NotImplementedException);
}

void cpio::VideoSource::SetRoi(cpio::Rectangle<uint32_t> roi)
{
	ThrowException(cpio::NotImplementedException);
}

cpio::Size<uint32_t> cpio::VideoSource::GetSize()
{
	ThrowException(cpio::NotImplementedException);
}

cpio::VideoSource::VideoSource()
{
}
