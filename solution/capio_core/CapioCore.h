#pragma once
#include "export.h"
#include <exception>
#include <cassert>


#ifdef _MSC_VER
#include "Windows.h"
#endif // _MSC_VER
namespace cpio
{

#ifdef _MSC_VER
	typedef HWND WindowHandle;
#endif // _MSC_VER

	//Exception types
	class CapioException : public std::exception
	{
	public:
		CapioException() : std::exception() {}
		CapioException(const char* message) : std::exception(message) { }
	};
	class NotImplementedException : public CapioException
	{
	public:
		NotImplementedException() : CapioException("Not implemented") {}
		NotImplementedException(const char* message) : CapioException(message) {}
	};

	class AssertionException : public CapioException
	{
	public:
		AssertionException() : CapioException() {}
		AssertionException(const char* message) : CapioException(message) {}
	};

#define STRINGIZE(A) #A

#define ASSERTION_MESSAGE(expression, file, line) STRINGIZE(expression \r\nFile: file \r\nLine:  line)

#ifdef  _DEBUG

#define ThrowException(type, ...) \
{\
_wassert(_CRT_WIDE("Exception of type #type is thrown"), _CRT_WIDE(__FILE__), (unsigned)(__LINE__));\
throw type(__VA_ARGS__);\
}\


#define CAPI_Assert(expression) \
if(!!!(expression))\
{\
	_wassert(_CRT_WIDE(#expression), _CRT_WIDE(__FILE__), (unsigned)(__LINE__)); \
    throw cpio::AssertionException(ASSERTION_MESSAGE(expression, __FILE__, __LINE__)); \
}\

#else

#define ThrowException(type, ...) throw type(__VA_ARGS__)

#define CAPI_Assert(expression) \
{\
	if(!!!(expression))\
	{\
		throw cpio::AssertionException(ASSERTION_MESSAGE(expression, __FILE__, __LINE__)); \
	}\
}\

#endif //  _DEBUG

	constexpr size_t AlignTo(size_t size, size_t alignment)
	{
		if (size % alignment != 0)
		{
			return (size / alignment + 1) * alignment;
		}
		return size;
	}
}