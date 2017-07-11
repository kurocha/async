//
//  Stream.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 11/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Stream.hpp"

#include <system_error>

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

#include "../Readable.hpp"
#include "../Writable.hpp"

namespace Async
{
	namespace Protocol
	{
		Stream::Stream(Descriptor descriptor, Reactor & reactor) : Generic(descriptor, reactor)
		{
		}
		
		Stream::~Stream()
		{
		}
		
		Result Stream::read(Byte * begin, const Byte * end, bool partial)
		{
			Readable event(_descriptor, _reactor);
			
			auto mark = begin;
			
			while (mark != end) {
				auto result = read_partial(mark, end);
				
				if (result == Result::OK) {
					if (partial) return result;
					
					mark += result.size;
				} else if (result == Result::CLOSED) {
					throw std::runtime_error("could not complete read, remote end shutdown");
				} else {
					event.wait();
				}
			}
			
			return {Result::OK, end-begin};
		}
		
		std::string Stream::read(std::size_t size, bool partial)
		{
			Byte buffer[size];
			
			auto result = read(buffer, buffer+size, partial);
			
			return std::string(buffer, buffer+result.size);
		}
		
		void Stream::write(const Byte * begin, const Byte * end)
		{
			Writable event(_descriptor, _reactor);
			
			auto mark = begin;
			
			while (mark != end) {
				auto result = write_partial(mark, end);
				
				if (result == Result::OK) {
					mark += result.size;
				} else if (result == Result::CLOSED) {
					throw std::runtime_error("could not complete write, remote end shutdown");
				} else {
					event.wait();
				}
			}
		}
		
		void Stream::write(const std::string & buffer)
		{
			auto begin = reinterpret_cast<const Byte *>(buffer.data());
			auto end = reinterpret_cast<const Byte *>(buffer.data() + buffer.size());
			
			write(begin, end);
		}
		
		Result Stream::read_partial(Byte * begin, const Byte * end)
		{
			auto result = ::read(_descriptor, begin, end - begin);
			
			return Result::check(result, "read");
		}
		
		Result Stream::write_partial(const Byte * begin, const Byte * end)
		{
			auto result = ::write(_descriptor, begin, end - begin);
			
			return Result::check(result, "write");
		}
	}
}
