#include "WordDatabase.hpp"

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

#include <cstdio>
#include <cstring>
#include <algorithm>

#include "Timer.hpp"

WordDatabase::WordDatabase() :
	mAllocator(1 << 14)
{}

bool WordDatabase::load(const char *path) {
	Timer t;
	rapidxml::file<> file(path);
	printf("Loading %s took %zums\n", path, t.milliseconds());
	t.start();

	rapidxml::xml_document<> doc;
	doc.parse<0>(file.data());
	printf("Parsing %s took %zums\n", path, t.milliseconds());
	t.start();

	rapidxml::xml_node<>* root = doc.first_node("JMdict");
	if(!root) {
		fprintf(stderr, "Failed loading dictionary from '%s'", path);
		return false;
	}

	rapidxml::xml_node<>* node = root->first_node("entry", 5, true);
	while(node) {
		Word w;
		if(w.parse(node, mAllocator))
			mWords.emplace_back(std::move(w));
		node = node->next_sibling("entry", 5, true);
	}
	printf("Converting data took %zums\n", t.milliseconds());

	puts  ("Memory requirements: ");
	printf(" - Raw file:         %zu\n", file.size());
	printf(" - Database:         %zu (%zu words)\n", mWords.size() * sizeof(Word), mWords.size());
	printf(" - Raw word storage: %zu (%zu wasted = %f%%)\n", mAllocator.size(), mAllocator.wasted(), mAllocator.wasted() * 100.f / mAllocator.size());
	puts  (" - Total:");
	printf("   - Persistent:     %zu\n", mWords.size() * sizeof(Word) + mAllocator.size());
	printf("   - Parsing:        %zu (+ rapidxml data structure size)\n", mWords.size() * sizeof(Word) + mAllocator.size() + file.size());

	return true;
}

static
bool testRelevance(SearchState* state, const char* test, bool lowercasify, float* pRelevance) {
	const char* m0 = test;
	const char* m  = test;

	while(*m) {
		if(*m == state->mTerm[0]) {
			if(lowercasify) {
				for (size_t k = 0; k < state->mTerm.size(); k++) {
					if(m[k] == '\0' || state->mTerm[k] != tolower(m[k]))
						goto CONTINUE_OUTER;
				}
			}
			else {
				for (size_t k = 0; k < state->mTerm.size(); k++) {
					if(m[k] == '\0' || state->mTerm[k] != m[k])
						goto CONTINUE_OUTER;
				}
			}

			*pRelevance = 0;

			size_t m0len = strlen(m0);

			// Matching factor
			if(m == m0) { // Word at begin
				if(m0len == state->mTerm.size())
					*pRelevance += state->mRelevance.matchExact;
				else
					*pRelevance += state->mRelevance.matchAtBegin;
			}
			else if(m == m0 + m0len - state->mTerm.size()) {
				*pRelevance += state->mRelevance.matchSomewhere;
			}

			// TODO: Word relevance

			// Alphabetic factors
			*pRelevance += state->mRelevance.length / m0len;
			// TODO: actual alphabetic sorting

			return *pRelevance > state->mRelevance.cutoff;
		}
		CONTINUE_OUTER:
		m++;
	}

	return false;
}

void WordDatabase::searchMeaning(SearchState* state) {
	Timer t;

	float relevance;

	const std::string& s = state->mTerm;
	for (size_t i = 0; i < mWords.size(); i++) {
		for (const char* m : mWords[i].mMeaning) {
			if(!m) break;
			if(testRelevance(state, m, true, &relevance)) {
				state->mResults.emplace_back(SearchResult {
					&mWords[i],
					relevance
				});
			}
		}
	}

	state->mTime = t.milliseconds();
}

void WordDatabase::searchKana(SearchState* state) {
	Timer t;

	float relevance;

	const std::string& s = state->mTerm;
	for (size_t i = 0; i < mWords.size(); i++) {
		for (const char* m : mWords[i].mKana) {
			if(!m) break;
			if(testRelevance(state, m, false, &relevance)) {
				state->mResults.emplace_back(SearchResult {
					&mWords[i],
					relevance
				});
			}
		}
	}

	state->mTime = t.milliseconds();
}

void WordDatabase::sortResults(SearchState* state) {
	// Using stable sort because we will presort the list of words
	std::stable_sort(state->mResults.begin(), state->mResults.end(), [](const auto& a, const auto& b) {
		return a.relevance < b.relevance;
	});
}



