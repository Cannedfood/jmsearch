#pragma once

#include <string>
#include <cstdint>
#include <set>

#include "Word.hpp"

struct SearchResult {
	Word* word;
	float relevance;
};

struct SearchState {
	struct {
		// Matching factors (mutually exclusive)
		float matchExact     = 100; // That the word is exactly what you searched for
		float matchAtBegin   = 80; // The word is contained at the beginning
		float matchAtEnd     = 70; // The word is contained at the end
		float matchSomewhere = 60; // The word is contained somewhere

		// General relevance factors
		float ichi1 = 30;
		float ichi2 = 25;

		// Alphabetic factors
		float alphabetic = 10;
		float length     = 10;

		// Cuttof (How much relevance it at least needed, doesn't do anything when <= matchSomewhere)
		float cutoff     = 60;
	} mRelevance;

	std::string               mTerm;
	std::vector<SearchResult> mResults;
	size_t                    mTime;
};

class WordDatabase {
	ArenaAllocator        mAllocator;
	std::deque <Word>     mWords;
	std::vector<uint32_t> mCandidates;

public:
	WordDatabase();

	static std::string Romaji2Hiragana(std::string s);
	static std::string Romaji2Katakana(std::string s);

	bool load(const char* path);
	void searchMeaning(SearchState* state);
	void searchKana   (SearchState* state);
	void searchKanji  (SearchState* state);
	void sortResults  (SearchState* state);
};
