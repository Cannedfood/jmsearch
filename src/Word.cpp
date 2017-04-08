#include "Word.hpp"

extern "C" {
#	include <memory.h>
}


template<typename T, size_t len>
T** findEmpty(T* (&t)[len]) {
	for (size_t i = 0; i < len; i++) {
		if(t[i] == nullptr)
			return &t[i];
	}
	return nullptr;
}

bool Word::parse(rapidxml::xml_node<> *node, ArenaAllocator& alloc) {
	memset(mKanji,    0, sizeof(mKanji));
	memset(mKana,     0, sizeof(mKana));
	memset(mMeaning,  0, sizeof(mMeaning));

	if(rapidxml::xml_node<>* k_ele = node->first_node("k_ele", 5, true)) {
		if(rapidxml::xml_node<>* keb = k_ele->first_node("keb", 3, true)) {
			if(const char** s = findEmpty(mKanji)) {
				*s = alloc.allocateString(keb->value(), keb->value_size());
			}
		}
	}

	if(rapidxml::xml_node<>* r_ele = node->first_node("r_ele", 5, true)) {
		if(rapidxml::xml_node<>* reb = r_ele->first_node("reb", 3, true)) {
			if(const char** s = findEmpty(mKana)) {
				*s = alloc.allocateString(reb->value(), reb->value_size());
			}
		}
	}

	if(rapidxml::xml_node<>* sense = node->first_node("sense", 5, true)) {
		if(rapidxml::xml_node<>* gloss = sense->first_node("gloss", 5, true)) {
			if(const char** s = findEmpty(mMeaning)) {
				*s = alloc.allocateString(gloss->value(), gloss->value_size());
			}
		}
	}

	return true;
}
