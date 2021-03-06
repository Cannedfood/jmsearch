# JMsearch

JMsearch is a (currently) command line japanese-english dictionary kinda thing, mostly for learning japanese.

It is currently designed for unix, but the largest issue on windows would probably be finding a utf8-compatible terminal emulator.

Also **⇛ it requires a utf8 and japanese font compatible terminal to work ⇚**

## Usage

After starting it you can search a word by typing it pressing enter.

By default it searches everywhere (meanings & kana & romaji), I will improve that soon.

End the program by using ctrl-d (unix) or ctrl-z (windows)

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
  g++ -std=c++14 -O2 src/*.cpp -ldl -o jmsearch
  ```

  or use the `premake5.lua` with premake5.

3. It dynamically loads zlib when available to extract the dictionary files, so make sure it's installed. Alternatively you can extract the dictionary file yourself and rename it to JMdict_e.xml

## Installation

_This will be improved in the future_

If you are on a system supporting PKGBUILD (like ArchLinux):

```bash
mkdir jmdict_build && cd jmdict_build
curl https://raw.githubusercontent.com/Cannedfood/jmsearch/master/PKGBUILD > PKGBUILD
makepkg -i
```

Otherwise

- Download the **JMdict.gz** file from [here](http://www.edrdg.org/jmdict/edict_doc.html)
- If you only need english translations you can download the JMdict_en.gz, you'll have to rename it into JMdict.gz though
- For the program to work the `JMdict.gz` in one of

  - `/usr/share/jmsearch/JMdict.gz`
  - `/usr/share/jmsearch/JMdict.xml`
  - `~/.local/share/jmsearch/JMdict.gz`
  - `~/.local/share/jmsearch/JMdict.xml`
  - `./JMdict.xml`
  - `./JMdict.xml`

If you're feeling adventurous you can also change the paths in the sources

## TODO:

(< means WIP)

- Sort search results by how much they match **DONE**
- Split meanings into senses (as it is in the xml) **DONE**
- Add support for the whole JMdict file (= Add russian german french italian and spanish support) **DONE**
- Implement caching, so we don't have to parse xml files all the time (SLOW and high memory requirement)

  - Different cache files for different languages

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
