#include "ArenaAllocator.hpp"

#include <memory.h>
#include <cstdlib>
#include <cstdio>

char* ArenaAllocator::allocate(size_t n) {
	char* result = mStackAllocator.allocate(n);

	if(!result) // -> remaining space too small
	{
		if(!mStackAllocator.empty()) {
			mWasted += mStackAllocator.remaining();
			mData.emplace_back(std::move(mStackAllocator.mBlock));
		}
		mStackAllocator = StackAllocator(std::max(n * 2, mBlockSize));
		result = mStackAllocator.allocate(n);
	}

	return result;
}

char* ArenaAllocator::allocateString(const char *s, size_t len) {
	char* re = allocate(len + 1);
	if(re) {
		memcpy(re, s, len);
		re[len] = '\0';
	}
	else {
		puts("ArenaAllocator: Allocation failure!");
		exit(-2);
	}
	return re;
}

size_t ArenaAllocator::size() const {
	size_t result = 0;
	for(const auto& v : mData)
		result += v.size();
	return result;
}

size_t ArenaAllocator::wasted() const {
	return mWasted + mStackAllocator.remaining();
}
