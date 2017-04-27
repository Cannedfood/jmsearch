#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"

#include "Timer.hpp"
#include "ArenaAllocator.hpp"
#include "Word.hpp"
#include "WordDatabase.hpp"

#include <iostream>
#include <algorithm>

static
void printResult(size_t num_results, size_t index, Word* w) {
	printf("%zu. ", index);

	if(w->mKanji.size()) {
		printf("%s", w->mKanji[0]);
		for (size_t i = 1; i < w->mKanji.size() && w->mKanji[i]; i++)
			printf(" / %s", w->mKanji[i]);
		printf("\n   ");
	}

	if(w->mKana.size()) {
		printf("%s", w->mKana[0]);
		for (size_t i = 1; i < w->mKana.size() && w->mKana[i]; i++) {
			printf(" / %s", w->mKana[i]);
		}
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

static
void printDetailed(Word* w) {
	if(w->mKanji.size()) {
		printf("%s", w->mKanji[0]);
		for (size_t i = 1; i < w->mKanji.size() && w->mKanji[i]; i++)
			printf(" / %s", w->mKanji[i]);
		printf("\n");
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
				printf("- %s\n", m);
				first = false;
			}
			else {
				printf("  %s\n", m);
			}
		}
	}
}

int main(int argc, char** argv) {
	WordDatabase database;

	{
		const char* path;

		if(std::ifstream("JMdict_e.xml").good())
			path = "JMdict_e.xml";
		else if(std::ifstream("/usr/share/jmsearch/JMdict_e.xml").good())
			path = "/usr/share/jmsearch/JMdict_e.xml";
		else {
			puts("Could not find JMdict_e.xml!");
			exit(-1);
		}
		if(!database.load(path))
			return -1;
	}

	SearchState search;
	while (true) {
		search.mTerm.clear();
		while(search.mTerm.empty()) {
			if(!std::cin.good()) exit(0);
			puts("Enter a search term, ? for help, or ~ to exit:");
			std::cin >> search.mTerm;
		}

		if(search.mTerm == "~") break;

		if(search.mTerm == "?") {
			puts("  Japanese -> English search: Just write a word (romaji or kana currently)");
			puts("  English  -> Japanese search: Write a word prefixed with . e.g. .tree");
			puts("  More information about a result: write the number of the result.");
			puts("  Write ~ to exit (Or use ctrl-C)");
			continue;
		}

		if(search.mTerm.front() >= '0' && search.mTerm.front() <= '9') {
			size_t idx = strtoull(search.mTerm.c_str(), nullptr, 10) - 1;
			if(idx >= search.mResults.size()) {
				printf("Search term index %zu does not exist.\n", idx);
			}
			else {
				printDetailed(search.mResults[idx].word);
			}
			continue;
		}

		search.mResults.clear();

		if(search.mTerm.front() == '.') {
			search.mTerm.erase(search.mTerm.begin());
			database.search(&search, WordDatabase::SEARCH_MEANING);
		}
		else {
			search.mTerm = WordDatabase::Romaji2Hiragana(search.mTerm).c_str();
			database.search(&search, WordDatabase::SEARCH_KANA_KANJI);
		}

		size_t index = 1;
		// std::reverse(search.mResults.begin(), search.mResults.end());
		for(const SearchResult& result : search.mResults) {
			printResult(search.mResults.size(), index++, result.word);
		}

		printf(" [ %zu results for \"%s\" in %zums ]\n", search.mResults.size(), search.mTerm.c_str(), search.mTime);
	}
}
