#include "Word.hpp"

#include <vector>
bool Word::parse(rapidxml::xml_node<>* node, ArenaAllocator& alloc) {
	std::vector<const char*> strings;

	{
		strings.clear();
		rapidxml::xml_node<>* k_ele = node->first_node("k_ele", 5, true);
		while(k_ele) {
			if(rapidxml::xml_node<>* keb = k_ele->first_node("keb", 3, true)) {
				strings.push_back(alloc.allocateString(keb->value(), keb->value_size()));
			}
			k_ele = k_ele->next_sibling("k_ele", 5, true);
		}

		mKanji = Strings(strings.data(), strings.size(), alloc);
	}

	{
		strings.clear();
		rapidxml::xml_node<>* r_ele = node->first_node("r_ele", 5, true);
		while(r_ele) {
			if(rapidxml::xml_node<>* reb = r_ele->first_node("reb", 3, true)) {
				strings.push_back(alloc.allocateString(reb->value(), reb->value_size()));
			}
			r_ele = r_ele->next_sibling("r_ele", 5, true);
		}

		mKana = Strings(strings.data(), strings.size(), alloc);
	}

	{
		std::vector<Sense> senses;
		rapidxml::xml_node<>* sense = node->first_node("sense", 5, true);
		while(sense) {
			senses.emplace_back();

			// Parsing glosses
			strings.clear();
			rapidxml::xml_node<>* gloss = sense->first_node("gloss", 5, true);
			while(gloss) {
				if(gloss->value_size()){
					strings.push_back(alloc.allocateString(gloss->value(), gloss->value_size()));
				}
				gloss = gloss->next_sibling("gloss", 5, true);
			}

			senses.back().mMeanings = Strings(strings.data(), strings.size(), alloc);

			// Parsing cross-references
			strings.clear();
			rapidxml::xml_node<>* xref = sense->first_node("xref", 4, true);
			while(xref) {
				if(xref->value_size()) {
					strings.push_back(alloc.allocateString(xref->value(), xref->value_size()));
				}
				xref = xref->next_sibling("xref", 4, true);
			}

			senses.back().mReferences = Strings(strings.data(), strings.size(), alloc);

			sense = sense->next_sibling("sense", 5, true);
		}

		mSenses = VectorView<Sense>(senses.data(), senses.size(), alloc);
	}

	return true;
}
