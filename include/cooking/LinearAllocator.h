// https://github.com/android/platform_frameworks_base/blob/master/libs/hwui/utils/LinearAllocator.h

#ifndef _COOKING_LINEAR_ALLOCATOR_H_
#define _COOKING_LINEAR_ALLOCATOR_H_

namespace cooking
{

class LinearAllocator
{
public:
	LinearAllocator();
	~LinearAllocator();

	template<class T>
	void* alloc(size_t size) {
// 		static_assert(std::is_trivially_destructible<T>::value,
// 			"Error, type is non-trivial! did you mean to use create()?");
		return AllocImpl(size);
	}

private:
	class Page;

	LinearAllocator(const LinearAllocator& other);

	void* AllocImpl(size_t size);

	bool FitsInCurrPage(size_t size);
	void EnsureNext(size_t size);
	void* Start(Page* p);
	void* End(Page* p);
	Page* NewPage(size_t page_size);

private:
	size_t m_page_size;
	void* m_next;
	Page* m_curr_page;
	Page* m_pages;

	// Memory usage tracking
	size_t m_total_allocated;
	size_t m_wasted_space;
	size_t m_page_count;

}; // LinearAllocator

}

#endif // _COOKING_LINEAR_ALLOCATOR_H_