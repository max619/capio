#include "pch.h"
#include "CppUnitTest.h"
#include "SmartPointers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace cpio
{
	class TestStruct
	{
	public:
		int a, b, c;

		TestStruct() : a(0), b(0), c(0) {}
		TestStruct(int a) : a(a), b(0), c(0) {}
		TestStruct(int a, int b) : a(a), b(b), c(0) {}
		TestStruct(int a, int b, int c) : a(a), b(b), c(c) {}

		virtual int Sum() const
		{
			return a + b + c;
		}
	};

	class TestStructDerived : public TestStruct
	{
	public:


		TestStructDerived() : TestStruct(){}
		TestStructDerived(int a) : TestStruct(a) {}
		TestStructDerived(int a, int b) : TestStruct(a, b) {}
		TestStructDerived(int a, int b, int c) : TestStruct(a, b, c) {}

		int Sum() const override
		{
			return TestStruct::Sum() * 10;
		}
	};

	TEST_CLASS(UniquePtrTest)
	{
	public:

		TEST_METHOD(EmptyTest)
		{
			UniquePtr<TestStruct> p;
			Assert::IsFalse(p);
			Assert::IsTrue(p.IsEmpty());
		}

		TEST_METHOD(MakeUnique)
		{
			auto p1 = cpio::MakeUnique<TestStruct>();

			Assert::IsTrue(p1);
			Assert::IsFalse(p1.IsEmpty());
			Assert::IsTrue(p1.Get().a == 0 && p1.Get().b == 0 && p1.Get().c == 0);

			auto p2 = cpio::MakeUnique<TestStruct>(1);

			Assert::IsTrue(p2);
			Assert::IsFalse(p2.IsEmpty());
			Assert::IsTrue(p2.Get().a == 1 && p2.Get().b == 0 && p2.Get().c == 0);

			auto p3 = cpio::MakeUnique<TestStruct>(1, 2);

			Assert::IsTrue(p3);
			Assert::IsFalse(p3.IsEmpty());
			Assert::IsTrue(p3.Get().a == 1 && p3.Get().b == 2 && p3.Get().c == 0);

			auto p4 = cpio::MakeUnique<TestStruct>(1, 2, 3);

			Assert::IsTrue(p4);
			Assert::IsFalse(p4.IsEmpty());
			Assert::IsTrue(p4.Get().a == 1 && p4.Get().b == 2 && p4.Get().c == 3);
		}

		TEST_METHOD(MoveTest)
		{
			auto a = new TestStruct(1);
			auto b = new TestStruct(2);

			UniquePtr<TestStruct> p1(a);
			UniquePtr<TestStruct> p2(b);

			p2 = std::move(p1);
			Assert::IsTrue(p2);
			Assert::IsFalse(p1);
		}

		TEST_METHOD(MoveCtorTest)
		{
			auto p = cpio::MakeUnique<TestStruct>(1, 2);
			UniquePtr<TestStruct> p1(std::move(p));
			Assert::IsTrue(p1);
		}

		TEST_METHOD(ArrowOperatorTest)
		{
			auto p = cpio::MakeUnique<TestStruct>(1, 2);
			Assert::IsTrue(p->Sum() == 3);
		}

		TEST_METHOD(ComparePointersTest)
		{
			UniquePtr<TestStruct> p1;
			UniquePtr<TestStruct> p2;

			Assert::IsTrue(p1 == p2);
			Assert::IsTrue(p1 == nullptr);
			Assert::IsTrue(p2 == nullptr);

			p1 = cpio::MakeUnique<TestStruct>(1, 2);

			Assert::IsFalse(p1 == p2);
			Assert::IsFalse(p1 == nullptr);
		}


		cpio::UniquePtr<TestStruct> ConversionTest()
		{
			auto p = cpio::MakeUnique<TestStructDerived>(1, 2, 3);
			p->Sum();

			return p;
		}

		TEST_METHOD(TestParentesis)
		{
			auto baseP = cpio::MakeUnique<TestStruct>(1, 2, 3);
			auto derivedP = cpio::MakeUnique<TestStructDerived>(1, 2, 3);

			Assert::IsTrue(baseP->Sum() != derivedP->Sum());

			auto res = derivedP->Sum();

			baseP = std::move(derivedP);
			
			Assert::IsTrue(baseP->Sum() == res);
		}


		TEST_METHOD(ConversionParentesis)
		{
			auto baseP = cpio::MakeUnique<TestStruct>(1, 2, 3);
			auto derivedP = ConversionTest();

			Assert::IsTrue(baseP->Sum() != derivedP->Sum());

			auto res = derivedP->Sum();

			baseP = ConversionTest();
			baseP = std::move(derivedP);

			Assert::IsTrue(baseP->Sum() == res);
		}
	};
}