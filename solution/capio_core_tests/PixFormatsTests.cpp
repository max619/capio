#include "pch.h"
#include "CppUnitTest.h"
#include "Frame.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace cpio
{
	TEST_CLASS(PixFormatsTests)
	{
	public:
		TEST_METHOD(TestBpp)
		{
			Assert::IsTrue(cpio::GetBpp(cpio::PixelFormat::RGB24) == 24);
			Assert::IsTrue(cpio::GetBpp(cpio::PixelFormat::BGR24) == 24);
			Assert::IsTrue(cpio::GetBpp(cpio::PixelFormat::RGBA32) == 32);
			Assert::IsTrue(cpio::GetBpp(cpio::PixelFormat::BGRA32) == 32);
			Assert::IsTrue(cpio::GetBpp(cpio::PixelFormat::ABGR32) == 32);
			Assert::IsTrue(cpio::GetBpp(cpio::PixelFormat::ARGB32) == 32);
			Assert::IsTrue(cpio::GetBpp(cpio::PixelFormat::GRAY8) == 8);
			Assert::IsTrue(cpio::GetBpp(cpio::PixelFormat::GRAY16) == 16);
			Assert::IsTrue(cpio::GetBpp(cpio::PixelFormat::RG8) == 16);
			Assert::IsTrue(cpio::GetBpp(cpio::PixelFormat::BG8) == 16);
		}

		TEST_METHOD(TestChannels)
		{
			Assert::IsTrue(cpio::GetChannelsCount(cpio::PixelFormat::RGB24) == 3);
			Assert::IsTrue(cpio::GetChannelsCount(cpio::PixelFormat::BGR24) == 3);
			Assert::IsTrue(cpio::GetChannelsCount(cpio::PixelFormat::RGBA32) == 4);
			Assert::IsTrue(cpio::GetChannelsCount(cpio::PixelFormat::BGRA32) == 4);
			Assert::IsTrue(cpio::GetChannelsCount(cpio::PixelFormat::ABGR32) == 4);
			Assert::IsTrue(cpio::GetChannelsCount(cpio::PixelFormat::ARGB32) == 4);
			Assert::IsTrue(cpio::GetChannelsCount(cpio::PixelFormat::GRAY8) == 1);
			Assert::IsTrue(cpio::GetChannelsCount(cpio::PixelFormat::GRAY16) == 1);
			Assert::IsTrue(cpio::GetChannelsCount(cpio::PixelFormat::RG8) == 2);
			Assert::IsTrue(cpio::GetChannelsCount(cpio::PixelFormat::BG8) == 2);
		}

	};
}