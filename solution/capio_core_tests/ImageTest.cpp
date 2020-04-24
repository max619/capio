#include "pch.h"
#include "CppUnitTest.h"
#include "Frame.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace cpio
{

	TEST_CLASS(ImageTests)
	{
	public:
		TEST_METHOD(AllocateTest)
		{
			Image img(Size<uint32_t>(128, 128), PixelFormat::RGB24);
			auto& buffer = img.GetImageBuffer();
		}

		TEST_METHOD(CopyTest)
		{
			Image img1(Size<uint32_t>(128, 128), PixelFormat::RGB24);
			auto& buffer = img1.GetImageBuffer();
			void* data = buffer->GetData();
			memset(data, 0xffffffff, buffer->GetStride() * buffer->GetSize().height);
			Image img2(Size<uint32_t>(128, 128), PixelFormat::RGB24);

			img1.CopyTo(img2);
		}

		TEST_METHOD(CopyROITest)
		{
			Image img1(Size<uint32_t>(128, 128), PixelFormat::RGB24);
			auto& buffer = img1.GetImageBuffer();
			void* data = buffer->GetData();
			memset(data, 0xffffffff, buffer->GetStride() * buffer->GetSize().height);
			Image img2(Size<uint32_t>(64, 64), PixelFormat::RGB24);
			img1.SetRoi(Rectangle<uint32_t>(0, 0, 64, 64));
			img1.CopyTo(img2);
		}
	};
}