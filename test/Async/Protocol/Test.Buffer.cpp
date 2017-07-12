//
//  Test.Buffer.cpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 11/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Async/Pipe.hpp>
#include <Async/Protocol/Buffer.hpp>

namespace Async
{
	namespace Protocol
	{
		UnitTest::Suite BufferTestSuite {
			"Async::Protocol::Buffer",
			
			{"it can append data",
				[](UnitTest::Examiner & examiner) {
					Buffer buffer(1024);
					
					std::string message = "Hello World";
					
					for (std::size_t i = 0; i < 1000; i += 1) {
						examiner.expect(buffer.size()) == 0u;
						buffer.append((const Byte *)message.data(), message.size());
						
						examiner.expect(buffer.size()) == message.size();
						buffer.consume(message.size());
					}
				}
			},
		};
	}
}
