//
//  Buffer.cpp
//  This file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 11/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include <UnitTest/UnitTest.hpp>

#include <Async/Pipe.hpp>
#include <Async/Protocol/Buffer.hpp>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
					
					examiner.expect(buffer.size()) == 0u;
					buffer.append((const Byte *)message.data(), message.size());
						
					examiner.expect(buffer.size()) == message.size();
					buffer.consume(message.size());
				}
			},
			
			{"it can read data from a file",
				[](UnitTest::Examiner & examiner) {
					Handle file = ::open("/dev/zero", O_RDONLY);
					
					Buffer buffer(1024);
					buffer.read_from(file);
					
					examiner << "Filled the ring buffer with data from the file." << std::endl;
					examiner.expect(buffer.size()) == 1024u;
				}
			},
			
			{"it can read data into non-contiguous buffer",
				[](UnitTest::Examiner & examiner) {
					Handle file = ::open("/dev/zero", O_RDONLY);
					
					Buffer buffer(1024);
					buffer.read_from(file);
					
					examiner.expect(buffer.size()) == 1024u;
					examiner.expect(buffer.end()) == buffer.top();
					examiner.expect(buffer.is_contiguous()) == true;
					
					buffer.consume(512);
					examiner.expect(buffer.begin()) == (buffer.bottom() + 512);
					examiner.expect(buffer.end()) == buffer.top();
					
					buffer.read_from(file);
					examiner.expect(buffer.size()) == 512u;
					examiner.expect(buffer.begin()) == (buffer.bottom() + 512);
					examiner.expect(buffer.end()) == buffer.top();
					
					buffer.consume(1000);
					examiner.expect(buffer.size()) == 24u;
					
					buffer.read_from(file);
					examiner.expect(buffer.is_contiguous()) == false;
					
					examiner.expect(buffer.size()) == 536u;
					examiner.expect(buffer.total_size()) == 1024u;
				}
			},

			
			{"it can read data from a file in chunks",
				[](UnitTest::Examiner & examiner) {
					Handle file = ::open("/dev/zero", O_RDONLY);
					
					Buffer buffer(1024);
					buffer.read_from(file);
					
					buffer.consume(100);
					examiner.expect(buffer.read_from(file).size) == 100u;
					
					examiner << "Filled the ring buffer with data from the file." << std::endl;
					examiner.expect(buffer.size()) == 924u;
					examiner.expect(buffer.total_size()) == 1024u;
				}
			},
		};
	}
}
