//
//  Notification.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 13/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Reactor.hpp"
#include "Pipe.hpp"

namespace Async
{
	class Notification
	{
	public:
		Notification(Reactor & reactor);
		virtual ~Notification();
		
		void wait();
		
		void signal();
		
	private:
		Reactor & _reactor;
		Pipe _pipe;
	};
}
