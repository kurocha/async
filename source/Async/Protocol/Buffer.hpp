//
//  Buffer.hpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 11/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#pragma once

#include "Result.hpp"

#include "../Handle.hpp"
#include "../Readable.hpp"
#include "../Writable.hpp"

#include <cstddef>

namespace Async
{
	namespace Protocol
	{
		typedef unsigned char Byte;
		
		class Buffer
		{
		public:
			Buffer(std::size_t size);
			virtual ~Buffer();
			
			/// The total data size in the ring buffer of all segments.
			std::size_t total_size () const;
			
			/// The start of the first contiguous segment:
			Byte * data() {return _data + _offset;}
			const Byte * data() const {return _data + _offset;}
			
			/// The contiguous length of the buffer.
			std::size_t size() const;
			
			bool empty() const {return _used == 0;}
			bool full() const {return _used == _size;}
			
			/// The start of the first contiguous segment.
			Byte * begin() {return data();}
			const Byte * begin() const {return data();}
			
			Byte * end() {return data() + size();}
			const Byte * end() const {return data() + size();}
			
			// These are primarily for testing.
			Byte * bottom() {return _data;}
			const Byte * bottom() const {return _data;}
			
			Byte * top() {return _data + _size;}
			const Byte * top() const {return _data + _size;}
			
			/// Increment the head of the buffer by the given amount.
			/// throw std::out_of_range if this would increment head past the tail.
			void consume(std::size_t amount);
			
			/// Append data to the buffer. Returns the actual amount appended.
			std::size_t append(const Byte * data, std::size_t size);
			
			// Whether the ring buffer has one contiguous segment or has wrapped around.
			bool contiguous() const;
			
			Result read_from(Descriptor descriptor);
			Result write_to(Descriptor descriptor);
			
			Result read_from(Descriptor descriptor, Readable & readable);
			Result write_to(Descriptor descriptor, Writable & writable);
			
		private:
			Byte * free_begin();
			Byte * free_end();
			
			std::size_t _size = 0;
			Byte * _data = nullptr;
			
			std::size_t _offset = 0, _used = 0;
		};

	}
}
