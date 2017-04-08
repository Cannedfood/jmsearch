#pragma once

#include "rapidxml/rapidxml.hpp"
#include "ArenaAllocator.hpp"
#include <bitset>

struct Word {
	const char* mKanji[4];
	const char* mKana[4];
	const char* mMeaning[8];

	bool parse(rapidxml::xml_node<>* node, ArenaAllocator& alloc);
};
