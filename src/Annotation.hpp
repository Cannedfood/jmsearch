#pragma once

#include <cstddef>
#include <cstring>
#include <unordered_map>

namespace detail {
	template<typename _HashType = size_t>
	struct FNV1aHash {
		using HashType = _HashType;

		HashType operator()(const char* c, size_t len) const {
			HashType hash;
			HashType prime;

			switch (sizeof(HashType)) {
				case 8:
					hash  = 14695981039346656037UL;
					prime = 1099511628211UL;
				break;
				case 4:
					hash  = 2166136261;
					prime = 16777619;
				break;
				default: return 0;
			}

			for(const char* b = c; b < c + len; b++) {
				hash = hash ^ ((HashType) *b);
				hash = hash * prime;
			}

			return hash;
		}

		HashType operator()(const char* c) const {
			return (*this)(c, strlen(c));
		}
	};

	struct CstrCmp : public std::binary_function<const char*, const char*, bool> {
		bool operator()(const char* a, const char* b) const
		{ return strcmp(a, b) == 0; }
	};
}

class Annotations {
	enum Annotation : unsigned char {
		F_none,   // NULL flag (marks end of flag string and is used as error-code)

		// Topics
		F_anat,   // anatomical term
		F_archit, // architecture term
		F_astron, // astronomy, etc. term
		F_Buddh,  // Buddhist term
		F_chem,   // chemistry term
		F_chn,    // children's language
		F_col,    // colloquialism
		F_comp,   // computer terminology
		F_baseb,  // baseball term
		F_biol,   // biology term
		F_bot,    // botany term
		F_bus,    // business term
		F_econ,   // economics term
		F_engr,   // engineering term
		F_finc,   // finance term
		F_food,   // food term
		F_geol,   // geology, etc. term
		F_geom,   // geometry term
		F_joc,    // jocular, humorous term
		F_law,    // law, etc. term
		F_MA,     // martial arts term
		F_mahj,   // mahjong term
		F_math,   // mathematics
		F_med,    // medicine, etc. term
		F_mil,    // military
		F_music,  // music term
		F_physics,// physics terminology
		F_poet,   // poetical term
		F_Shinto, // Shinto term
		F_shogi,  // shogi term
		F_sports, // sports term
		F_sumo,   // sumo term
		F_vulg,   // vulgar expression or word
		F_X,      // rude or X-rated term (not displayed in educational software)
		F_zool,   // zoology term

		// Accents
		F_kyb,    // Kyoto-ben
		F_osb,    // Osaka-ben
		F_ksb,    // Kansai-ben
		F_ktb,    // Kantou-ben
		F_tsb,    // Tosa-ben
		F_thb,    // Touhoku-ben
		F_tsug,   // Tsugaru-ben
		F_kyu,    // Kyuushuu-ben
		F_rkb,    // Ryuukyuu-ben
		F_nab,    // Nagano-ben
		F_hob,    // Hokkaido-ben

		// Crippled
		F_abbr,   // abbreviation
		F_arch,   // archaism

