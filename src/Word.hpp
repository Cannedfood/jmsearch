#pragma once

#include "rapidxml/rapidxml.hpp"
#include "ArenaAllocator.hpp"
#include <bitset>

struct Word {
	const char* mKanji[8];
	const char* mKana[8];
	const char* mMeaning[8];

	bool parse(rapidxml::xml_node<>* node, ArenaAllocator& alloc);
};
