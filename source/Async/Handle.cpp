//
//  Handle.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Handle.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <cassert>

#include <system_error>

namespace Async
{
	void set_non_blocking(Descriptor descriptor, bool value)
	{
		int flags = ::fcntl(descriptor, F_GETFL, 0);

		if (flags == -1)
			throw std::system_error(errno, std::generic_category(), "fcntl(..., F_GETFL, ...)");

		if (value)
			flags |= O_NONBLOCK;
		else
			flags &= ~O_NONBLOCK;

		if (::fcntl(descriptor, F_SETFL, flags) == -1)
			throw std::system_error(errno, std::generic_category(), "fcntl(..., F_SETFL, ...)");
	}
	
	Handle::Handle(Descriptor descriptor) : _descriptor(descriptor)
	{
		if (_descriptor == -1)
			throw std::invalid_argument("invalid descriptor");
	}
	
	Handle::Handle(const Handle & other) : _descriptor(::dup(other._descriptor))
	{
		if (_descriptor == -1)
			throw std::system_error(errno, std::generic_category(), "dup");
	}
	
	Handle & Handle::operator=(const Handle & other)
	{
		auto result = ::dup2(other._descriptor, _descriptor);
		
		if (result == -1)
			throw std::system_error(errno, std::generic_category(), "dup2");
		
		return *this;
	}
	
	Handle::~Handle()
	{
		assert(_descriptor != -1);
		
		auto result = ::close(_descriptor);
		
		if (result == -1) {
			throw std::system_error(errno, std::generic_category(), "close");
		}
	}
}
