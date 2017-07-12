//
//  Result.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 11/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Result.hpp"

#include <system_error>
#include <errno.h>

#include <cassert>

namespace Async
{
	namespace Protocol
	{
		Result::Result(Status status_, ssize_t size_) : status(status_)
		{
			assert(size_ > 0);
			size = static_cast<std::size_t>(size_);
		}
		
		Result Result::check(ssize_t result, const char * what)
		{
			if (result == -1) {
				if (errno == EWOULDBLOCK || errno == EAGAIN) {
					return Result::WAITING;
				} else if (errno == EINTR) {
					return Result::INTERRUPTED;
				} else if (errno == EPIPE || errno == ECONNRESET) {
					return Result::FAILED;
				}
				
				throw std::system_error(errno, std::generic_category(), what);
			} else if (result == 0) {
				return Result::CLOSED;
			} else {
				return {Result::OK, result};
			}
		}
	}
}
