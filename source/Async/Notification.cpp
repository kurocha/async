//
//  Notification.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 13/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Notification.hpp"

#include "Readable.hpp"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

#include <system_error>

namespace Async
{
	Notification::Notification(Reactor & reactor) : _reactor(reactor)
	{
	}
	
	Notification::~Notification()
	{
	}
	
	void Notification::wait()
	{
		Readable event(_pipe.input, _reactor);
		
		event.wait();
		
		char buffer;
		auto result = ::read(_pipe.input, &buffer, 1);
		
		if (result == -1) {
			throw std::system_error(errno, std::generic_category(), "Notification::wait -> read");
		}
	}
	
	void Notification::signal()
	{
		auto result = ::write(_pipe.output, ".", 1);
		
		if (result == -1) {
			throw std::system_error(errno, std::generic_category(), "Notification::signal -> write");
		}
	}
}
