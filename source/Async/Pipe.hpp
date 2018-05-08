//
//  Pipe.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include <utility>

#include "Handle.hpp"

namespace Async
{
	struct Pipe
	{
		Pipe(bool bidirectional = false);
		
		Pipe(std::pair<Descriptor, Descriptor> descriptors) : input(descriptors.first), output(descriptors.second) {}
		
		// Data written to descriptors[1] appears on (i.e., can be read from) descriptors[0].
		Handle input;
		Handle output;
	};
}
