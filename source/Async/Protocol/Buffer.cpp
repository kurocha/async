//
//  Buffer.cpp
//  File file is part of the "Async" project and released under the MIT License.
//
//  Created by Samuel Williams on 11/7/2017.
//  Copyright, 2017, by Samuel Williams. All rights reserved.
//

#include "Buffer.hpp"

#include <cstring>
#include <cstdlib>

#include <algorithm>
#include <stdexcept>
#include <system_error>

#include <sys/uio.h>
#include <errno.h>

namespace Async
{
	namespace Protocol
	{
		Buffer::Buffer(std::size_t size) : _size(size), _data(reinterpret_cast<Byte *>(malloc(size)))
		{
		}

		Buffer::~Buffer()
		{
			if (_data)
				free(_data);
		}

		std::size_t Buffer::total_size () const
		{
			return _used;
		}

		std::size_t Buffer::size () const
		{
			auto remainder = _size - _offset;
			
			return std::min(remainder, _used);
		}

		void Buffer::consume(std::size_t amount)
		{
			if (amount > _used) throw std::out_of_range("consuming more than available");
			
			// We do it this way to avoid any kind of overflow.
			auto remainder = _size - _offset;
			if (amount < remainder) {
				_offset += amount;
			} else {
				_offset = amount - remainder;
			}
			
			_used -= amount;
			
			// Reset the offset back to 0 if we consumed all data.
			if (_used == 0) {
				_offset = 0;
			}
		}

		std::size_t Buffer::append(const Byte * data, std::size_t size)
		{
			// Handle cases (0, 1)
			if (_used == _size) return 0;
			
			auto begin = this->free_begin();
			auto end = this->free_end();
			
			if (begin < end) {
				// Handle cases (2, 3, 4, 5)
				auto available = std::size_t(end-begin);
				auto amount = std::min(size, available);
				
				std::memcpy(begin, data, amount);
				_used += amount;
				
				return amount;
			} else {
				// Handle cases (6)
				// Write data from free begin to the end of the _data buffer:
				auto available = std::size_t((_data + _size) - begin);
				auto amount = std::min(size, available);
				
				std::memcpy(begin, data, amount);
				_used += amount;
				
				// If there is still more to write:
				if (amount < size) {
					auto available = std::size_t(end - _data);
					auto remainder = std::min(size - amount, available);
					
					std::memcpy(_data, data, remainder);
					_used += remainder;
					
					return amount + remainder;
				} else {
					return amount;
				}
			}
		}

		bool Buffer::is_contiguous() const
		{
			// The RHS s the biggest offset which can possibly be contiguous.
			// Buffer size: 20, size: 8, offset <= 12 (base 0)
			// We do it this way to avoid any kind of buffer overflow.
			return _offset <= _size - _used;
		}

		Byte * Buffer::free_begin()
		{
			auto begin = this->end();
			
			if (begin == (_data + _size))
				return _data;
			
			return begin;
		}

		Byte * Buffer::free_end()
		{
			auto end = this->begin();
			
			if (end == _data)
				return _data + _size;
			
			return end;
		}

		Result Buffer::read_from(Descriptor descriptor)
		{
			// There are at most two discrete segments to read into.
			// [ * ] indicates used area.
			// < - > indicates free area (where we want to place data from read).
			
			// Here are the cases with no free space:
			// [ * * * * * * * * * * * * * * ] (0)
			// * * ] [ * * * * * * * * * * * * (1)
			
			// Here are all cases with one segment:
			// < - - - - - - - - - - - - - - > (2)
			// [ * * * ] < - - - - - - - - - > (3)
			// < - - - - - - - - - > [ * * * ] (4)
			// * * ] < - - - - - - - - - > [ * (5)
			
			// Here are all cases with two segments:
			// < - > [ * * * ] < - - - - - - > (6)
			
			// No free space to read into (case 0, 1).
			if (_used == _size) return Result::OK;
			
			// We need to handle several cases here, as efficiently as possible.
			struct iovec iov[2];
			int count = 0;
			
			auto begin = this->free_begin();
			auto end = this->free_end();
			
			if (begin < end) {
				// Handle cases (2, 3, 4, 5)
				iov[0].iov_base = begin;
				iov[0].iov_len = end - begin;
				count = 1;
			} else {
				// Handle cases (6)
				iov[0].iov_base = begin;
				iov[0].iov_len = (_data + _size) - begin;
				iov[1].iov_base = _data;
				iov[1].iov_len = end - _data;
				count = 2;
			}
			
			// for (std::size_t i = 0; i < count; i += 1) {
			// 	std::cerr << "iov[" << i << "] " << ((Byte *)iov[i].iov_base - _data) << " length " << iov[i].iov_len << std::endl;
			// }
			
			auto result = ::readv(descriptor, iov, count);
			
			if (result > 0) {
				_used += result;
				
				return {Result::OK, result};
			} else {
				return Result::check(result, "readv");
			}
		}

		Result Buffer::write_to(Descriptor descriptor)
		{
			// There are at most two discrete segments to write from.
			// [ * ] indicates used area (data we want to write).
			// < - > indicates free area.
			
			// Here are the cases with empty buffer:
			// < - - - - - - - - - - - - - - > (0)
			
			// Here are all cases with one segment:
			// [ * * * * * * * * * * * * * * ] (1)
			// [ * * * ] < - - - - - - - - - > (2)
			// < - - - - - - - - - > [ * * * ] (3)
			// < - > [ * * * ] < - - - - - - > (4)
			
			// Here are all cases with two segments:
			// * * ] < - - - - - - - - - > [ * (5)
			// * * ] [ * * * * * * * * * * * * (6)
			
			// Nothing to write (case 0).
			if (_used == 0) return Result::OK;
			
			// We need to handle several cases here, as efficiently as possible.
			struct iovec iov[2];
			int count = 0;
			
			auto begin = this->begin();
			auto end = this->end();
			
			if (begin < end) {
				// Handle cases (1, 2, 3, 4)
				iov[0].iov_base = begin;
				iov[0].iov_len = end - begin;
				count = 1;
			} else {
				// Handle cases (5)
				iov[0].iov_base = begin;
				iov[0].iov_len = (_data + _size) - begin;
				iov[1].iov_base = _data;
				iov[1].iov_len = end - _data;
				count = 2;
			}
			
			auto result = ::writev(descriptor, iov, count);
			
			if (result > 0) {
				_used -= result;
				
				return {Result::OK, result};
			} else {
				return Result::check(result, "writev");
			}
		}
		
		Result Buffer::read_from(Descriptor descriptor, Readable & readable)
		{
			while (!full()) {
				auto result = read_from(descriptor);
				
				if (result.is_pending()) {
					readable.wait();
				} else {
					return result;
				}
			}
			
			return Result::WAITING;
		}
		
		Result Buffer::write_to(Descriptor descriptor, Writable & writable)
		{
			while (!empty()) {
				auto result = write_to(descriptor);
				
				if (result.is_pending()) {
					writable.wait();
				} else {
					return result;
				}
			}
			
			return Result::WAITING;
		}
	}
}
