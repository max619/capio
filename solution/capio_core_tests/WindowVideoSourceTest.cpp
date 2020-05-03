#include "pch.h"
#include "CppUnitTest.h"
#include "WindowVideoSource.h"
#include "MemLeakTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace cpio
{
	TEST_CLASS(WindowVideoSourceTest)
	{
	public:
		TEST_METHOD(CreateTest)
		{
			CHECK_MEM_LEAK();
			WindowVideoSource source(nullptr);
		};

		TEST_METHOD(GrabTest)
		{
			CHECK_MEM_LEAK();
			WindowVideoSource source(nullptr);
			auto frame = source.GrabFrame();
		};
	};
}