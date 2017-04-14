#include "Word.hpp"

#include <cstring>

#include <vector>

#define XML_EACH(FROM, NAME, VAR) \
	rapidxml::xml_node<>* VAR = FROM->first_node(NAME, sizeof(NAME) - 1, true); \
	VAR != nullptr;\
	VAR = VAR->next_sibling(NAME, sizeof(NAME), true)

bool Word::parse(rapidxml::xml_node<>* node, ArenaAllocator& alloc) {
	std::vector<const char*> strings;
	std::vector<std::string> stringStore;

	{
		strings.clear();
		for(XML_EACH(node, "k_ele", k_ele)) {
			if(rapidxml::xml_node<>* keb = k_ele->first_node("keb", 3, true)) {
				strings.push_back(alloc.allocateString(keb->value(), keb->value_size()));
			}
		}

		mKanji = Strings(strings.data(), strings.size(), alloc);
	}

	{
		strings.clear();
		for(XML_EACH(node, "r_ele", r_ele)) {
			bool ok    = false;
			bool ik    = false;
			bool oik   = false;
			bool gikun = false;

			// TODO: add those ^ flags to the hiragana string

			for(XML_EACH(r_ele, "re_inf", re_inf)) {
				if(strncmp(re_inf->value(), "&ok;", re_inf->value_size()) == 0) {
					ok = true;
				}
				else if(strncmp(
					re_inf->value(), "&ik;",
					re_inf->value_size()) == 0
				) ik = true;
				else if(strncmp(
					re_inf->value(), "&gikun;",
					re_inf->value_size()) == 0
				) gikun = true;
				else if(strncmp(
					re_inf->value(), "&oik;",
					re_inf->value_size()) == 0
				) oik = true;
				else {
					fprintf(stderr, "Unknown re_inf attribute in xml: '");
					fwrite(re_inf->value(), re_inf->value_size(), 1, stderr);
					fprintf(stderr, "'\n");
				}
			}

			if(rapidxml::xml_node<>* reb = r_ele->first_node("reb", 3, true)) {
				strings.push_back(alloc.allocateString(reb->value(), reb->value_size()));
			}
		}

		mKana = Strings(strings.data(), strings.size(), alloc);
	}

	{
		std::vector<Sense> senses;
		std::vector<char>  flags;

		for(XML_EACH(node, "sense", sense)) {
			flags.clear();
			senses.emplace_back();

			// Parsing glosses
			strings.clear();
			for(XML_EACH(sense, "gloss", gloss)) {
				if(gloss->value_size()){
					strings.push_back(alloc.allocateString(gloss->value(), gloss->value_size()));
				}
			}

			senses.back().mMeanings = Strings(strings.data(), strings.size(), alloc);

			// pos "part-of-speech" flags
			for(XML_EACH(sense, "pos", pos)) {
				// TODO: handle pos flags
			}

			// Parsing cross-references
			strings.clear();

			for(XML_EACH(sense, "xref", xref)) {
				if(xref->value_size()) {
					strings.push_back(alloc.allocateString(xref->value(), xref->value_size()));
				}
			}

			senses.back().mReferences = Strings(strings.data(), strings.size(), alloc);
		}

		mSenses = VectorView<Sense>(senses.data(), senses.size(), alloc);
	}

	return true;
}