		// Word type - adjectives
		F_adj_i,  // adjective (keiyoushi)
		F_adj_ix, // adjective (keiyoushi) - yoi/ii class
		F_adj_na, // adjectival nouns or quasi-adjectives (keiyodoshi)
		F_adj_no, // nouns which may take the genitive case particle `no'
		F_adj_pn, // pre-noun adjectival (rentaishi)
		F_adj_t,  // `taru' adjective
		F_adj_f,  // noun or verb acting prenominally
		F_adv,    // adverb (fukushi)
		F_adv_to, // adverb taking the `to' particle
		F_ateji,  // ateji (phonetic) reading
		F_aux,    // auxiliary
		F_aux_v,  // auxiliary verb
		F_aux_adj,// auxiliary adjective
		F_conj,   // conjunction
		F_cop_da, // copula
		F_ctr,    // counter
		F_derog,  // derogatory
		F_eK,     // exclusively kanji
		F_ek,     // exclusively kana
		F_exp,    // expressions (phrases, clauses, etc.)
		F_fam,    // familiar language
		F_fem,    // female term or language
		F_gikun,  // gikun (meaning as reading) or jukujikun (special kanji reading)
		F_hon,    // honorific or respectful (sonkeigo) language
		F_hum,    // humble (kenjougo) language
		F_iK,     // word containing irregular kanji usage
		F_id,     // idiomatic expression
		F_ik,     // word containing irregular kana usage
		F_int,    // interjection (kandoushi)
		F_io,     // irregular okurigana usage
		F_iv,     // irregular verb
		F_ling,   // linguistics terminology
		F_m_sl,   // manga slang
		F_male,   // male term or language
		F_male_sl,// male slang
		F_n,      // noun (common) (futsuumeishi)
		F_n_adv,  // adverbial noun (fukushitekimeishi)
		F_n_suf,  // noun, used as a suffix
		F_n_pref, // noun, used as a prefix
		F_n_t,    // noun (temporal) (jisoumeishi)
		F_num,    // numeric
		F_oK,     // word containing out-dated kanji
		F_obs,    // obsolete term
		F_obsc,   // obscure term
		F_ok,     // out-dated or obsolete kana usage
		F_oik,    // old or irregular kana form
		F_on_mim, // onomatopoeic or mimetic word
		F_pn,     // pronoun
		F_pol,    // polite (teineigo) language
		F_pref,   // prefix
		F_proverb,// proverb
		F_prt,    // particle
		F_rare,   // rare
		F_sens,   // sensitive
		F_sl,     // slang
		F_suf,    // suffix
		F_uK,     // word usually written using kanji alone
		F_uk,     // word usually written using kana alone
		F_unc,    // unclassified
		F_yoji,   // yojijukugo
		F_v1,     // Ichidan verb
		F_v1_s,   // Ichidan verb - kureru special class
		F_v2a_s,  // Nidan verb with 'u' ending (archaic)
		F_v4h,    // Yodan verb with `hu/fu' ending (archaic)
		F_v4r,    // Yodan verb with `ru' ending (archaic)
		F_v5aru,  // Godan verb - -aru special class
		F_v5b,    // Godan verb with `bu' ending
		F_v5g,    // Godan verb with `gu' ending
		F_v5k,    // Godan verb with `ku' ending
		F_v5k_s,  // Godan verb - Iku/Yuku special class
		F_v5m,    // Godan verb with `mu' ending
		F_v5n,    // Godan verb with `nu' ending
		F_v5r,    // Godan verb with `ru' ending
		F_v5r_i,  // Godan verb with `ru' ending (irregular verb)
		F_v5s,    // Godan verb with `su' ending
		F_v5t,    // Godan verb with `tsu' ending
		F_v5u,    // Godan verb with `u' ending
		F_v5u_s,  // Godan verb with `u' ending (special class)
		F_v5uru,  // Godan verb - Uru old class verb (old form of Eru)
		F_vz,     // Ichidan verb - zuru verb (alternative form of -jiru verbs)
		F_vi,     // intransitive verb
		F_vk,     // Kuru verb - special class
		F_vn,     // irregular nu verb
		F_vr,     // irregular ru verb, plain form ends with -ri
		F_vs,     // noun or participle which takes the aux. verb suru
		F_vs_c,   // su verb - precursor to the modern suru
		F_vs_s,   // suru verb - special class
		F_vs_i,   // suru verb - irregular
		F_vt,     // transitive verb
		F_adj_kari, // `kari' adjective (archaic)
		F_adj_ku, // `ku' adjective (archaic)
		F_adj_shiku, // `shiku' adjective (archaic)
		F_adj_nari, // archaic/formal form of na-adjective
		F_n_pr,   // proper noun
		F_v_unspec, // verb unspecified
		F_v4k,    // Yodan verb with `ku' ending (archaic)
		F_v4g,    // Yodan verb with `gu' ending (archaic)
		F_v4s,    // Yodan verb with `su' ending (archaic)
		F_v4t,    // Yodan verb with `tsu' ending (archaic)
		F_v4n,    // Yodan verb with `nu' ending (archaic)
		F_v4b,    // Yodan verb with `bu' ending (archaic)
		F_v4m,    // Yodan verb with `mu' ending (archaic)
		F_v2k_k,  // Nidan verb (upper class) with `ku' ending (archaic)
		F_v2g_k,  // Nidan verb (upper class) with `gu' ending (archaic)
		F_v2t_k,  // Nidan verb (upper class) with `tsu' ending (archaic)
		F_v2d_k,  // Nidan verb (upper class) with `dzu' ending (archaic)
		F_v2h_k,  // Nidan verb (upper class) with `hu/fu' ending (archaic)
		F_v2b_k,  // Nidan verb (upper class) with `bu' ending (archaic)
		F_v2m_k,  // Nidan verb (upper class) with `mu' ending (archaic)
		F_v2y_k,  // Nidan verb (upper class) with `yu' ending (archaic)
		F_v2r_k,  // Nidan verb (upper class) with `ru' ending (archaic)
		F_v2k_s,  // Nidan verb (lower class) with `ku' ending (archaic)
		F_v2g_s,  // Nidan verb (lower class) with `gu' ending (archaic)
		F_v2s_s,  // Nidan verb (lower class) with `su' ending (archaic)
		F_v2z_s,  // Nidan verb (lower class) with `zu' ending (archaic)
		F_v2t_s,  // Nidan verb (lower class) with `tsu' ending (archaic)
		F_v2d_s,  // Nidan verb (lower class) with `dzu' ending (archaic)
		F_v2n_s,  // Nidan verb (lower class) with `nu' ending (archaic)
		F_v2h_s,  // Nidan verb (lower class) with `hu/fu' ending (archaic)
		F_v2b_s,  // Nidan verb (lower class) with `bu' ending (archaic)
		F_v2m_s,  // Nidan verb (lower class) with `mu' ending (archaic)
		F_v2y_s,  // Nidan verb (lower class) with `yu' ending (archaic)
		F_v2r_s,  // Nidan verb (lower class) with `ru' ending (archaic)
		F_v2w_s,  // Nidan verb (lower class) with `u' ending and `we' conjugati (archaic)
		NUM_ANNOTATIONS
	};

	using CstrMap =
		std::unordered_map<const char*, Annotation,
			detail::FNV1aHash<size_t>,
			detail::CstrCmp
		>;

	CstrMap                                  mReverseName;
	std::array<const char*, NUM_ANNOTATIONS> mNames;
	std::array<const char*, NUM_ANNOTATIONS> mDescriptions;

	Annotations();
	Annotations(Annotations&&) = delete;

	Annotation  parseAnnotation(const char* value);
	const char* annotationString(Annotations::Annotation ann) const;
	const char* annotationMeaning(Annotations::Annotation ann) const;
};
