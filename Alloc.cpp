#include "Alloc.h"
#include "Allocator.h"
namespace TokameinE {
	char* alloc::start_free = 0;
	char* alloc::end_free = 0;
	size_t alloc::heap_size = 0;

	//初始状态下，默认该链表无内容
	//Toka: 如果您学过一些pwn,应该就会注意到,free_list的逻辑和bin一样
	//尤其是allocate和deallocate
	alloc::obj* volatile alloc::free_list[alloc::ENFreeLists::NFREELISTS] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};


	void* alloc::allocate(size_t bytes) {
		//Toka: 超过限度之后，采用第一级配置器，直接调用malloc
		if (bytes > EMaxBytes::MAXBYTES)
		{
			return malloc(bytes);
		}
		//Toka: 否则，采用第二种配置器
		size_t index = FREELIST_INDEX(bytes);
		obj* tmp = free_list[index];
		if (tmp)
		{
			free_list[index] = tmp->next;
			return tmp;
		}
		else
		{
			return refill(index*(EAlign::ALIGN));//返回对齐后的地址(与index应为倍数关系)
		}

	}

	void alloc::deallocate(void* ptr, size_t bytes) {
		if (bytes > EMaxBytes::MAXBYTES)
		{
			free(ptr);
			return;
		}
		size_t index = FREELIST_INDEX(bytes);
		obj* node = static_cast<obj*>(ptr);
		node->next = free_list[index];
		free_list[index] = node;
	}

	void* alloc::reallocate(void* ptr, size_t old_sz, size_t new_sz) {
		//do somethings
		return 0;
	}


	//refill:填充free_list
	//bytes已经对齐
	void* alloc::refill(size_t bytes) {
		size_t nobjs = ENObjs::NOBJS;
		//从内存池里取
		//Toka: nobjs pass by reference,将修订返回的chunk数
		//Toka: 取出 bytes*nobjs 数量的空间
		char* chunk = chunk_alloc(bytes, nobjs);
		obj* volatile *  my_free_list = 0;
		obj* result = 0;
		obj* current_obj = 0, * next_obj = 0;

		//Toka: "1==other"的写法并没有什么特殊,但如果你漏写了一个等号就会立刻报错,方便debug
		//若只取出一块chunk,直接返回即可
		if (1== nobjs) {
			return chunk;
		}
		my_free_list = free_list + FREELIST_INDEX(bytes);
		result = (obj*)(chunk);//取出第一块chunk
		*my_free_list = next_obj = (obj*)(chunk + bytes);
		//将多余的 chunk 链入对应的free_list
		for (int i = 1;; ++i) {
			current_obj = next_obj;
			next_obj = (obj*)((char*)next_obj + bytes);
			if (nobjs - 1 == i) {
				current_obj->next = 0;
				break;
			}
			else {
				current_obj->next = next_obj;
			}
		}
		return result;
	}

	//从内存池获取chunk
	//Toka: bytes已经对齐
	char* alloc::chunk_alloc(size_t bytes, size_t& nobjs) {
		char* result = 0;
		size_t total_bytes = bytes * nobjs;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes) {//内存池剩余空间充足
			result = start_free;
			start_free = start_free + total_bytes;
			return result;
		}
		else if (bytes_left >= bytes) {//不完全够,但至少够一个
			nobjs = bytes_left / bytes;
			total_bytes = nobjs * bytes;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else {//一个chunk的空间都没有
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			if (bytes_left > 0) {
				//内存池内还有剩下的空间,放入free_list
				obj* volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((obj*)start_free)->next = *my_free_list;
				*my_free_list = (obj*)start_free;
			}
			//重新开辟内存池
			start_free = (char*)malloc(bytes_to_get);
			if (!start_free) {
				//开辟失败
				//遍历free_list
				obj* volatile* my_free_list = 0, * p = 0;
				for (int i = bytes; i <= EMaxBytes::MAXBYTES; i += EAlign::ALIGN) {
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != 0) {
						//对于找到了空闲chunk的情况
						//将该chunk放入内存池,再次chunk_alloc尝试从内存池获取chunk
						//Toka: 但这个归入的内存如果没能被分配,会重新回到free_list
						//但显然,如果能够找到,必然是能够被分配的,递归调用的主要目的是为了更新nobjs
						*my_free_list = p->next;
						start_free = (char*)p;
						end_free = start_free + i;
						return chunk_alloc(bytes, nobjs);
					}
				}
				end_free = 0;
				//复用allocator<size_t>::allocate尝试分配,若分配失败就会因为检测机制exit
				start_free = (char*)allocator<size_t>::allocate(bytes_to_get);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(bytes, nobjs);
		}
	}


}
