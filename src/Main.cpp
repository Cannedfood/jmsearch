#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

#include "Timer.hpp"
#include "ArenaAllocator.hpp"
#include "Word.hpp"
#include "WordDatabase.hpp"

#include <iostream>
#include <algorithm>

int main(int argc, char** argv) {
	WordDatabase database;
	if(!database.load("JMdict_e.xml"))
		return -1;

	SearchState search;
	while (true) {
		search.mTerm.clear();
		while(search.mTerm.empty()) {
			puts("Enter a search term (or ~ to exit):");
			std::cin >> search.mTerm;
		}

		if(search.mTerm == "~") break;

		if(search.mTerm.front() != '.') {
			search.mTerm = WordDatabase::Romaji2Hiragana(search.mTerm).c_str();
			database.searchKana(&search);
		}
		else {
			search.mTerm.erase(search.mTerm.begin());
			database.searchMeaning(&search);
		}

		database.sortResults(&search);

		// std::reverse(search.mResults.begin(), search.mResults.end());
		for(const SearchResult& result : search.mResults) {
			Word* w = result.word;

			printf(" - ");

			if(w->mKanji.size()) {
				printf("%s", w->mKanji[0]);
				for (size_t i = 1; i < w->mKanji.size() && w->mKanji[i]; i++)
					printf(" / %s", w->mKanji[i]);
				printf("\n   ");
			}

			if(w->mKana.size()) {
				printf("%s", w->mKana[0]);
				for (size_t i = 1; i < w->mKana.size() && w->mKana[i]; i++)
					printf(" / %s", w->mKana[i]);
				printf("\n");
			}

			for(const Sense& s : w->mSenses) {
				bool first = true;
				for(const char* m : s.mMeanings) {
					if(first) {
						printf("   - %s\n", m);
						first = false;
					}
					else {
						printf("     %s\n", m);
					}
				}
			}
		}

		printf(" [ %zu results for \"%s\" in %zums ]\n", search.mResults.size(), search.mTerm.c_str(), search.mTime);
		search.mResults.clear();
	}
}
