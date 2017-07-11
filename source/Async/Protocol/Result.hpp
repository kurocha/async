//
//  Result.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 11/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include <cstddef>
#include <sys/types.h>

namespace Async
{
	namespace Protocol
	{
		struct Result
		{
			enum Status {
				OK = 0, WAITING, INTERRUPTED, CLOSED
			} status;
			
			std::size_t size = 0;
			
			Result(Status status_ = OK, std::size_t size_ = 0) : status(status_), size(size_) {}
			
			bool operator==(const Status & status_) {return status == status_;}
			bool operator!=(const Status & status_) {return status != status_;}
			
			static Result check(ssize_t result, const char * what);
		};
	}
}
