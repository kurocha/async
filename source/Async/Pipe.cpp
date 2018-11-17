//
//  Pipe.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Pipe.hpp"

#include <unistd.h>
#include <fcntl.h>

#include <system_error>

#include <sys/socket.h>

#include <iostream>

namespace Async
{
	inline static std::pair<Descriptor, Descriptor> make_pipe(bool bidirectional)
	{
		Descriptor descriptors[2];
		
		if (bidirectional) {
#if defined(SOCK_NONBLOCK) && defined(SOCK_CLOEXEC)
			auto result = ::socketpair(AF_LOCAL, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, descriptors);
#else
			auto result = ::socketpair(AF_LOCAL, SOCK_STREAM, 0, descriptors);
			
			update_flags(descriptors[0], O_NONBLOCK | O_CLOEXEC);
			update_flags(descriptors[1], O_NONBLOCK | O_CLOEXEC);
#endif
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "socketpair(...)");
		} else {
#if defined(_GNU_SOURCE)
			auto result = ::pipe2(descriptors, O_NONBLOCK | O_CLOEXEC);
#else
			auto result = ::pipe(descriptors);
			
			update_flags(descriptors[0], O_NONBLOCK | O_CLOEXEC);
			update_flags(descriptors[1], O_NONBLOCK | O_CLOEXEC);
#endif
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "pipe(...)");
		}
		
		return std::make_pair(descriptors[0], descriptors[1]);
	}
	
	Pipe::Pipe(bool bidirectional) : Pipe(make_pipe(bidirectional))
	{
	}
}
