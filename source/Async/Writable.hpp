//
//  Writable.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 29/6/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Reactor.hpp"

namespace Async
{
	class Writable
	{
	public:
		Writable(Descriptor descriptor, Reactor & reactor);
		~Writable();
		
		void wait();
		
	private:
		Descriptor _descriptor;
		Reactor & _reactor;
	};
}
