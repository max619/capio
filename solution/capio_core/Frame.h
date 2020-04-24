#pragma once

#include <inttypes.h>
#include "CapioCore.h"
#include "SmartPointers.h"

namespace cpio
{
	enum class DataFormat : uint16_t
	{
		None = 0,
		UInt8 = (1 << 1),
		UInt16 = (1 << 2),
		Float32 = (1 << 3),
		Float16 = (1 << 4),
		Float64 = (1 << 5),
	};

	constexpr uint16_t MakePixelSequence(int alphafirst, int seqtype, int channels)
	{
		return (uint16_t)((uint16_t)(alphafirst << 4) | (uint16_t)(seqtype << 3) | (uint16_t)((channels)));
	}

	enum class PixelSequence : uint16_t
	{
		None = 0,
		RGB = MakePixelSequence(0, 0, 3),
		BGR = MakePixelSequence(0, 1, 3),
		RGBA = MakePixelSequence(1, 0, 4),
		BGRA = MakePixelSequence(1, 1, 4),
		ARGB = MakePixelSequence(2, 0, 4),
		ABGR = MakePixelSequence(2, 1, 4),
		GRAY = MakePixelSequence(0, 0, 1),
		RG = MakePixelSequence(0, 0, 2),
		BG = MakePixelSequence(0, 1, 2),
	};


	constexpr uint16_t MakePixelFormat(PixelSequence seq, DataFormat fmt)
	{
		return ((uint16_t)(((uint16_t)(seq) << 7) | (uint16_t)(fmt)));
	}

	constexpr uint16_t MakePixelFormat(int alphafirst, int seqtype, int channels, DataFormat fmt)
	{
		return ((uint16_t)((MakePixelSequence(alphafirst, seqtype, channels) << 7) | (uint16_t)(fmt)));
	}

	enum class PixelFormat : uint16_t
	{
		None = 0,
		RGB24 = MakePixelFormat(PixelSequence::RGB, DataFormat::UInt8),
		BGR24 = MakePixelFormat(PixelSequence::BGR, DataFormat::UInt8),
		RGBA32 = MakePixelFormat(PixelSequence::RGBA, DataFormat::UInt8),
		BGRA32 = MakePixelFormat(PixelSequence::BGRA, DataFormat::UInt8),
		ARGB32 = MakePixelFormat(PixelSequence::ARGB, DataFormat::UInt8),
		ABGR32 = MakePixelFormat(PixelSequence::ABGR, DataFormat::UInt8),
		GRAY8 = MakePixelFormat(PixelSequence::GRAY, DataFormat::UInt8),
		GRAY16 = MakePixelFormat(PixelSequence::GRAY, DataFormat::UInt16),
		RG8 = MakePixelFormat(PixelSequence::RG, DataFormat::UInt8),
		BG8 = MakePixelFormat(PixelSequence::BG, DataFormat::UInt8),
	};

	constexpr DataFormat GetDataFormat(PixelFormat fmt)
	{
		return (DataFormat)((uint16_t)(fmt) & 0b00111111);
	}

	constexpr uint8_t GetChannelsCount(PixelFormat fmt)
	{
		return (uint8_t)(((uint16_t)(fmt) >> 6) & 0b00000111);
	}

	constexpr PixelSequence GetPixelSequence(PixelFormat fmt)
	{
		return (PixelSequence)(((uint16_t)(fmt) >> 6));
	}

	constexpr size_t GetDataFormatSize(DataFormat fmt)
	{
		switch (fmt)
		{
		case cpio::DataFormat::UInt8:
			return sizeof(uint8_t);
		case cpio::DataFormat::UInt16:
			return sizeof(uint16_t);
		case cpio::DataFormat::Float32:
			return sizeof(float);
		case cpio::DataFormat::Float16:
			return sizeof(uint16_t);
		case cpio::DataFormat::Float64:
			return sizeof(double);
		default:
			return 0;
			break;
		}
	}

	constexpr size_t GetPixelSize(PixelFormat fmt)
	{
		return static_cast<size_t>(GetChannelsCount(fmt))* GetDataFormatSize(GetDataFormat(fmt));
	}

