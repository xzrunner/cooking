#include "cooking/LinearAllocator.h"

#include <algorithm>

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>

namespace cooking
{

#define INITIAL_PAGE_SIZE ((size_t)512) // 512b
#define MAX_PAGE_SIZE ((size_t)131072) // 128kb

#if ALIGN_DOUBLE
#define ALIGN_SZ (sizeof(double))
#else
#define ALIGN_SZ (sizeof(int))
#endif

#define ALIGN(x) (((x) + ALIGN_SZ - 1 ) & ~(ALIGN_SZ - 1))
#define ALIGN_PTR(p) ((void*)(ALIGN((size_t)(p))))

class LinearAllocator::Page
{
public:
	Page() : m_next(NULL) {}

	Page* Next() { return m_next; }
	void SetNext(Page* next) { m_next = next; }

	void* operator new(size_t size, void* buf) { return buf; }

	void* Start() {
		return (void*) (((size_t)this) + sizeof(Page));
	}

	void* End(int page_size) {
		return (void*) (((size_t)Start()) + page_size);
	}

private:
	Page(const Page& other) {}

private:
	Page* m_next;

}; // LinearAllocator::Page

LinearAllocator::LinearAllocator()
	: m_page_size(INITIAL_PAGE_SIZE)
	, m_next(NULL)
	, m_curr_page(NULL)
	, m_pages(NULL)
	, m_total_allocated(0)
	, m_wasted_space(0)
	, m_page_count(0)
{
}

LinearAllocator::~LinearAllocator()
{
	Page* p = m_pages;
	while (p) {
		Page* next = p->Next();
		p->~Page();
		free(p);
		p = next;
	}
}

void LinearAllocator::Rewind()
{
	m_curr_page = m_pages;
	m_wasted_space = m_total_allocated;
}

void* LinearAllocator::AllocImpl(size_t size)
{
	size = ALIGN(size);
	assert(size < m_page_size);
	if (size >= m_page_size) {
		return NULL;
	}
	EnsureNext(size);
	void* ptr = m_next;
	m_next = ((char*)m_next) + size;
	m_wasted_space -= size;
	return ptr;
}

bool LinearAllocator::FitsInCurrPage(size_t size)
{
	return m_next && ((char*)m_next + size) <= End(m_curr_page);
}

void LinearAllocator::EnsureNext(size_t size)
{
	if (FitsInCurrPage(size)) {
		return;
	} else if (m_curr_page && m_curr_page->Next()) {
		m_curr_page = m_curr_page->Next();
		m_next = Start(m_curr_page);
		return;
	}

	if (m_curr_page && m_page_size < MAX_PAGE_SIZE) {
		m_page_size = std::min(MAX_PAGE_SIZE, m_page_size * 2);
		m_page_size = ALIGN(m_page_size);
	}
	m_wasted_space += m_page_size;
	Page* p = NewPage(m_page_size);
	if (m_curr_page) {
		m_curr_page->SetNext(p);
	}
	m_curr_page = p;
	if (!m_pages) {
		m_pages = m_curr_page;
	}
	m_next = Start(m_curr_page);
}

void* LinearAllocator::Start(Page* p)
{
	return ALIGN_PTR((size_t)p + sizeof(Page));
}

void* LinearAllocator::End(Page* p)
{
	return ((char*)p) + m_page_size;
}

LinearAllocator::Page* LinearAllocator::NewPage(size_t page_size)
{
	page_size = ALIGN(page_size + sizeof(LinearAllocator::Page));
	m_total_allocated += page_size;
	++m_page_count;
	void* buf = malloc(page_size);
	return new (buf) Page();
}

}