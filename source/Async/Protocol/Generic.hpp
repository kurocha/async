//
//  Generic.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 11/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include "../Handle.hpp"

namespace Async
{
	namespace Protocol
	{
		class Generic
		{
		public:
			typedef unsigned char Byte;
			
			Generic(Descriptor descriptor, Reactor & reactor);
			virtual ~Generic();
			
		protected:
			Descriptor _descriptor;
			Reactor & _reactor;
		};
	}
}
