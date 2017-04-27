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

#ifdef __unix__

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

std::string system_res_dir() {
	return "/usr/share/jmsearch/";
}

std::string user_res_dir() {
	const char* base;
	if((base = getenv("HOME")) == NULL) {
		base = getpwuid(getuid())->pw_dir;
	}

	return std::string(base) + "/.local/share/jmsearch/";
}

#else // defined(__unix__)
// TODO: implement for windows
#	error "Not implemented for Windows"
#endif

int main(int argc, char** argv) {
	WordDatabase database;

	{
		const char* path;

		if(
			std::ifstream("JMdict_e.gz").good() ||
			std::ifstream("JMdict_e.xml").good()
		) {
			path = "JMdict_e";
		}
		else if(
			std::ifstream(user_res_dir() + "JMdict_e.xml").good() ||
			std::ifstream(user_res_dir() + "JMdict_e.gz").good()
		) {
			path = ".local/share/jmsearch/JMdict_e";
		}
		else if(
			std::ifstream(system_res_dir() + "JMdict_e.xml").good() ||
			std::ifstream(system_res_dir() + "JMdict_e.gz").good()
		) {
			path = "/usr/share/jmsearch/JMdict_e";
		}
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
			puts("Enter a search term: (? for help)");
			std::getline(std::cin, search.mTerm);
		}

		if(search.mTerm == "~") break;

		if(search.mTerm == "?") {
			puts("  - Type a word to search (in kanji, kana and meanings).");
			puts("  - Write the number of a result to get more info. (Currently not actually more)");
			puts("  - Write ~ to exit (Or use ctrl-C)");
			continue;
		}

		if(search.mTerm.front() >= '0' && search.mTerm.front() <= '9') {
			size_t idx = strtoull(search.mTerm.c_str(), nullptr, 10) - 1;
			if(idx >= search.mResults.size()) {
				printf("Search index %zu does not exist.\n", idx);
			}
			else {
				printDetailed(search.mResults[idx].word);
			}
			continue;
		}

		search.mResults.clear();

		database.search(&search, WordDatabase::SEARCH_KANA_KANJI_CNVT | WordDatabase::SEARCH_MEANING);

		size_t index = 1;
		// std::reverse(search.mResults.begin(), search.mResults.end());
		for(const SearchResult& result : search.mResults) {
			printResult(search.mResults.size(), index++, result.word);
		}

		printf(" [ %zu results for \"%s\" in %zums ]\n", search.mResults.size(), search.mTerm.c_str(), search.mTime);
	}
}
