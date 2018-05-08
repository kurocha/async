//
//  Monitor.hpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 8/5/2018.
//  Copyright, 2018, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Reactor.hpp"

namespace Async
{
	class Monitor
	{
	public:
		Monitor(Descriptor descriptor, Reactor & reactor);
		~Monitor();
		
		enum Event : int {
			NONE = 0,
#if defined(ASYNC_KQUEUE)
			READABLE = EVFILT_READ,
			WRITABLE = EVFILT_WRITE,
#elif defined(ASYNC_EPOLL)
			READABLE = EPOLLIN,
			WRITABLE = EPOLLOUT,
#endif
		};
		
		void wait_readable();
		void wait_writable();
		
		void wait(Event event);
		
	protected:
		bool _added = false;
		
		Event _events = NONE;
		
		Descriptor _descriptor;
		Reactor & _reactor;
		
		void append();
	};
}
