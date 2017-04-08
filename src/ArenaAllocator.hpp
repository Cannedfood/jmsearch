#pragma once

#include <vector>
#include <deque>
#include <cstddef>

class ArenaAllocator {
	size_t                        mWasted;
	size_t                        mBlockSize;
	std::deque<std::vector<char>> mData;

	struct StackAllocator {
		size_t            mUsed;
		std::vector<char> mBlock;

		StackAllocator(size_t size) :
			mUsed(0),
			mBlock(size)
		{}

		char* allocate(size_t n) {
			char* result = nullptr;
			if(mUsed + n < mBlock.size()) {
				result = mBlock.data() + mUsed;
				mUsed += n;
			}
			return result;
		}

		bool empty() const { return mUsed == 0; }
		size_t remaining() const { return mBlock.size() - mUsed; }
	} mStackAllocator;

public:
	ArenaAllocator(size_t block_size = 8192) :
		mWasted(0),
		mBlockSize(block_size),
		mStackAllocator(mBlockSize)
	{}

	char* allocate(size_t n);

	char* allocateString(const char* s, size_t len);

	size_t size() const;

	size_t wasted() const;
};
