#pragma once

#include <string>
#include <cstdint>

#include "Word.hpp"

#include <vector>

struct SearchResult {
	Word* word;
	float relevance;
};

struct SearchState {
private:
	std::string               mLastTerm;
	std::vector<SearchResult> mResults;
	size_t                    mTime;

	friend class WordDatabase;
public:
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
	} relevance;

	const std::vector<SearchResult>& results()  const { return mResults; }
	size_t                           time()     const { return mTime; }
	const std::string&               lastTerm() const { return mLastTerm; }
};

class WordDatabase {
	ArenaAllocator        mAllocator;
	std::deque <Word>     mWords;
	std::vector<uint32_t> mCandidates;

	void singleSearch(SearchState* state, uint32_t flags);
public:
	WordDatabase();
	WordDatabase(const WordDatabase&) = delete;
	WordDatabase(WordDatabase&&)      = delete;

	bool load(const char* path, const char* locale = nullptr);

	enum SearchFlags {
		SEARCH_KANJI            = 1,
		SEARCH_KANA             = 2,
		SEARCH_MEANING          = 4,
		SEARCH_CONVERT_HIRAGANA = 8,
		SEARCH_CONVERT_KATAKANA = 16,
		SEARCH_RESULTS          = 32,

		SEARCH_KANA_KANJI       = SEARCH_KANA | SEARCH_KANJI,
		SEARCH_KANA_KANJI_CNVT  = SEARCH_KANA | SEARCH_KANJI | SEARCH_CONVERT_HIRAGANA | SEARCH_CONVERT_KATAKANA,
	};

	void search      (SearchState* state, const std::string& term, uint32_t flags);
	void cleanResults(SearchState* state);
};
