#include "Word.hpp"

Strings::Strings() :
	mPointers(nullptr),
	mLength(0)
{}

Strings::Strings(const char** data, size_t length) :
	mPointers(data),
	mLength(length)
{}

Strings::Strings(const char** data, size_t length, ArenaAllocator& alloc) {
	const char** my_data = (const char**) alloc.allocate(sizeof(const char*) * length);
	std::copy(data, data + length, my_data);
	mPointers = my_data;
	mLength = length;
}

bool Word::parse(rapidxml::xml_node<>* node, ArenaAllocator& alloc) {
	std::vector<const char*> strings;

	{
		rapidxml::xml_node<>* k_ele = node->first_node("k_ele", 5, true);
		while(k_ele) {
			if(rapidxml::xml_node<>* keb = k_ele->first_node("keb", 3, true)) {
				strings.push_back(alloc.allocateString(keb->value(), keb->value_size()));
			}
			k_ele = k_ele->next_sibling("k_ele", 5, true);
		}

		mKanji = Strings(strings.data(), strings.size(), alloc);
		strings.clear();
	}

	{
		rapidxml::xml_node<>* r_ele = node->first_node("r_ele", 5, true);
		while(r_ele) {
			if(rapidxml::xml_node<>* reb = r_ele->first_node("reb", 3, true)) {
				strings.push_back(alloc.allocateString(reb->value(), reb->value_size()));
			}
			r_ele = r_ele->next_sibling("r_ele", 5, true);
		}

		mKana = Strings(strings.data(), strings.size(), alloc);
		strings.clear();
	}

	{
		rapidxml::xml_node<>* sense = node->first_node("sense", 5, true);
		while(sense) {
			rapidxml::xml_node<>* gloss = sense->first_node("gloss", 5, true);
			while(gloss) {
				strings.push_back(alloc.allocateString(gloss->value(), gloss->value_size()));
				gloss = gloss->next_sibling("gloss", 5, true);
			}
			sense = sense->next_sibling("sense", 5, true);
		}

		mMeaning = Strings(strings.data(), strings.size(), alloc);
		strings.clear();
	}

	return true;
}
