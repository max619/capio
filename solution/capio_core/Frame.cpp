#include "Frame.h"

#include <limits>

#ifdef max
#undef max
#endif // max
#ifdef min
#undef min
#endif // max


size_t GetMemoryPageSize()
{
	static size_t pageSize = std::numeric_limits<size_t>::max();

	if (pageSize == std::numeric_limits<size_t>::max())
	{
#ifdef _MSC_VER
		SYSTEM_INFO si;
		GetSystemInfo(&si);
		pageSize = si.dwPageSize;
#endif
	}

	return pageSize;
}

cpio::ImageBuffer::~ImageBuffer()
{
	//Release();
}

cpio::ImageBuffer::ImageBuffer()
{
}

cpio::ImageBuffer::ImageBuffer(const ImageBuffer& buffer)
{
	ThrowException(cpio::NotImplementedException);
}

void cpio::ImageBuffer::Release()
{
	ThrowException(cpio::NotImplementedException);
}

void cpio::ImageBuffer::Resize(Size<uint32_t> size, PixelFormat fmt)
{
	ThrowException(cpio::NotImplementedException);
}

void cpio::ImageBuffer::Allocate(Size<uint32_t> size, PixelFormat fmt)
{
	ThrowException(cpio::NotImplementedException);
}

void* cpio::ImageBuffer::GetData()
{
	ThrowException(cpio::NotImplementedException);
}

const void* cpio::ImageBuffer::GetData() const
{
	ThrowException(cpio::NotImplementedException);
}

cpio::Size<uint32_t> cpio::ImageBuffer::GetSize() const
{
	return this->imageSize;
}

size_t cpio::ImageBuffer::GetStride() const
{
	return this->stride;
}

cpio::PixelFormat cpio::ImageBuffer::GetPixelFormat() const
{
	return fmt;
}

cpio::SystemMemoryImageBuffer::SystemMemoryImageBuffer()
{
}

cpio::SystemMemoryImageBuffer::SystemMemoryImageBuffer(SystemMemoryImageBuffer& other)
{
}

cpio::SystemMemoryImageBuffer::~SystemMemoryImageBuffer()
{
	Release();
}

void cpio::SystemMemoryImageBuffer::Release()
{
	if (data != nullptr)
	{
#ifdef _DEBUG
		_aligned_free_dbg(data);
#else
		_aligned_free(data);
#endif // _DEBUG
		data = nullptr;
	}
}

void cpio::SystemMemoryImageBuffer::Resize(Size<uint32_t> size, PixelFormat fmt)
{
	if (data == nullptr)
	{
		Allocate(size, fmt);
	}
	else
	{
		auto stride = AlignTo(size.width * GetPixelSize(fmt), 4);
		auto totalSize = stride * static_cast<size_t>(size.height);

		data = _aligned_realloc(data, totalSize, GetMemoryPageSize());


		this->imageSize = size;
		this->fmt = fmt;
		this->stride = stride;
	}
}

void cpio::SystemMemoryImageBuffer::Allocate(Size<uint32_t> size, PixelFormat fmt)
{
	if (data != nullptr)
	{
		ThrowException(cpio::CapioException, "Image buffer already allocated. Call resize");
	}

	auto stride = AlignTo(size.width * GetPixelSize(fmt), 4);
	auto totalSize = stride * static_cast<size_t>(size.height);

#ifdef _DEBUG
	data = _aligned_malloc_dbg(totalSize, GetMemoryPageSize(), __FILE__, __LINE__);
#else
	data = _aligned_malloc(totalSize, GetMemoryPageSize());
#endif // _DEBUG

	if (data == nullptr)
		ThrowException(cpio::CapioException, "Out of memory!");

	this->imageSize = size;
	this->fmt = fmt;
	this->stride = stride;
}

void* cpio::SystemMemoryImageBuffer::GetData()
{
	return data;
}

const void* cpio::SystemMemoryImageBuffer::GetData() const
{
	return data;
}

cpio::Image::Image(cpio::UniquePtr<cpio::ImageBuffer>&& buffer)
{
	buffer = std::move(buffer);
}

cpio::Image::Image(Size<uint32_t> size, PixelFormat fmt)
{
	buffer = AllocateDefault(size, fmt);
}

cpio::Image::Image(const Image& rhs)
{
	buffer = AllocateDefault(rhs.GetImageBuffer()->GetSize(), rhs.GetImageBuffer()->GetPixelFormat());
	rhs.CopyTo(*this);
}

cpio::Image::~Image()
{
}

void cpio::Image::SetRoi(Rectangle<uint32_t> roi)
{
	CAPI_Assert_Image_ROI(*this, roi);
	this->roi = roi;
	this->roiSet = true;
}

void cpio::Image::ResetRoi()
{
	this->roiSet = false;
}

void cpio::Image::CopyTo(Image& other) const
{
	const ImageBuffer& thisBuffer = *buffer;
	ImageBuffer& otherBuffer = *(other.buffer);

	auto srcRoi = this->GetRoi();
	auto dstRoi = other.GetRoi();

	CAPI_Assert_ROI(srcRoi, dstRoi);

	CAPI_Assert(thisBuffer.GetPixelFormat() == otherBuffer.GetPixelFormat());

	size_t pixelSize = GetPixelSize(thisBuffer.GetPixelFormat());
	size_t srcStride = thisBuffer.GetStride();
	size_t dstStride = otherBuffer.GetStride();
	size_t copyStride = pixelSize * srcRoi.size.width;

	const uint8_t* srcPtr = reinterpret_cast<const uint8_t*>(thisBuffer.GetData()) + srcStride * srcRoi.topLeft.y + pixelSize * srcRoi.topLeft.x;
	uint8_t* dstPtr = reinterpret_cast<uint8_t*>(otherBuffer.GetData()) + dstStride * dstRoi.topLeft.y + pixelSize * dstRoi.topLeft.x;

	for (uint32_t row = 0; row < srcRoi.size.height; row++, srcPtr += srcStride, dstPtr += dstStride)
	{
		memcpy(dstPtr, srcPtr, copyStride);
	}
}

cpio::Rectangle<uint32_t> cpio::Image::GetRoi() const
{
	CAPI_Assert(buffer);

	if (roiSet)
		return roi;

	auto size = buffer->GetSize();
	return cpio::Rectangle<uint32_t>(0, 0, size.width, size.height);
}

const cpio::UniquePtr<cpio::ImageBuffer>& cpio::Image::GetImageBuffer() const
{
	CAPI_Assert(buffer);
	return buffer;
}

cpio::UniquePtr<cpio::ImageBuffer>& cpio::Image::GetImageBuffer()
{
	CAPI_Assert(buffer);
	return buffer;
}

cpio::UniquePtr<cpio::ImageBuffer> cpio::Image::AllocateDefault(cpio::Size<uint32_t> size, cpio::PixelFormat fmt)
{
	auto buffer = cpio::MakeUnique<cpio::SystemMemoryImageBuffer>();
	buffer->Allocate(size, fmt);
	return buffer;
}
