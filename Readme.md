# JMsearch

JMsearch is a (currently) command line japanese-english dictionary kinda thing, mostly for learning japanese.

It is currently designed for unix, but the largest issue on windows would probably be finding a utf8-compatible terminal emulator.

Also **⇛ it requires a utf8 and japanese font compatible terminal to work ⇚**

## Usage

After starting it you can search a word by typing it pressing enter.

By default it searches everywhere (meanings & kana & romaji), I will improve that soon.

End the program by using ctrl-c or writing "~" and typing enter

## License
Everything in this repository I created is licensed under creative commons CC0 1.0 (see LICENSE).

Rapidxml is dual licensed under the boost and MIT licenses: see src/rapidxml/license.txt

This software uses the EDICT and KANJIDIC dictionary files. These files are the property of the Electronic Dictionary Research and Development Group, and are used in conformance with the Group's licence. You can get more information [here](http://www.edrdg.org/jmdict/j_jmdict.html)

## Building

1. Clone the repository
```bash
git clone git@github.com:Cannedfood/jmsearch.git
```

2. Build the library
```bash
cd jmsearch
g++ -std=c++14 -O2 src/*.cpp -o jmsearch
```
or use the `premake5.lua` with premake5.

## Installation
*This will be improved in the future*

If you are on a system supporting PKGBUILD (like ArchLinux):
```bash
mkdir jmdict_build && cd jmdict_build
curl https://raw.githubusercontent.com/Cannedfood/jmsearch/master/PKGBUILD > PKGBUILD
makepkg -i
```

Otherwise
- Download the **JMdict_e.gz** file from [here](http://www.edrdg.org/jmdict/edict_doc.html)
- Extract the file and rename it `JMdict_e.xml`
```bash
	# For example like this
	gunzip -c JMdict_e.gz > JMdict_e.xml
```
- You have to start jmsearch within the directory of the unpacked `JMdict_e.xml` unless it is in `/usr/share/jmsearch/JMdict_e.xml`.
If you're feeling adventurous you can also change the path in the sources

## TODO:
(< means WIP)
- Sort search results by how much they match **DONE**
- Split meanings into senses (as it is in the xml) **DONE**
- Implement caching, so we don't have to parse xml files all the time (SLOW and high memory requirement)
- Add support for the whole JMdict file (= Add russian german french italian spanish whatever support)
- Implement minimal incremental search (search in results if the search string simply got longer)
- Sort search results by relevance
	- Read flags from xml
- Clean up code & make more robust
	- Create unittests
		- Run a list of A LOT OF WORDS (dictionary & all 4-letter combinations) against Romaji2Hiragana and Romaji2Katakana to make sure it doesn't hang up
		- Run a list of all english words against Kana2Romaji
		- Run a list of all english words against the search
- Search for similar sounding words e.g.
	- せけん (seken) = world -> せっけん (sekken) = soap
	- しょねん (shonen) = first year -> しょうねん (shounen) = boy
	- れそこ (resoko) = ???? -> れいぞうこ (reizouko) = fridge
- Use a settings file for enabling and disabling that stuff ^
- Create option to show romaji of words
- **Implement incremental search** (That's why there's a SearchState already)
- Multithread?
- **Create proper API**
- **Create QT frontend** (Or GTK?)
- Make it faster
	- Don't brute force
	- Heuristics: Find things with higher occurences faster
- Detect inflections?
