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
			auto result = ::socketpair(AF_LOCAL, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, descriptors);
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "socketpair(...)");
		} else {
			auto result = ::pipe(descriptors);
			
			if (result == -1)
				throw std::system_error(errno, std::generic_category(), "pipe(...)");
			
			set_non_blocking(descriptors[0], true);
			set_non_blocking(descriptors[1], true);
		}
		
		return std::make_pair(descriptors[0], descriptors[1]);
	}
	
	Pipe::Pipe(bool bidirectional) : Pipe(make_pipe(bidirectional))
	{
	}
}
