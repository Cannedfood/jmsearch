#pragma once

#include "rapidxml/rapidxml.hpp"
#include "ArenaAllocator.hpp"

#include <cassert>

template<typename T>
class VectorView {
	size_t mLength;
	T*     mData;

public:
	VectorView() :
		mLength(0),
		mData(nullptr)
	{}

	VectorView(T* data, size_t length) :
		mLength(length),
		mData(data)
	{}

	VectorView(T* data, size_t length, ArenaAllocator& alloc) :
		mLength(length),
		mData((T*) alloc.allocate(length * sizeof(T)))
	{
		std::copy(data, data + length, mData);
	}

	inline size_t size()                    const { return mLength; }
	inline size_t length()                  const { return mLength; }
	inline const char* operator[](size_t i) const {
		assert(i < mLength);
		return mData[i];
	}

	using  iterator = T*;
	inline iterator begin() const { return mData; }
	inline iterator end  () const { return mData + mLength; }
};

using Strings = VectorView<const char*>;

struct Sense {
	Strings     mReferences;
	Strings     mMeanings;
	const char* mFlags;
};

struct Word {
	Strings           mKanji;
	Strings           mKana;
	VectorView<Sense> mSenses;

	bool parse(rapidxml::xml_node<>* node, ArenaAllocator& alloc);
};
