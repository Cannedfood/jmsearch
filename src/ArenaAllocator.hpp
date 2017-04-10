#pragma once

#include <memory>
#include <deque>
#include <cstddef>

class ArenaAllocator {
	size_t mTotal;
	size_t mWasted;

	std::deque<std::unique_ptr<char[]>> mData;

	size_t mBlockSize;

	std::unique_ptr<char[]> mStack;
	size_t                  mStackSize;
	size_t                  mStackUsed;

	void startNewStack(size_t minSize);
public:
	ArenaAllocator(size_t block_size = 8192);
	ArenaAllocator(const ArenaAllocator& other) = delete;

	void* allocate(size_t n);
	char* allocateString(const char* s, size_t len);

	size_t total()  const { return mTotal; }
	size_t wasted() const { return mWasted + mStackSize - mStackUsed; }
};
