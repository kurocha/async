//
//  Writable.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 29/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Writable.hpp"

#include <Concurrent/Fiber.hpp>

#include <cassert>

namespace Async
{
	using namespace Concurrent;
	
	Writable::Writable(FileDescriptor file_descriptor, Reactor * reactor) : _file_descriptor(file_descriptor), _reactor(reactor)
	{
		_reactor->changes().emplace_back(
			_file_descriptor,
			EVFILT_WRITE,
			EV_ADD | EV_CLEAR,
			0,
			0,
			(void*)Fiber::current
		);
	}
	
	Writable::~Writable()
	{
		_reactor->changes().emplace_back(
			_file_descriptor,
			EVFILT_WRITE,
			EV_DELETE,
			0,
			0,
			nullptr
		);
	}
	
	void Writable::wait()
	{
		assert(Fiber::current);
		
		Fiber::current->yield();
	}
}
