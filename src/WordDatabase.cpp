#include "WordDatabase.hpp"

#include "rapidxml/rapidxml.hpp"

#include <cstring>
#include <algorithm>

#ifdef DEBUG
#	include <cstdio>
#endif

#include "Timer.hpp"
#include "Files.hpp"
#include "KanaConversion.hpp"

WordDatabase::WordDatabase() :
	mAllocator(1 << 16) // ~65KB blocks
{}

bool WordDatabase::load(const char *path) {
	Timer t;

	std::vector<char> data;
	if(!loadFileIntoVector(path, data)) {
		printf(" -> Failed loading word database.");
		return false;
	}
#ifdef DEBUG
	printf("Loading %s took %zums\n", path, t.milliseconds());
#endif // DEBUG
	t.start();

	rapidxml::xml_document<> doc;
	doc.parse<0>(data.data());
#ifdef DEBUG
	printf("Parsing %s took %zums\n", path, t.milliseconds());
#endif // DEBUG
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
#ifdef DEBUG
	printf("Converting data took %zums\n", t.milliseconds());
#endif // DEBUG

#ifdef DEBUG
	puts  ("Memory requirements: ");
	printf(" - Raw file:         %zu\n", data.size());
	printf(" - Database:         %zu (%zu words)\n", mWords.size() * sizeof(Word), mWords.size());
	printf(" - Raw word storage: %zu (%zu wasted = %f%%)\n", mAllocator.total(), mAllocator.wasted(), mAllocator.wasted() * 100.f / mAllocator.total());
	puts  (" - Total:");
	printf("   - Persistent:     %zu\n", mWords.size() * sizeof(Word) + mAllocator.total());
	printf("   - Parsing:        %zu (+ rapidxml data structure size)\n", mWords.size() * sizeof(Word) + mAllocator.total() + data.size());
#endif // debug

	return true;
}

static
bool testRelevance(const SearchState* state, const std::string& term, const char* test, bool lowercasify, float* pRelevance) {
	const char* m0 = test;
	const char* m  = test;

	while(*m) {
		if(*m == term[0]) {
			if(lowercasify) {
				for (size_t k = 0; k < term.size(); k++) {
					if(m[k] == '\0' || term[k] != tolower(m[k]))
						goto CONTINUE_OUTER;
				}
			}
			else {
				for (size_t k = 0; k < term.size(); k++) {
					if(m[k] == '\0' || term[k] != m[k])
						goto CONTINUE_OUTER;
				}
			}

			*pRelevance = 0;

			size_t m0len = strlen(m0);

			// Matching factor
			if(m == m0) { // Word at begin
				if(m0len == term.size())
					*pRelevance += state->relevance.matchExact;
				else
					*pRelevance += state->relevance.matchAtBegin;
			}
			else if(m == m0 + m0len - term.size()) {
				*pRelevance += state->relevance.matchSomewhere;
			}

			// TODO: Word relevance

			// Alphabetic factors
			*pRelevance += state->relevance.length / m0len;
			// TODO: actual alphabetic sorting

			return *pRelevance > state->relevance.cutoff;
		}
		CONTINUE_OUTER:
		m++;
	}

	return false;
}

static
float testWord(SearchState* state, const std::string& term, Word& w, uint32_t flags) {
	float max_relevance = 0;
	float relevance;

	if(flags & 1) {
		for(const char* m : w.mKanji) {
			if(testRelevance(state, term, m, false, &relevance) && (relevance > max_relevance)) {
				max_relevance = relevance;
			}
		}
	}

	if(flags & 2) {
		for(const char* m : w.mKana) {
			if(testRelevance(state, term, m, false, &relevance) && (relevance > max_relevance))
				max_relevance = relevance;
		}
	}

	if(flags & 4) {
		for (Sense& s : w.mSenses) {
			for(const char* m : s.mMeanings) {
				if(testRelevance(state, term, m, true, &relevance) && relevance > max_relevance)
					max_relevance = relevance;
			}
		}
	}

	return max_relevance;
}

void WordDatabase::search(SearchState* state, const std::string& term, uint32_t all_flags) {
	Timer t;

	state->mResults.clear();

	auto searchAll = [=](const std::string& term, uint32_t flags) {
		for(size_t i = 0; i < mWords.size(); i++) {
			float relevance = testWord(state, term, mWords[i], flags);
			if(relevance >= state->relevance.cutoff) {
				state->mResults.emplace_back(SearchResult {
					&mWords[i],
					relevance
				});
			}
		}
	};

	if(all_flags & (SEARCH_CONVERT_HIRAGANA | SEARCH_CONVERT_KATAKANA)) {
		std::string hiragana = Romaji2Hiragana(term);
		if(hiragana != term) {
			searchAll(hiragana, all_flags & ~SEARCH_MEANING);
			all_flags &= ~SEARCH_KANA_KANJI;
			printf("Also searching for '%s'\n", hiragana.c_str());
		}
	}

	if(all_flags) {
		searchAll(term, all_flags);
	}

	cleanResults(state);

	state->mTime = t.milliseconds();
}

void WordDatabase::cleanResults(SearchState* state) {
	// Removing duplicate results
	std::sort(state->mResults.begin(), state->mResults.end(),
		[](const auto& a, const auto& b) {
			if(a.word != b.word)
				return a.word < b.word;
			else
				return a.relevance < b.relevance;
		}
	);

	std::unique(state->mResults.begin(), state->mResults.end(),
		[](const auto& a, const auto& b) {
			return a.word == b.word;
		}
	);

	// Sorting for relevance
	std::stable_sort(state->mResults.begin(), state->mResults.end(),
		[](const auto& a, const auto& b) {
			return a.relevance < b.relevance;
		}
	);
}
