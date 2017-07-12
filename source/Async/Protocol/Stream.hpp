//
//  Stream.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 11/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Result.hpp"
#include "Generic.hpp"

#include <string>

namespace Async
{
	namespace Protocol
	{
		class Stream : public Generic
		{
		public:
			Stream(Descriptor descriptor, Reactor & reactor);
			virtual ~Stream();
			
			Result read(Byte * begin, const Byte * end, bool partial = true);
			std::string read(std::size_t size, bool partial = true);
			
			Result write(const Byte * begin, const Byte * end);
			Result write(const std::string & buffer);
			
		protected:
			Result read_partial(Byte * begin, const Byte * end);
			Result write_partial(const Byte * begin, const Byte * end);
		};
	}
}
