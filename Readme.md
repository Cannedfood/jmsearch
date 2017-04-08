# JMsearch

JMsearch is a (currently) command line japanese-english dictionary kinda thing, mostly for learning japanese.

It is currently designed for unix, but the largest issue on windows would probably be finding a utf8-compatible terminal emulator.

Also **⇛ it requires a utf8 and japanese font compatible terminal to work ⇚**

## Usage

After starting it you can search a word by typing it pressing enter.

By default it assumes you search in romaji/kana, however you can prefix your search with a dot . to search within the meanings (e.g. ".bicycle")

End the program by using ctrl-c or writing "~" and typing enter

## License
Everything in this repository I created is licensed under creative commons CC0 1.0 (see LICENSE).

Rapidxml however is dual licensed under the boost and MIT licenses: see src/rapidxml/license.txt

## Building

1. Clone the repository
```
git clone git@github.com:Cannedfood/jmsearch.git
```

2. Build the library
```
cd jmsearch
g++ -std=c++14 -O2 src/*.cpp -o jmsearch
```
or use the `premake5.lua` with premake5.

## Installation
*This will be improved in the future*

JMsearch needs a uncompressed JMdict_e.xml in its working directory.
- Download the **JMdict_e.gz** file from [here](http://www.edrdg.org/jmdict/edict_doc.html)
- Extract the file named `data` from it and rename it `JMdict_e.xml`
- You have to start jmsearch within the directory of the unpacked `JMdict_e.xml` so you'll probably want to do something like
```
echo 'alias jmsearch="cd <folder with JMdict_e.xml> && <jmsearch executable>"' >> ~/.bashrc
```
and restart your terminal or
```
echo 'cd <folder wher JMdict_e.xml is> && <path to jmsearch> ${@}' > /bin/jmsearch
```
If you're feeling adventurous you can also change the path in the sources

## TODO:
(< means WIP)
- Sort search results by how much they match <
- Sort search results by relevance
- Clean up code & make more robust
	- Run a list of A LOT OF WORDS (dictionary & all 4-letter combinations) against Romaji2Hiragana and Romaji2Katakana to make sure it doesn't hang up
	- Run the whole dictionary against Kana2Romaji
- Search for similar sounding words e.g.
	- せけん (seken) -> せっけん (sekken)
	- しょねん (shonen) -> しょうねん (shounen)
- Use a settings file for enabling and disabling that stuff ^
- Create option to show romaji of words
- **Implement incremental search** (That's why there's a SearchState already)
- Multithread?
- **Create proper API**
- **Create QT frontend** (Or GTK?)
- Make it faster
	- Don't brute force
	- Heuristics: Find things with higher occurences faster
- Detect inflections
