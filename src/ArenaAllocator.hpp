#pragma once

#include <memory>
#include <deque>
#include <cstddef>

class ArenaAllocator {
	size_t                        mSize;
	size_t                        mWasted;
	size_t                        mBlockSize;
	std::deque<std::unique_ptr<char[]>> mData;

	struct StackAllocator {
		size_t                  mUsed;
		size_t                  mSize;
		std::unique_ptr<char[]> mBlock;

		StackAllocator(size_t size) :
			mUsed(0),
			mSize(size),
			mBlock(new char[size])
		{}

		char* allocate(size_t n) {
			if(n == 0) return nullptr;

			char* result = nullptr;
			if(mUsed + n < mSize) { // TODO: can this be <= ?
				result = mBlock.get() + mUsed;
				mUsed += n;
			}
			return result;
		}

		bool empty() const { return mUsed == 0; }
		size_t remaining() const { return mSize - mUsed; }
	} mStackAllocator;

public:
	ArenaAllocator(ArenaAllocator& other) = delete;

	ArenaAllocator(size_t block_size = 8192) :
		mSize(block_size),
		mWasted(0),
		mBlockSize(block_size),
		mStackAllocator(mBlockSize)
	{}

	void* allocate(size_t n);

	char* allocateString(const char* s, size_t len);

	size_t size() const;

	size_t wasted() const;
};
