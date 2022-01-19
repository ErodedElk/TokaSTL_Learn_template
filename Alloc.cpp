#include "Alloc.h"
#include "Allocator.h"
namespace TokameinE {
	char* alloc::start_free = 0;
	char* alloc::end_free = 0;
	size_t alloc::heap_size = 0;

	//��ʼ״̬�£�Ĭ�ϸ�����������
	//Toka: �����ѧ��һЩpwn,Ӧ�þͻ�ע�⵽,free_list���߼���binһ��
	//������allocate��deallocate
	alloc::obj* volatile alloc::free_list[alloc::ENFreeLists::NFREELISTS] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	};


	void* alloc::allocate(size_t bytes) {
		//Toka: �����޶�֮�󣬲��õ�һ����������ֱ�ӵ���malloc
		if (bytes > EMaxBytes::MAXBYTES)
		{
			return malloc(bytes);
		}
		//Toka: ���򣬲��õڶ���������
		size_t index = FREELIST_INDEX(bytes);
		obj* tmp = free_list[index];
		if (tmp)
		{
			free_list[index] = tmp->next;
			return tmp;
		}
		else
		{
			return refill(index*(EAlign::ALIGN));//���ض����ĵ�ַ(��indexӦΪ������ϵ)
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


	//refill:���free_list
	//bytes�Ѿ�����
	void* alloc::refill(size_t bytes) {
		size_t nobjs = ENObjs::NOBJS;
		//���ڴ����ȡ
		//Toka: nobjs pass by reference,���޶����ص�chunk��
		//Toka: ȡ�� bytes*nobjs �����Ŀռ�
		char* chunk = chunk_alloc(bytes, nobjs);
		obj* volatile *  my_free_list = 0;
		obj* result = 0;
		obj* current_obj = 0, * next_obj = 0;

		//Toka: "1==other"��д����û��ʲô����,�������©д��һ���Ⱥžͻ����̱���,����debug
		//��ֻȡ��һ��chunk,ֱ�ӷ��ؼ���
		if (1== nobjs) {
			return chunk;
		}
		my_free_list = free_list + FREELIST_INDEX(bytes);
		result = (obj*)(chunk);//ȡ����һ��chunk
		*my_free_list = next_obj = (obj*)(chunk + bytes);
		//������� chunk �����Ӧ��free_list
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

	//���ڴ�ػ�ȡchunk
	//Toka: bytes�Ѿ�����
	char* alloc::chunk_alloc(size_t bytes, size_t& nobjs) {
		char* result = 0;
		size_t total_bytes = bytes * nobjs;
		size_t bytes_left = end_free - start_free;

		if (bytes_left >= total_bytes) {//�ڴ��ʣ��ռ����
			result = start_free;
			start_free = start_free + total_bytes;
			return result;
		}
		else if (bytes_left >= bytes) {//����ȫ��,�����ٹ�һ��
			nobjs = bytes_left / bytes;
			total_bytes = nobjs * bytes;
			result = start_free;
			start_free += total_bytes;
			return result;
		}
		else {//һ��chunk�Ŀռ䶼û��
			size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
			if (bytes_left > 0) {
				//�ڴ���ڻ���ʣ�µĿռ�,����free_list
				obj* volatile * my_free_list = free_list + FREELIST_INDEX(bytes_left);
				((obj*)start_free)->next = *my_free_list;
				*my_free_list = (obj*)start_free;
			}
			//���¿����ڴ��
			start_free = (char*)malloc(bytes_to_get);
			if (!start_free) {
				//����ʧ��
				//����free_list
				obj* volatile* my_free_list = 0, * p = 0;
				for (int i = bytes; i <= EMaxBytes::MAXBYTES; i += EAlign::ALIGN) {
					my_free_list = free_list + FREELIST_INDEX(i);
					p = *my_free_list;
					if (p != 0) {
						//�����ҵ��˿���chunk�����
						//����chunk�����ڴ��,�ٴ�chunk_alloc���Դ��ڴ�ػ�ȡchunk
						//Toka: �����������ڴ����û�ܱ�����,�����»ص�free_list
						//����Ȼ,����ܹ��ҵ�,��Ȼ���ܹ��������,�ݹ���õ���ҪĿ����Ϊ�˸���nobjs
						*my_free_list = p->next;
						start_free = (char*)p;
						end_free = start_free + i;
						return chunk_alloc(bytes, nobjs);
					}
				}
				end_free = 0;
				//����allocator<size_t>::allocate���Է���,������ʧ�ܾͻ���Ϊ������exit
				start_free = (char*)allocator<size_t>::allocate(bytes_to_get);
			}
			heap_size += bytes_to_get;
			end_free = start_free + bytes_to_get;
			return chunk_alloc(bytes, nobjs);
		}
	}


}
