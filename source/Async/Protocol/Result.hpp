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
				OK = 0,
				// EAGAIN or EWOULDBLOCK:
				WAITING = 0b0001,
				// EINTR:
				INTERRUPTED = 0b0010,
				// read/write returned 0
				CLOSED = 0b0100,
				// ECONNRESET or EPIPE
				FAILED = 0b1100,
			} status;
			
			std::size_t size = 0;
			
			Result(Status status_, ssize_t size_);
			Result(Status status_ = OK, std::size_t size_ = 0) : status(status_), size(size_) {}
			
			bool operator==(const Status & status_) const noexcept {return status == status_;}
			bool operator!=(const Status & status_) const noexcept {return status != status_;}
			
			bool is_closed() const noexcept {return status & CLOSED;}
			
			static Result check(ssize_t result, const char * what);
		};
	}
}
