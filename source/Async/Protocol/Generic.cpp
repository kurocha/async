//
//  Generic.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 11/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Generic.hpp"

namespace Async
{
	namespace Protocol
	{
		Generic::Generic(Descriptor descriptor, Reactor & reactor) : _descriptor(descriptor), _reactor(reactor)
		{
		}
		
		Generic::~Generic()
		{
		}
	}
}
