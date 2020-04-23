#pragma once
#include "CapioCore.h"

namespace cpio
{
	class VideoSource
	{
	public:
		VideoSource() { throw cpio::NotImplementedException(); }
		virtual ~VideoSource() { throw cpio::NotImplementedException(); };
	};
}