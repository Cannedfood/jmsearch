#include "ArenaAllocator.hpp"

#include <memory.h>
#include <cstdlib>
#include <cstdio>


ArenaAllocator::ArenaAllocator(size_t block_size) :
	mTotal(0),
	mWasted(0),
	mData(),
	mBlockSize(block_size),
	mStack(nullptr),
	mStackSize(0),
	mStackUsed(0)
{
	startNewStack(0);
}

void ArenaAllocator::startNewStack(size_t minSize) {
	if(mStackUsed > 0) {
		mWasted += mStackSize - mStackUsed;
		mData.emplace_back(std::move(mStack));
	}

	size_t size = std::max(minSize, mBlockSize);
	mStack.reset(new char[size]); mTotal += size;

	mStackSize  = size;
	mStackUsed  = 0;
}

void* ArenaAllocator::allocate(size_t n) {
	if(mStackUsed + n > mStackSize) { // TODO: is this right? Should it be >=?
		startNewStack(n);
	}

	void* result = mStack.get() + mStackUsed;
	mStackUsed += n;

	return result;
}

char* ArenaAllocator::allocateString(const char *s, size_t len) {
	char* re = (char*) allocate(len + 1);
	std::copy(s, s + len, re);
	re[len] = '\0';
	return re;
}
