#pragma once

#include <string>
#include <cstdint>
#include <set>

#include "Word.hpp"

struct SearchState {
	struct {
		float ichi1 = 1;
		float ichi2 = .5f;
	} mPreferences;

	std::string         mTerm;
	std::string         mOldTerm;
	int                 mMode;
	std::vector<Word*>  mResults;
	std::set<size_t>    mResultIndices;
	size_t              mTime;
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
	void searchMeaning (SearchState* state);
	void searchKana    (SearchState* state);
	void searchKanji   (SearchState* state);
	void resolveResults(SearchState* state);
};
