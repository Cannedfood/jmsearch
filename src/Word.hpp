#pragma once

#include "rapidxml/rapidxml.hpp"
#include "ArenaAllocator.hpp"
#include "WordFlags.hpp"

class Strings {
private:
	size_t       mLength;
	const char** mPointers;

public:
	Strings();
	Strings(const char** data, size_t length);
	Strings(const char** data, size_t length, ArenaAllocator& alloc);

	inline size_t size()                    const { return mLength; }
	inline size_t length()                  const { return mLength; }
	inline const char* operator[](size_t i) const { return mPointers[i]; }

	using  iterator = const char**;
	inline iterator begin() const { return mPointers; }
	inline iterator end  () const { return mPointers + mLength; }
};

struct Word {
	Strings mKanji;
	Strings mKana;
	Strings mMeaning;

	bool parse(rapidxml::xml_node<>* node, ArenaAllocator& alloc);
};
