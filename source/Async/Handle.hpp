//
//  Handle.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 1/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

namespace Async
{
	typedef int Descriptor;
	
	void set_non_blocking(Descriptor descriptor, bool value = true);
	
	class Reactor;
	
	// Takes ownership of a descriptor and closes it when it goes out of scope.
	class Handle
	{
	public:
		Handle() {}
		
		// Takes ownership of the descriptor.
		Handle(Descriptor descriptor);
		
		// Calls close on the descriptor.
		~Handle();
		
		// Dup the descriptor into the handle.
		Handle(const Handle & other);
		Handle & operator=(const Handle & other);
		
		Handle(Handle && other);
		Handle & operator=(Handle && other);
		
		operator Descriptor() const {return _descriptor;}
		
	protected:
		Descriptor _descriptor = -1;
	};
}