std::string WordDatabase::Romaji2Hiragana(std::string s) {
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);

	std::string result;

	size_t i = 0;
	while(i < s.size()) {
		switch (s[i]) {
			case 'a': result += u8"あ"; i += 1; break;
			case 'i': result += u8"い"; i += 1; break;
			case 'u': result += u8"う"; i += 1; break;
			case 'e': result += u8"え"; i += 1; break;
			case 'o': result += u8"お"; i += 1; break;
			case 'k':
			switch (s[i + 1]) {
				case 'k': result += u8"っ"; i += 1; break;
				case 'a': result += u8"か"; i += 2; break;
				case 'i': result += u8"き"; i += 2; break;
				case 'u': result += u8"く"; i += 2; break;
				case 'e': result += u8"け"; i += 2; break;
				case 'o': result += u8"こ"; i += 2; break;
				case 'y': if(i + 2 < s.size()) {
					switch (s[i + 2]) {
						case 'a': result += u8"きゃ"; i += 3; break;
						case 'u': result += u8"きゅ"; i += 3; break;
						case 'o': result += u8"きょ"; i += 3; break;
						default: i += 1; break;
					}
				} break;
				default: i += 1; break;
			} break;
			case 's':
			switch (s[i + 1]) {
				case 's': result += u8"っ"; i += 1; break;
				case 'a': result += u8"さ"; i += 2; break;
				case 'u': result += u8"す"; i += 2; break;
				case 'e': result += u8"せ"; i += 2; break;
				case 'o': result += u8"そ"; i += 2; break;
				case 'h': if(i + 2 < s.size()) {
					switch (s[i + 2]) {
						case 'a': result += u8"しゃ"; i += 3; break;
						case 'u': result += u8"しゅ"; i += 3; break;
						case 'o': result += u8"しょ"; i += 3; break;
						case 'i': result += u8"し";   i += 3; break;
						default: i += 1; break;
					}
				} break;
				default: i += 1; break;
			} break;
			case 't':
			switch (s[i + 1]) {
				case 't': result += u8"っ"; i += 1; break;
				case 'a': result += u8"た"; i += 2; break;
				case 's': result += u8"つ"; i += 3; break;
				case 'e': result += u8"て"; i += 2; break;
				case 'o': result += u8"と"; i += 2; break;
				default: i += 1; break;
			} break;
			case 'n':
			if(i + 1 >= s.size()) {
				result += u8"ん"; i += 1; break;
			}
			switch (s[i + 1]) {
				case 'a': result += u8"な"; i += 2; break;
				case 'i': result += u8"に"; i += 2; break;
				case 'u': result += u8"ぬ"; i += 2; break;
				case 'e': result += u8"ね"; i += 2; break;
				case 'o': result += u8"の"; i += 2; break;
				case 'y': if(i + 2 < s.size()) {
					switch (s[i + 2]) {
						case 'a': result += u8"にゃ"; i += 3; break;
						case 'u': result += u8"にゅ"; i += 3; break;
						case 'o': result += u8"にょ"; i += 3; break;
						default: i += 1; break;
					}
				default: result += u8"ん"; i += 1; break;
				} break;
			} break;
			case 'h':
			switch (s[i + 1]) {
				case 'a': result += u8"は"; i += 2; break;
				case 'i': result += u8"ひ"; i += 2; break;
				case 'e': result += u8"へ"; i += 2; break;
				case 'o': result += u8"ほ"; i += 2; break;
				case 'y': if(i + 2 < s.size()) {
					switch (s[i + 2]) {
						case 'a': result += u8"ひゃ"; i += 3; break;
						case 'u': result += u8"ひゅ"; i += 3; break;
						case 'o': result += u8"ひょ"; i += 3; break;
						default: i += 1; break;
					}
				default: i += 1; break;
				} break;
			} break;
			case 'm':
			switch (s[i + 1]) {
				case 'a': result += u8"ま"; i += 2; break;
				case 'i': result += u8"み"; i += 2; break;
				case 'u': result += u8"む"; i += 2; break;
				case 'e': result += u8"め"; i += 2; break;
				case 'o': result += u8"も"; i += 2; break;
				case 'y': if(i + 2 < s.size()) {
					switch (s[i + 2]) {
						case 'a': result += u8"みゃ"; i += 3; break;
						case 'u': result += u8"みゅ"; i += 3; break;
						case 'o': result += u8"みょ"; i += 3; break;
						default: i += 1; break;
					}
				default: i += 1; break;
				} break;
			} break;
			case 'y':
			switch (s[i + 1]) {
				case 'a': result += u8"や"; i += 2; break;
				case 'u': result += u8"ゆ"; i += 2; break;
				case 'o': result += u8"よ"; i += 2; break;
				default: i += 1; break;
			} break;
			case 'r':
			switch (s[i + 1]) {
				case 'a': result += u8"ら"; i += 2; break;
				case 'i': result += u8"り"; i += 2; break;
				case 'u': result += u8"る"; i += 2; break;
				case 'e': result += u8"れ"; i += 2; break;
				case 'o': result += u8"ろ"; i += 2; break;
				case 'y': if(i + 2 < s.size()) {
					switch (s[i + 2]) {
						case 'a': result += u8"りゃ"; i += 3; break;
						case 'u': result += u8"りゅ"; i += 3; break;
						case 'o': result += u8"りょ"; i += 3; break;
						default: i += 1; break;
					}
				}
				else
					i += 1;
				break;
				default: i += 1; break;
			} break;
			case 'w':
			switch (s[i + 1]) {
				case 'a': result += u8"わ"; i += 2; break;
				case 'i': result += u8"ゐ"; i += 2; break;
				case 'e': result += u8"ゑ"; i += 2; break;
				case 'o': result += u8"を"; i += 2; break;
				default: i += 1; break;
			} break;
			case 'g':
			switch (s[i + 1]) {
				case 'a': result += u8"が"; i += 2; break;
				case 'i': result += u8"ぎ"; i += 2; break;
				case 'u': result += u8"ぐ"; i += 2; break;
				case 'e': result += u8"げ"; i += 2; break;
				case 'o': result += u8"ご"; i += 2; break;
				case 'y': if(i + 2 < s.size()) {
					switch (s[i + 2]) {
						case 'a': result += u8"ぎゃ"; i += 3; break;
						case 'u': result += u8"ぎゅ"; i += 3; break;
						case 'o': result += u8"ぎょ"; i += 3; break;
						default: i += 1; break;
					}
				default: i += 1; break;
				} break;
			} break;
			case 'z':
			switch (s[i + 1]) {
				case 'a': result += u8"ざ"; i += 2; break;
				case 'i': result += u8"じ"; i += 2; break;
				case 'u': result += u8"ず"; i += 2; break;
				case 'e': result += u8"ぜ"; i += 2; break;
				case 'o': result += u8"ぞ"; i += 2; break;
				default: i += 1; break;
			} break;
			case 'd':
			switch (s[i + 1]) {
				case 'a': result += u8"だ"; i += 2; break;
				case 'j': result += u8"ぢ"; i += 3; break;
				case 'u': result += u8"づ"; i += 2; break;
				case 'z': result += u8"づ"; i += 3; break;
				case 'e': result += u8"で"; i += 2; break;
				case 'o': result += u8"ど"; i += 2; break;
				default: i += 1; break;
			} break;
			case 'b':
			switch (s[i + 1]) {
				case 'a': result += u8"ば"; i += 2; break;
				case 'i': result += u8"び"; i += 2; break;
				case 'u': result += u8"ぶ"; i += 2; break;
				case 'e': result += u8"べ"; i += 2; break;
				case 'o': result += u8"ぼ"; i += 2; break;
				case 'y': if(i + 2 < s.size()) {
					switch (s[i + 2]) {
						case 'a': result += u8"びゃ"; i += 3; break;
						case 'u': result += u8"びゅ"; i += 3; break;
						case 'o': result += u8"びょ"; i += 3; break;
						default: i += 1; break;
					}
				default: i += 1; break;
				} break;
			} break;
			case 'p':
			switch (s[i + 1]) {
				case 'p': result += u8"っ"; i += 1; break;
				case 'a': result += u8"ぱ"; i += 2; break;
				case 'i': result += u8"ぴ"; i += 2; break;
				case 'u': result += u8"ぷ"; i += 2; break;
				case 'e': result += u8"ぺ"; i += 2; break;
				case 'o': result += u8"ぽ"; i += 2; break;
				case 'y': if(i + 2 < s.size()) {
					switch (s[i + 2]) {
						case 'a': result += u8"ぴゃ"; i += 3; break;
						case 'u': result += u8"ぴゅ"; i += 3; break;
						case 'o': result += u8"ぴょ"; i += 3; break;
						default: i += 1; break;
					}
				default: i += 1; break;
				} break;
			} break;
			case 'j': if(i + 1 <= s.size()) {
				switch (s[i + 1]) {
					case 'a': result += u8"じゃ"; i += 2; break;
					case 'u': result += u8"じゅ"; i += 2; break;
					case 'o': result += u8"じょ"; i += 2; break;
					case 'i': result += u8"じ"; i += 2; break;
					default: i += 1; break;
				}
			} else i++;
			break;
			case 'c': if(i + 2 <= s.size() && s[i + 1] == 'h') {
				switch (s[i + 2]) {
					case 'c': result += u8"っ"; i += 1; break;
					case 'a': result += u8"ちゃ"; i += 3; break;
					case 'u': result += u8"ちゅ"; i += 3; break;
					case 'o': result += u8"ちょ"; i += 3; break;
					case 'i': result += u8"ち"; i += 3; break;
					default: i += 1; break;
				}
			} else i++;
			break;
			default: result += s[i]; i += 1; break;
		}
		// printf("-> %s\n", result.c_str());
	}

	return result;
}

#include <stdexcept>
std::string WordDatabase::Romaji2Katakana(std::string s) {
	// TODO: Copy Romaji2Hiragana and replace Hiragana with Katakana

	throw std::runtime_error("Tried to use the unimplemented function WordDatabase::Romaji2Katakana");

	return "";
}
