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

namespace Async
{
	namespace Protocol
	{
		Result Result::check(ssize_t result, const char * what)
		{
			if (result == -1) {
				if (errno == EWOULDBLOCK || errno == EAGAIN) {
					return Result::WAITING;
				} else if (errno == EINTR) {
					return Result::INTERRUPTED;
				}
				
				throw std::system_error(errno, std::generic_category(), "what");
			} else if (result == 0) {
				return Result::CLOSED;
			} else {
				return {Result::OK, result};
			}
		}
	}
}