	constexpr size_t GetBpp(PixelFormat fmt)
	{
		return GetPixelSize(fmt) * 8;
	}

	template<typename T>
	struct Size
	{
		T width, height;

		Size() : width(0), height(0) {}
		Size(T w, T h) : width(w), height(h) {}


		bool operator==(const Size<T>& rhs)
		{
			return width == rhs.width && height == rhs.height;
		}

		bool operator!=(const Size<T>& rhs)
		{
			return width != rhs.width || height != rhs.height;
		}
	};

	template<typename T>
	struct Point
	{
		T x, y;

		Point() : x(0), y(0) {}
		Point(T x, T y) : x(x), y(y) {}
	};

	template<typename T>
	struct Rectangle
	{
		Point<T> topLeft;
		Size<T> size;

		Rectangle()
		{
			topLeft = {};
			size = {};
		}

		Rectangle(Point<T> top, Size<T> size)
		{
			topLeft = top;
			size = size;
		}


		Rectangle(T x, T y, T w, T h)
		{
			topLeft = { x, y };
			size = { w, h };
		}
	};

	struct Color
	{
		float r, g, b, a;

		Color() :
			r(0), g(0), b(0), a(1) {}
		Color(float r) :
			r(r), g(r), b(r), a(1) {}
		Color(float r, float g, float b) :
			r(r), g(g), b(b), a(1) {}
		Color(float r, float g, float b, float a) :
			r(r), g(g), b(b), a(a) {}
	};

#define CAPI_Assert_Image_ROI(img, roi) CAPI_Assert((img).GetImageBuffer()->GetSize().width >= (roi).topLeft.x + (roi).size.width && (img).GetImageBuffer()->GetSize().height >= (roi).topLeft.y + (roi).size.width)
#define CAPI_Assert_ROI(a, b) CAPI_Assert((a).size.width == (b).size.width && (a).size.height == (b).size.height)

	class CAPIO_EXPORT ImageBuffer
	{
	public:
		virtual ~ImageBuffer();
		ImageBuffer(const ImageBuffer& other);
		virtual void Release();
		virtual void Resize(Size<uint32_t> size, PixelFormat fmt);
		virtual void Allocate(Size<uint32_t> size, PixelFormat fmt);
		virtual void* GetData();
		virtual const void* GetData() const;

		cpio::Size<uint32_t> GetSize() const;
		size_t GetStride()  const;
		cpio::PixelFormat GetPixelFormat()  const;

	protected:
		ImageBuffer();
		cpio::Size<uint32_t> imageSize = { 0,0 };
		PixelFormat fmt;
		size_t stride;
	};


	class CAPIO_EXPORT SystemMemoryImageBuffer : public ImageBuffer
	{
	public:
		SystemMemoryImageBuffer();
		SystemMemoryImageBuffer(SystemMemoryImageBuffer& other);
		~SystemMemoryImageBuffer();
		void Release() override;
		void Resize(Size<uint32_t> size, PixelFormat fmt) override;
		void Allocate(Size<uint32_t> size, PixelFormat fmt) override;
		void* GetData() override;
		const void* GetData() const override;

	private:
		void* data = nullptr;
	};

	class CAPIO_EXPORT Image
	{
	public:
		Image(cpio::UniquePtr<cpio::ImageBuffer>&& buffer);
		Image(cpio::Size<uint32_t> size, cpio::PixelFormat fmt);
		Image(const Image& rhs);
		~Image();

		void SetRoi(cpio::Rectangle<uint32_t> roi);
		void ResetRoi();
		void CopyTo(cpio::Image& other) const;
		cpio::Rectangle<uint32_t> GetRoi() const;
		const cpio::UniquePtr<cpio::ImageBuffer>& GetImageBuffer() const;
		cpio::UniquePtr<cpio::ImageBuffer>& GetImageBuffer();

	private:
		cpio::Rectangle<uint32_t> roi = {};
		bool roiSet = false;
		cpio::UniquePtr<cpio::ImageBuffer> buffer;

		static cpio::UniquePtr<cpio::ImageBuffer> AllocateDefault(cpio::Size<uint32_t> size, cpio::PixelFormat fmt);
	};

}