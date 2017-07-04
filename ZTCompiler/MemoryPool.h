#pragma once
#ifndef _ZT_MEMORYPOOL_H_
#define _ZT_MEMORYPOOL_H_
#include <vector>
#include <cassert>
namespace ztCompiler {
	class memory_pool {
		static const int block_size = 4 * 1024;	//默认的内存分配大小
	private:
		char* alloc_ptr;	//内存的偏移指针，指向未使用的内存的首地址
		size_t remained;	//剩余的内存
		std::vector<char*> blocks;	//
		size_t allocated_memory;	//已分配的总内存
	public:
		memory_pool() {
			allocated_memory = 0;
			alloc_ptr = nullptr;
			remained = 0;
		}

		~memory_pool() {
			for (int i = 0; i < blocks.size();++i)
				delete[] blocks[i];
		}
	public:
		char* allocate(size_t bytes) {
			assert(bytes >0);
			//如果需要的内存少于剩余的内存，则直接分配。即移动指针，减少剩余内存数，并返回刚分配内存的首地址
			if (bytes <= remained) {	
				char* begin_pos = alloc_ptr;	//先保存指针的初始位置
				alloc_ptr += bytes;
				remained -= bytes;
				return begin_pos;
			}
			//当需求内存大于剩余内存时
			return allocate_fallback(bytes);
		}
	private:
		char* allocate_fallback(size_t bytes) {
			if (bytes > block_size/4) {	//当需求内存大于1024kb时，则给其单独分配一块大小为bytes的内存
				char* ret = allocate_new_block(bytes);
				return ret;
			}
			//当所需内存大于剩余内存，且小于1024kb时
			alloc_ptr = allocate_new_block(block_size);
			remained = block_size;

			char* begin_pos = alloc_ptr;
			alloc_ptr += bytes;
			remained -= bytes;
			return begin_pos;
		}
		char* allocate_new_block(size_t bytes) {
			char* ret = new char[bytes];	//分配内存
			allocated_memory += bytes;
			blocks.push_back(ret);	//添加进内存指针数组
			return ret;
		}
	};
}

#endif