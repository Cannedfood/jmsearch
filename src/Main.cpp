#include "WordDatabase.hpp"

#include <iostream>
#include <algorithm>
#include <fstream>

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
			std::ifstream("JMdict.gz").good() ||
			std::ifstream("JMdict.xml").good()
		) {
			path = "JMdict";
		}
		else if(
			std::ifstream(user_res_dir() + "JMdict.xml").good() ||
			std::ifstream(user_res_dir() + "JMdict.gz").good()
		) {
			path = ".local/share/jmsearch/JMdict";
		}
		else if(
			std::ifstream(system_res_dir() + "JMdict.xml").good() ||
			std::ifstream(system_res_dir() + "JMdict.gz").good()
		) {
			path = "/usr/share/jmsearch/JMdict";
		}
		else {
			puts("Could not find JMdict.xml or JMdict.gz!");
			exit(-1);
		}

		const char* locale = nullptr;
		if(argc > 1) locale = argv[1];
		if(!database.load(path, locale))
			return -1;
	}

	SearchState search;
	uint32_t    flags =
		WordDatabase::SEARCH_CONVERT_HIRAGANA |
		WordDatabase::SEARCH_KANA |
		WordDatabase::SEARCH_KANJI |
		WordDatabase::SEARCH_MEANING;

	while (true) {
		std::string term;

		while(term.empty()) {
			if(!std::cin.good()) exit(0);

			std::string flagstring;
			if(flags & WordDatabase::SEARCH_KANJI)            flagstring += 'k';
			if(flags & WordDatabase::SEARCH_KANA)             flagstring += 'r';
			if(flags & WordDatabase::SEARCH_MEANING)          flagstring += 'm';
			if(flags & WordDatabase::SEARCH_CONVERT_HIRAGANA) flagstring += 'c';
			printf("Enter a search term: (? for help) [+%s]\n", flagstring.c_str());
			std::getline(std::cin, term);
		}

		if(term == "?") {
#ifdef _WIN32
			puts("  - Press ctrl-z to exit");
#else
			puts("  - Press ctrl-d to exit");
#endif
			puts("  - Type a word to search (in kanji, kana and meanings).");
			puts("  Extended use:");
			puts("  - Write the number of a result to get more info. (Currently not actually more)");
			puts("  Search mode: You can set flags to change what is searched for.");
			puts("  - k: Search kanji");
			puts("  - r: Search reading (kana)");
			puts("  - m: Search meaning");
			puts("  - c: Convert the search string from romaji to kana");
			puts("  Use prefix a + to add flags, a - to remove flags or = to set the flags");
			puts("  - e.g.");
			puts("      +krmc (add all flags)");
			puts("      -krmc (Remove all flags -> Search for nothing?!)");
			puts("      =rc   (Search for romaji)");
			continue;
		}

		if(term.front() >= '0' && term.front() <= '9') {
			size_t idx = strtoull(term.c_str(), nullptr, 10) - 1;
			if(idx >= search.results().size()) {
				printf("Search index %zu does not exist.\n", idx);
			}
			else {
				printDetailed(search.results()[idx].word);
			}
			continue;
		}

		if(term.front() == '+' || term.front() == '=') {
			for(char c : term) switch (c) {
				case '+': break;
				case '=': flags = 0; break;
				case 'k': flags |= WordDatabase::SEARCH_KANJI;            break;
				case 'r': flags |= WordDatabase::SEARCH_KANA;             break;
				case 'm': flags |= WordDatabase::SEARCH_MEANING;          break;
				case 'c': flags |= WordDatabase::SEARCH_CONVERT_HIRAGANA; break;
				default: break;
			}
			continue;
		}

		if(term.front() == '-') {
			for(char c : term) switch (c) {
				case '-': break;
				case 'k': flags &= ~WordDatabase::SEARCH_KANJI;            break;
				case 'r': flags &= ~WordDatabase::SEARCH_KANA;             break;
				case 'm': flags &= ~WordDatabase::SEARCH_MEANING;          break;
				case 'c': flags &= ~WordDatabase::SEARCH_CONVERT_HIRAGANA; break;
				default: break;
			}
			continue;
		}

		if(term.front() == '!') {
			term.erase(0);
			term = search.lastTerm() + term;
		}

		database.search(&search, term, flags);

		size_t index = 1;
		// std::reverse(search.mResults.begin(), search.mResults.end());
		for(const SearchResult& result : search.results()) {
			printResult(search.results().size(), index++, result.word);
		}

		printf(" [ %zu results for \"%s\" in %zums ]\n", search.results().size(), term.c_str(), search.time());
	}
}
