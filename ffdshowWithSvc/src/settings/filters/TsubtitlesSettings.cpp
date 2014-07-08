/*
 * Copyright (c) 2003-2006 Milan Cutka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "stdafx.h"
#include "TsubtitlesSettings.h"
#include "Ttranslate.h"
#include "Tfont.h"
#include "Twindow.h"
#include "TimgFilterSubtitles.h"
#include "Csubtitles.h"
#include "CsubtitlesText.h"
#include "Cfont.h"
#include "Cvobsub.h"
#include "TffdshowPageDec.h"

const char_t* TsubtitlesSettings::alignments[]=
{
 _l("ffdshow default"),
 _l("left"),
 _l("center"),
 _l("right"),
 NULL
};
const char_t* TsubtitlesSettings::vobsubAAs[]=
{
 _l("none (fastest, most ugly)"),
 _l("approximate"),
 _l("full (slowest)"),
 _l("bilinear (fast and not too bad)"),
 _l("swscaler gaussian"),
 NULL
};
const TsubtitlesSettings::Tlang TsubtitlesSettings::langs[]=
{
// "cc","Closed Caption",
 {_l("default"),             _l("")  ,   NULL  },
 {_l("undefined"),              NULL ,_l("und")},

 {_l("(Afan) Oromo"),        _l("OM"),   NULL  },
 {_l("Abkhazian"),           _l("AB"),_l("abk")},
 {_l("Achinese"),               NULL ,_l("ace")},
 {_l("Acoli"),                  NULL ,_l("ach")},
 {_l("Adangme"),                NULL ,_l("ada")},
 {_l("Afar"),                _l("AA"),_l("aar")},
 {_l("Afrihili"),               NULL ,_l("afh")},
 {_l("Afrikaans"),           _l("AF"),_l("afr")},
 {_l("Afro-Asiatic (Other)"),   NULL ,_l("afa")},
 {_l("Akan"),                _l("AK"),_l("aka")},
 {_l("Akkadian"),               NULL ,_l("akk")},
 {_l("Albanian"),            _l("SQ"),_l("alb")},
 {_l("Albanian"),            _l("SQ"),_l("sqi")},
 {_l("Aleut"),                  NULL ,_l("ale")},
 {_l("Algonquian languages"),   NULL ,_l("alg")},
 {_l("Altaic (Other)"),         NULL ,_l("tut")},
 {_l("Amharic"),             _l("AM"),_l("amh")},
 {_l("Apache languages"),       NULL ,_l("apa")},
 {_l("Arabic"),              _l("AR"),_l("ara")},
 {_l("Aragonese"),           _l("AN"),_l("arg")},
 {_l("Aramaic"),                NULL ,_l("arc")},
 {_l("Arapaho"),                NULL ,_l("arp")},
 {_l("Araucanian"),             NULL ,_l("arn")},
 {_l("Arawak"),                 NULL ,_l("arw")},
 {_l("Armenian"),            _l("HY"),_l("arm")},
 {_l("Armenian"),            _l("HY"),_l("hye")},
 {_l("Artificial (Other)"),     NULL ,_l("art")},
 {_l("Assamese"),            _l("AS"),_l("asm")},
 {_l("Asturian; Bable"),        NULL ,_l("ast")},
 {_l("Athapascan languages"),   NULL ,_l("ath")},
 {_l("Australian languages"),   NULL ,_l("aus")},
 {_l("Austronesian (Other)"),   NULL ,_l("map")},
 {_l("Avaric"),              _l("AV"),_l("ava")},
 {_l("Avestan"),             _l("AE"),_l("ave")},
 {_l("Aymara"),              _l("AY"),_l("aym")},
 {_l("Awadhi"),                 NULL ,_l("awa")},
 {_l("Azerbaijani"),         _l("AZ"),_l("aze")},
 {_l("Balinese"),               NULL ,_l("ban")},
 {_l("Baltic (Other)"),         NULL ,_l("bat")},
 {_l("Baluchi"),                NULL ,_l("bal")},
 {_l("Bambara"),             _l("BM"),_l("bam")},
 {_l("Bamileke languages"),     NULL ,_l("bai")},
 {_l("Banda"),                  NULL ,_l("bad")},
 {_l("Bantu (Other)"),          NULL ,_l("bnt")},
 {_l("Basa"),                   NULL ,_l("bas")},
 {_l("Bashkir"),             _l("BA"),_l("bak")},
 {_l("Basque"),              _l("EU"),_l("baq")},
 {_l("Basque"),              _l("EU"),_l("eus")},
 {_l("Batak (Indonesia)"),      NULL ,_l("btk")},
 {_l("Beja"),                   NULL ,_l("bej")},
 {_l("Belarusian"),          _l("BE"),_l("bel")},
 {_l("Bemba"),                  NULL ,_l("bem")},
 {_l("Bengali; Bangla"),     _l("BN"),_l("ben")},
 {_l("Berber (Other)"),         NULL ,_l("ber")},
 {_l("Bhojpuri"),               NULL ,_l("bho")},
 {_l("Bhutani"),             _l("DZ"),   NULL  },
 {_l("Bihari"),              _l("BH"),_l("bih")},
 {_l("Bikol"),                  NULL ,_l("bik")},
 {_l("Bini"),                   NULL ,_l("bin")},
 {_l("Bislama"),             _l("BI"),_l("bis")},
 {_l("Bosnian"),             _l("BS"),_l("bos")},
 {_l("Braj"),                   NULL ,_l("bra")},
 {_l("Breton"),              _l("BR"),_l("bre")},
 {_l("Buginese"),               NULL ,_l("bug")},
 {_l("Bulgarian"),           _l("BG"),_l("bul")},
 {_l("Buriat"),                 NULL ,_l("bua")},
 {_l("Burmese"),             _l("MY"),_l("bur")},
 {_l("Burmese"),             _l("MY"),_l("mya")},
 {_l("Caddo"),                  NULL ,_l("cad")},
 {_l("Cambodian"),           _l("KM"),   NULL  },
 {_l("Carib"),                  NULL ,_l("car")},
 {_l("Catalan"),             _l("CA"),_l("cat")},
 {_l("Chinese"),             _l("ZH"),_l("zho")},
 {_l("Chinese"),             _l("ZH"),_l("chi")},
 {_l("Corsican"),            _l("CO"),_l("cos")},
 {_l("Czech"),               _l("CS"),_l("cze")},
 {_l("Czech"),               _l("CZ"),_l("ces")},
 {_l("Dansk"),               _l("DA"),_l("dan")},
 {_l("Deutsch"),             _l("DE"),_l("ger")},
 {_l("Deutsch"),             _l("DE"),_l("deu")},
 {_l("English"),             _l("EN"),_l("eng")},
 {_l("Espanol"),             _l("ES"),_l("spa")},
 {_l("Esperanto"),           _l("EO"),_l("epo")},
 {_l("Estonian"),            _l("ET"),_l("est")},
 {_l("Faroese"),             _l("FO"),_l("fao")},
 {_l("Fijian"),              _l("FJ"),_l("fij")},
 {_l("Finnish"),             _l("FI"),_l("fin")},
 {_l("French"),              _l("FR"),_l("fra")},
 {_l("French"),              _l("FR"),_l("fre")},
 {_l("Frisian"),             _l("FY"),_l("fry")},
 {_l("Galician"),            _l("GL"),_l("glg")},
 {_l("Georgian"),            _l("KA"),_l("geo")},
 {_l("Georgian"),            _l("KA"),_l("kat")},
 {_l("Greek"),               _l("EL"),_l("el") },
 {_l("Greek"),               _l("EL"),_l("ell")},
 {_l("Greenlandic"),         _l("KL"),_l("kal")},
 {_l("Guarani"),             _l("GN"),_l("grn")},
 {_l("Gujarati"),            _l("GU"),_l("guj")},
 {_l("Hausa"),               _l("HA"),_l("hau")},
 {_l("Hebrew"),              _l("HE"),_l("heb")},
 {_l("Hebrew"),              _l("HE"),_l("her")},
 {_l("Hebrew"),              _l("HZ"),_l("heb")},
 {_l("Hebrew"),              _l("HZ"),_l("her")},
 {_l("Hebrew"),              _l("IW"),_l("heb")},
 {_l("Hebrew"),              _l("IW"),_l("her")},
 {_l("Hindi"),               _l("HI"),_l("hin")},
 {_l("Hrvatski"),            _l("HR"),_l("src")},
 {_l("Hrvatski"),            _l("HR"),_l("hrv")},
 {_l("Hungarian"),           _l("HU"),_l("hun")},
 {_l("Icelandic"),           _l("IS"),_l("ice")},
 {_l("Icelandic"),           _l("IS"),_l("isl")},
 {_l("Indonesian"),          _l("ID"),_l("ind")},
 {_l("Indonesian"),          _l("IN"),_l("ind")},
 {_l("Interlingua"),         _l("IA"),_l("ina")},
 {_l("Interlingue"),         _l("IE"),_l("ile")},
 {_l("Inuktitut"),           _l("IU"),_l("iku")},
 {_l("Inupiak"),             _l("IK"),_l("ipk")},
 {_l("Irish"),               _l("GA"),_l("gle")},
 {_l("Italian"),             _l("IT"),_l("ita")},
 {_l("Japanese"),            _l("JA"),_l("jpn")},
 {_l("Javanese"),            _l("JW"),_l("jav")},
 {_l("Kannada"),             _l("KN"),_l("kan")},
 {_l("Kashmiri"),            _l("KS"),_l("kas")},
 {_l("Kazakh"),              _l("KK"),_l("kaz")},
 {_l("Kinyarwanda"),         _l("RW"),_l("kin")},
 {_l("Kirghiz"),             _l("KY"),_l("kir")},
 {_l("Kirundi"),             _l("RN"),_l("run")},
 {_l("Korean"),              _l("KO"),_l("kor")},
 {_l("Kurdish"),             _l("KU"),_l("kur")},
 {_l("Laothian"),            _l("LO"),_l("lao")},
 {_l("Latin"),               _l("LA"),_l("lat")},
 {_l("Latvian, Lettish"),    _l("LV"),_l("lav")},
 {_l("Lingala"),             _l("LN"),_l("lin")},
 {_l("Lithuanian"),          _l("LT"),_l("lit")},
 {_l("Macedonian"),          _l("MK"),_l("mac")},
 {_l("Macedonian"),          _l("MK"),_l("mkv")},
 {_l("Malagasy"),            _l("MG"),_l("mlg")},
 {_l("Malay"),               _l("MS"),_l("may")},
 {_l("Malay"),               _l("MS"),_l("msa")},
 {_l("Malayalam"),           _l("ML"),_l("mal")},
 {_l("Maltese"),             _l("MT"),_l("mlt")},
 {_l("Maori"),               _l("MI"),_l("mao")},
 {_l("Maori"),               _l("MI"),_l("mri")},
 {_l("Marathi"),             _l("MR"),_l("mar")},
 {_l("Moldavian"),           _l("MO"),_l("mol")},
 {_l("Mongolian"),           _l("MN"),_l("mon")},
 {_l("Nauru"),               _l("NA"),_l("nau")},
 {_l("Nederlands"),          _l("NL"),_l("nld")},
 {_l("Nederlands"),          _l("NL"),_l("dut")},
 {_l("Nepali"),              _l("NE"),_l("nep")},
 {_l("Norsk"),               _l("NO"),_l("nor")},
 {_l("Norwegian"),           _l("NB"),_l("nob")},
 {_l("Occitan"),             _l("OC"),_l("oci")},
 {_l("Oriya"),               _l("OR"),_l("ori")},
 {_l("Pashto, Pushto"),      _l("PS"),_l("pus")},
 {_l("Persian"),             _l("FA"),_l("per")},
 {_l("Persian"),             _l("FA"),_l("fas")},
 {_l("Polish"),              _l("PL"),_l("pol")},
 {_l("Portugues"),           _l("PT"),_l("por")},
 {_l("Punjabi"),             _l("PA"),_l("pan")},
 {_l("Quechua"),             _l("QU"),_l("que")},
 {_l("Rhaeto-Romance"),      _l("RM"),_l("roh")},
 {_l("Romanian"),            _l("RO"),_l("ron")},
 {_l("Romanian"),            _l("RO"),_l("run")},
 {_l("Russian"),             _l("RU"),_l("rus")},
 {_l("Samoan"),              _l("SM"),_l("smo")},
 {_l("Sangho"),              _l("SG"),_l("sag")},
 {_l("Sanskrit"),            _l("SA"),_l("san")},
 {_l("Scots Gaelic"),        _l("GD"),_l("gla")},
 {_l("Serbian"),             _l("SR"),_l("scc")},
 {_l("Serbian"),             _l("SR"),_l("srp")},
 {_l("Serbo-Croatian"),      _l("SH"),   NULL  },
 {_l("Sesotho"),             _l("ST"),_l("sot")},
 {_l("Setswana"),            _l("TN"),_l("tsn")},
 {_l("Shona"),               _l("SN"),_l("sna")},
 {_l("Sindhi"),              _l("SD"),_l("snd")},
 {_l("Sinhalese"),           _l("SI"),_l("sin")},
 {_l("Siswati"),             _l("SS"),_l("ssw")},
 {_l("Slovak"),              _l("SK"),_l("slk")},
 {_l("Slovak"),              _l("SK"),_l("slo")},
 {_l("Slovenian"),           _l("SL"),_l("slv")},
 {_l("Somali"),              _l("SO"),_l("som")},
 {_l("Sundanese"),           _l("SU"),_l("sun")},
 {_l("Swahili"),             _l("SW"),_l("swa")},
 {_l("Swedish"),             _l("SV"),_l("swe")},
 {_l("Tagalog"),             _l("TL"),_l("tgl")},
 {_l("Tajik"),               _l("TG"),_l("tgk")},
 {_l("Tamil"),               _l("TA"),_l("tam")},
 {_l("Tatar"),               _l("TT"),_l("tat")},
 {_l("Telugu"),              _l("TE"),_l("tel")},
 {_l("Thai"),                _l("TH"),_l("tha")},
 {_l("Tibetan"),             _l("BO"),_l("tib")},
 {_l("Tibetan"),             _l("BO"),_l("bod")},
 {_l("Tigrinya"),            _l("TI"),_l("tir")},
 {_l("Tonga"),               _l("TO"),_l("ton")},
 {_l("Tsonga"),              _l("TS"),_l("tso")},
 {_l("Turkish"),             _l("TR"),_l("tur")},
 {_l("Turkmen"),             _l("TK"),_l("tuk")},
 {_l("Twi"),                 _l("TW"),_l("twi")},
 {_l("Uighur"),              _l("UG"),_l("uig")},
 {_l("Ukrainian"),           _l("UK"),_l("ukr")},
 {_l("Urdu"),                _l("UR"),_l("urd")},
 {_l("Uzbek"),               _l("UZ"),_l("uzb")},
 {_l("Vietnamese"),          _l("VI"),_l("vie")},
 {_l("Volapuk"),             _l("VO"),_l("vol")},
 {_l("Welsh"),               _l("CY"),_l("wel")},
 {_l("Welsh"),               _l("CY"),_l("cym")},
 {_l("Wolof"),               _l("WO"),_l("wol")},
 {_l("Xhosa"),               _l("XH"),_l("xho")},
 {_l("Yiddish"),             _l("JI"),_l("yid")},
 {_l("Yiddish"),             _l("YI"),_l("yid")},
 {_l("Yoruba"),              _l("YO"),_l("yor")},
 {_l("Zhuang"),              _l("ZA"),_l("zha")},
 {_l("Zulu"),                _l("ZU"),_l("zul")},
 {   NULL ,                     NULL ,   NULL  }
};
/*
   {"Caucasian (Other)", _l("cau", NULL},
   {"Cebuano", _l("ceb", NULL},
   {"Celtic (Other)", _l("cel", NULL},
   {"Central American Indian (Other)", _l("cai", NULL},
   {"Chagatai", _l("chg", NULL},
   {"Chamic languages", _l("cmc", NULL},
   {"Chamorro", _l("cha", _l("ch")},
   {"Chechen", _l("che", _l("ce")},
   {"Cherokee", _l("chr", NULL},
   {"Chewa; Chichewa; Nyanja", _l("nya", _l("ny")},
   {"Cheyenne", _l("chy", NULL},
   {"Chibcha", _l("chb", NULL},
   {"Chichewa; Chewa; Nyanja", _l("nya", _l("ny")},
   {"Chinook jargon", _l("chn", NULL},
   {"Chipewyan", _l("chp", NULL},
   {"Choctaw", _l("cho", NULL},
   {"Chuang; Zhuang", _l("zha", _l("za")},
   {"Church Slavic; Old Church Slavonic", _l("chu", _l("cu")},
   {"Old Church Slavonic; Old Slavonic; _l(", _l("chu", _l("cu")},
   {"Church Slavonic; Old Bulgarian; Church Slavic;", _l("chu", _l("cu")},
   {"Old Slavonic; Church Slavonic; Old Bulgarian;", _l("chu", _l("cu")},
   {"Church Slavic; Old Church Slavonic", _l("chu", _l("cu")},
   {"Chuukese", _l("chk", NULL},
   {"Chuvash", _l("chv", _l("cv")},
   {"Coptic", _l("cop", NULL},
   {"Cornish", _l("cor", _l("kw")},
   {"Cree", _l("cre", _l("cr")},
   {"Creek", _l("mus", NULL},
   {"Creoles and pidgins (Other)", _l("crp", NULL},
   {"Creoles and pidgins,", _l("cpe", NULL},
   //   {"English-based (Other)", NULL, NULL},
   {"Creoles and pidgins,", _l("cpf", NULL},
   //   {"French-based (Other)", NULL, NULL},
   {"Creoles and pidgins,", _l("cpp", NULL},
   //   {"Portuguese-based (Other)", NULL, NULL},
   {"Cushitic (Other)", _l("cus", NULL},
   {"Dakota", _l("dak", NULL},
   {"Dargwa", _l("dar", NULL},
   {"Dayak", _l("day", NULL},
   {"Delaware", _l("del", NULL},
   {"Dinka", _l("din", NULL},
   {"Divehi", _l("div", _l("dv")},
   {"Dogri", _l("doi", NULL},
   {"Dogrib", _l("dgr", NULL},
   {"Dravidian (Other)", _l("dra", NULL},
   {"Duala", _l("dua", NULL},
   {"Dutch, Middle (ca. 1050-1350)", _l("dum", NULL},
   {"Dyula", _l("dyu", NULL},
   {"Dzongkha", _l("dzo", _l("dz")},
   {"Efik", _l("efi", NULL},
   {"Egyptian (Ancient)", _l("egy", NULL},
   {"Ekajuk", _l("eka", NULL},
   {"Elamite", _l("elx", NULL},
   {"English, Middle (1100-1500)", _l("enm", NULL},
   {"English, Old (ca.450-1100)", _l("ang", NULL},
   {"Ewe", _l("ewe", _l("ee")},
   {"Ewondo", _l("ewo", NULL},
   {"Fang", _l("fan", NULL},
   {"Fanti", _l("fat", NULL},
   {"Finno-Ugrian (Other)", _l("fiu", NULL},
   {"Flemish; Dutch", _l("dut", _l("nl")},
   {"Fon", _l("fon", NULL},
   {"French, Middle (ca.1400-1600)", _l("frm", NULL},
   {"French, Old (842-ca.1400)", _l("fro", NULL},
   {"Friulian", _l("fur", NULL},
   {"Fulah", _l("ful", _l("ff")},
   {"Ga", _l("gaa", NULL},
   {"Gaelic; Scottish Gaelic", _l("gla", _l("gd")},
   {"Ganda", _l("lug", _l("lg")},
   {"Gayo", _l("gay", NULL},
   {"Gbaya", _l("gba", NULL},
   {"Geez", _l("gez", NULL},
   {"German, Low; Saxon, Low; Low German; Low Saxon", _l("nds", NULL},
   {"German, Middle High (ca.1050-1500)", _l("gmh", NULL},
   {"German, Old High (ca.750-1050)", _l("goh", NULL},
   {"Germanic (Other)", _l("gem", NULL},
   {"Gikuyu; Kikuyu", _l("kik", _l("ki")},
   {"Gilbertese", _l("gil", NULL},
   {"Gondi", _l("gon", NULL},
   {"Gorontalo", _l("gor", NULL},
   {"Gothic", _l("got", NULL},
   {"Grebo", _l("grb", NULL},
   {"Greek, Ancient (to 1453)", _l("grc", NULL},
   {"Gwich+in", _l("gwi", NULL},
   {"Haida", _l("hai", NULL},
   {"Hawaiian", _l("haw", NULL},
   {"Hiligaynon", _l("hil", NULL},
   {"Himachali", _l("him", NULL},
   {"Hiri Motu", _l("hmo", _l("ho")},
   {"Hittite", _l("hit", NULL},
   {"Hmong", _l("hmn", NULL},
   {"Hupa", _l("hup", NULL},
   {"Iban", _l("iba", NULL},
   {"Ido", _l("ido", _l("io")},
   {"Igbo", _l("ibo", _l("ig")},
   {"Ijo", _l("ijo", NULL},
   {"Iloko", _l("ilo", NULL},
   {"Inari Sami", _l("smn", NULL},
   {"Indic (Other)", _l("inc", NULL},
   {"Indo-European (Other)", _l("ine", NULL},
   {"Ingush", _l("inh", NULL},
   {"Iranian (Other)", _l("ira", NULL},
   {"Irish, Middle (900-1200)", _l("mga", NULL},
   {"Irish, Old (to 900)", _l("sga", NULL},
   {"Iroquoian languages", _l("iro", NULL},
   {"Judeo-Arabic", _l("jrb", NULL},
   {"Judeo-Persian", _l("jpr", NULL},
   {"Kabardian", _l("kbd", NULL},
   {"Kabyle", _l("kab", NULL},
   {"Kachin", _l("kac", NULL},
   {"Kalaallisut; Greenlandic", _l("kal", _l("kl")},
   {"Kamba", _l("kam", NULL},
   {"Kanuri", _l("kau", _l("kr")},
   {"Kara-Kalpak", _l("kaa", NULL},
   {"Karen", _l("kar", NULL},
   {"Kawi", _l("kaw", NULL},
   {"Khasi", _l("kha", NULL},
   {"Khmer", _l("khm", _l("km")},
   {"Khoisan (Other)", _l("khi", NULL},
   {"Khotanese", _l("kho", NULL},
   {"Kikuyu; Gikuyu", _l("kik", _l("ki")},
   {"Kimbundu", _l("kmb", NULL},
   {"Klingon; tlhlngan-Hol", _l("tlh", NULL},
   {"Komi", _l("kom", _l("kv")},
   {"Kongo", _l("kon", _l("kg")},
   {"Konkani", _l("kok", NULL},
   {"Kosraean", _l("kos", NULL},
   {"Kpelle", _l("kpe", NULL},
   {"Kru", _l("kro", NULL},
   {"Kuanyama; Kwanyama", _l("kua", _l("kj")},
   {"Kumyk", _l("kum", NULL},
   {"Kurukh", _l("kru", NULL},
   {"Kutenai", _l("kut", NULL},
   {"Kwanyama, Kuanyama", _l("kua", _l("kj")},
   {"Ladino", _l("lad", NULL},
   {"Lahnda", _l("lah", NULL},
   {"Lamba", _l("lam", NULL},
   {"Letzeburgesch; Luxembourgish", _l("ltz", _l("lb")},
   {"Lezghian", _l("lez", NULL},
   {"Limburgan; Limburger; Limburgish", _l("lim", _l("li")},
   {"Limburger; Limburgan; Limburgish;", _l("lim", _l("li")},
   {"Limburgish; Limburger; Limburgan", _l("lim", _l("li")},
   {"Low German; Low Saxon; German, Low; Saxon, Low", _l("nds", NULL},
   {"Low Saxon; Low German; Saxon, Low; German, Low", _l("nds", NULL},
   {"Lozi", _l("loz", NULL},
   {"Luba-Katanga", _l("lub", _l("lu")},
   {"Luba-Lulua", _l("lua", NULL},
   {"Luiseno", _l("lui", NULL},
   {"Lule Sami", _l("smj", NULL},
   {"Lunda", _l("lun", NULL},
   {"Luo (Kenya and Tanzania)", _l("luo", NULL},
   {"Lushai", _l("lus", NULL},
   {"Luxembourgish; Letzeburgesch", _l("ltz", _l("lb")},
   {"Madurese", _l("mad", NULL},
   {"Magahi", _l("mag", NULL},
   {"Maithili", _l("mai", NULL},
   {"Makasar", _l("mak", NULL},
   {"Manchu", _l("mnc", NULL},
   {"Mandar", _l("mdr", NULL},
   {"Mandingo", _l("man", NULL},
   {"Manipuri", _l("mni", NULL},
   {"Manobo languages", _l("mno", NULL},
   {"Manx", _l("glv", _l("gv")},
   {"Mari", _l("chm", NULL},
   {"Marshallese", _l("mah", _l("mh")},
   {"Marwari", _l("mwr", NULL},
   {"Masai", _l("mas", NULL},
   {"Mayan languages", _l("myn", NULL},
   {"Mende", _l("men", NULL},
   {"Micmac", _l("mic", NULL},
   {"Minangkabau", _l("min", NULL},
   {"Miscellaneous languages", _l("mis", NULL},
   {"Mohawk", _l("moh", NULL},
   {"Mon-Khmer (Other)", _l("mkh", NULL},
   {"Mongo", _l("lol", NULL},
   {"Mossi", _l("mos", NULL},
   {"Multiple languages", _l("mul", NULL},
   {"Munda languages", _l("mun", NULL},
   {"Nahuatl", _l("nah", NULL},
   {"Navaho, Navajo", _l("nav", _l("nv")},
   {"Navajo; Navaho", _l("nav", _l("nv")},
   {"Ndebele, North", _l("nde", _l("nd")},
   {"Ndebele, South", _l("nbl", _l("nr")},
   {"Ndonga", _l("ndo", _l("ng")},
   {"Neapolitan", _l("nap", NULL},
   {"Newari", _l("new", NULL},
   {"Nias", _l("nia", NULL},
   {"Niger-Kordofanian (Other)", _l("nic", NULL},
   {"Nilo-Saharan (Other)", _l("ssa", NULL},
   {"Niuean", _l("niu", NULL},
   {"Norse, Old", _l("non", NULL},
   {"North American Indian (Other)", _l("nai", NULL},
   {"Northern Sami", _l("sme", _l("se")},
   {"North Ndebele", _l("nde", _l("nd")},
   {"Norwegian Bokmòl; Bokmòl, Norwegian", _l("nob", _l("nb")},
   {"Norwegian Nynorsk; Nynorsk, Norwegian", _l("nno", _l("nn")},
   {"Nubian languages", _l("nub", NULL},
   {"Nyamwezi", _l("nym", NULL},
   {"Nyanja; Chichewa; Chewa", _l("nya", _l("ny")},
   {"Nyankole", _l("nyn", NULL},
   {"Nynorsk, Norwegian; Norwegian Nynorsk", _l("nno", _l("nn")},
   {"Nyoro", _l("nyo", NULL},
   {"Nzima", _l("nzi", NULL},
   {"Ojibwa", _l("oji", _l("oj")},
   {"Old Bulgarian; Old Slavonic; Church Slavonic;", _l("chu", _l("cu")},
   {"Oromo", _l("orm", _l("om")},
   {"Osage", _l("osa", NULL},
   {"Ossetian; Ossetic", _l("oss", _l("os")},
   {"Ossetic; Ossetian", _l("oss", _l("os")},
   {"Otomian languages", _l("oto", NULL},
   {"Pahlavi", _l("pal", NULL},
   {"Palauan", _l("pau", NULL},
   {"Pali", _l("pli", _l("pi")},
   {"Pampanga", _l("pam", NULL},
   {"Pangasinan", _l("pag", NULL},
   {"Papiamento", _l("pap", NULL},
   {"Papuan (Other)", _l("paa", NULL},
   {"Persian, Old (ca.600-400 B.C.)", _l("peo", NULL},
   {"Philippine (Other)", _l("phi", NULL},
   {"Phoenician", _l("phn", NULL},
   {"Pohnpeian", _l("pon", NULL},
   {"Prakrit languages", _l("pra", NULL},
   {"Provenšal; Occitan (post 1500)", _l("oci", _l("oc")},
   {"Provenšal, Old (to 1500)", _l("pro", NULL},
   {"Rajasthani", _l("raj", NULL},
   {"Rapanui", _l("rap", NULL},
   {"Rarotongan", _l("rar", NULL},
   {"Reserved for local use", _l("qaa-qtz", NULL},
   {"Romance (Other)", _l("roa", NULL},
   {"Romany", _l("rom", NULL},
   {"Salishan languages", _l("sal", NULL},
   {"Samaritan Aramaic", _l("sam", NULL},
   {"Sami languages (Other)", _l("smi", NULL},
   {"Sandawe", _l("sad", NULL},
   {"Santali", _l("sat", NULL},
   {"Sardinian", _l("srd", _l("sc")},
   {"Sasak", _l("sas", NULL},
   {"Saxon, Low; German, Low; Low Saxon; Low German", _l("nds", NULL},
   {"Scots", _l("sco", NULL},
   {"Selkup", _l("sel", NULL},
   {"Semitic (Other)", _l("sem", NULL},
   {"Serer", _l("srr", NULL},
   {"Shan", _l("shn", NULL},
   {"Sichuan Yi", _l("iii", _l("ii")},
   {"Sidamo", _l("sid", NULL},
   {"Sign languages", _l("sgn", NULL},
   {"Siksika", _l("bla", NULL},
   {"Sino-Tibetan (Other)", _l("sit", NULL},
   {"Siouan languages", _l("sio", NULL},
   {"Skolt Sami", _l("sms", NULL},
   {"Slave (Athapascan)", _l("den", NULL},
   {"Slavic (Other)", _l("sla", NULL},
   {"Sogdian", _l("sog", NULL},
   {"Songhai", _l("son", NULL},
   {"Soninke", _l("snk", NULL},
   {"Sorbian languages", _l("wen", NULL},
   {"Sotho, Northern", _l("nso", NULL},
   {"South American Indian (Other)", _l("sai", NULL},
   {"Southern Sami", _l("sma", NULL},
   {"South Ndebele", _l("nbl", _l("nr")},
   {"Spanish", _l("spa", _l("es")},
   {"Sukuma", _l("suk", NULL},
   {"Sumerian", _l("sux", NULL},
   {"Susu", _l("sus", NULL},
   {"Syriac", _l("syr", NULL},
   {"Tahitian", _l("tah", _l("ty")},
   {"Tai (Other)", _l("tai", NULL},
   {"Tamashek", _l("tmh", NULL},
   {"Tereno", _l("ter", NULL},
   {"Tetum", _l("tet", NULL},
   {"Tigre", _l("tig", NULL},
   {"Timne", _l("tem", NULL},
   {"Tiv", _l("tiv", NULL},
   {"Tlingit", _l("tli", NULL},
   {"Tok Pisin", _l("tpi", NULL},
   {"Tokelau", _l("tkl", NULL},
   {"Tsimshian", _l("tsi", NULL},
   {"Tumbuka", _l("tum", NULL},
   {"Tupi languages", _l("tup", NULL},
   {"Turkish, Ottoman (1500-1928)", _l("ota", NULL},
   {"Tuvalu", _l("tvl", NULL},
   {"Tuvinian", _l("tyv", NULL},
   {"Ugaritic", _l("uga", NULL},
   {"Umbundu", _l("umb", NULL},
   {"Undetermined", _l("und", NULL},
   {"Vai", _l("vai", NULL},
   {"Venda", _l("ven", _l("ve")},
   {"Votic", _l("vot", NULL},
   {"Wakashan languages", _l("wak", NULL},
   {"Walamo", _l("wal", NULL},
   {"Walloon", _l("wln", _l("wa")},
   {"Waray", _l("war", NULL},
   {"Washo", _l("was", NULL},
   {"Yakut", _l("sah", NULL},
   {"Yao", _l("yao", NULL},
   {"Yapese", _l("yap", NULL},
   {"Yupik languages", _l("ypk", NULL},
   {"Zande", _l("znd", NULL},
   {"Zapotec", _l("zap", NULL},
   {"Zenaga", _l("zen", NULL},
   {"Zulu", _l("zul", _l("zu")},
   {"Zuni", _l("zun", NULL},
   {NULL, NULL, NULL}};
*/

const char_t* TsubtitlesSettings::durations[]=
{
 _l("subtitle"),
 _l("line"),
 _l("character"),
 NULL
};

const char_t* TsubtitlesSettings::fixIls[]=
{
 _l("English (may work with other languages)"),
 _l("French"),
 _l("Deutsch"),
 _l("Czech"),
 _l("Italian"),
 _l("Polish"),
 NULL
};

const char_t* TsubtitlesSettings::wordWraps[]=
{
 _l("Smart wrapping, lines are evenly broken"),
 _l("End-of-line word wrapping"),
 _l("Smart wrapping, lower line gets wider"),
 NULL
};

const TfilterIDFF TsubtitlesSettings::idffs=
{
 /*name*/      _l("Subtitles"),
 /*id*/        IDFF_filterSubtitles,
 /*is*/        IDFF_isSubtitles,
 /*order*/     IDFF_orderSubtitles,
 /*show*/      IDFF_showSubtitles,
 /*full*/      IDFF_fullSubtitles,
 /*half*/      0,
 /*dlgId*/     IDD_SUBTITLES,
};

TsubtitlesSettings::TsubtitlesSettings(TintStrColl *Icoll,TfilterIDFFs *filters, int Ifiltermode):TfilterSettingsVideo(sizeof(*this),Icoll,filters,&idffs),font(Icoll),filtermode(Ifiltermode)
{
 deepcopy=true;
 memset(flnm,0,sizeof(flnm));
 half=0;
 static const TintOptionT<TsubtitlesSettings> iopts[]=
  {
   IDFF_isSubtitles            ,&TsubtitlesSettings::is                      ,0,0,_l(""),1,
     _l("isSubtitles"), 0,
   IDFF_showSubtitles          ,&TsubtitlesSettings::show                    ,0,0,_l(""),1,
     _l("showSubtitles"), 1,
   IDFF_orderSubtitles         ,&TsubtitlesSettings::order                   ,1,1,_l(""),1,
     _l("orderSubtitles"), 0,
   IDFF_fullSubtitles          ,&TsubtitlesSettings::full                    ,0,0,_l(""),1,
     _l("fullSubtitles"), 1,
   IDFF_subAutoFlnm            ,&TsubtitlesSettings::autoFlnm                ,0,0,_l(""),1,
     _l("subAutoFlnm"), 1,
   IDFF_subPosX                ,&TsubtitlesSettings::posX                    ,0,100,_l(""),1,
     _l("subPosX"), 50,
   IDFF_subPosY                ,&TsubtitlesSettings::posY                    ,0,100,_l(""),1,
     _l("subPosY"), 90,
   IDFF_subAlign               ,&TsubtitlesSettings::align                   ,0,3,_l(""),1,
     _l("subAlign"), ALIGN_FFDSHOW,
   IDFF_subIsExpand            ,&TsubtitlesSettings::isExpand                ,0,0,_l(""),1,
     _l("subIsExpand"), -1,
   IDFF_subExpand              ,&TsubtitlesSettings::expandCode              ,1,1,_l(""),1,
     _l("subExpand"), 1,
   IDFF_subDelay               ,&TsubtitlesSettings::delay                   ,1,1,_l("Subtitles delay"), 1,
     _l("subDelay"), delayDef,
   IDFF_subSpeed               ,&TsubtitlesSettings::speed                   ,1,INT_MAX/2,_l(""),1,
     _l("subSpeed"), speedDef,
   IDFF_subSpeed2              ,&TsubtitlesSettings::speed2                  ,1,INT_MAX/2,_l(""),1,
     _l("subSpeed2"), speedDef,
   IDFF_subStereoscopic        ,&TsubtitlesSettings::stereoscopic            ,0,0,_l(""),1,
     _l("subStereoscopic"), 0,
   IDFF_subStereoscopicPar     ,&TsubtitlesSettings::stereoscopicPar         ,-100,100,_l(""),1,
     _l("subStereoscopicPar"), 0,
   IDFF_subDefLang             ,&TsubtitlesSettings::deflang                 ,1,1,_l(""),1,
     _l("subDefLang"), 0,
   IDFF_subDefLang2            ,&TsubtitlesSettings::deflang2                ,1,1,_l(""),1,
     _l("subDefLang2"), 0,
   IDFF_subVobsub              ,&TsubtitlesSettings::vobsub                  ,0,0,_l(""),1,
     _l("subVobsub"), 1,
   IDFF_subVobsubAA            ,&TsubtitlesSettings::vobsubAA                ,0,4,_l(""),1,
     _l("subVobsubAA"), 4,
   IDFF_subVobsubAAswgauss     ,&TsubtitlesSettings::vobsubAAswgauss         ,1,2000,_l(""),1,
     _l("subVobsubAAswgauss"), 700,
   IDFF_subVobsubChangePosition,&TsubtitlesSettings::vobsubChangePosition    ,0,0,_l(""),1,
     _l("subVobsubChangePosition"), 0,
   IDFF_subVobsubScale         ,&TsubtitlesSettings::vobsubScale             ,0x10,0x300,_l(""),1,
     _l("subVobsubScale"), 0x100,
   IDFF_subLinespacing         ,&TsubtitlesSettings::linespacing             ,0,200,_l(""),1,
     _l("subLinespacing"), 100,
   IDFF_subTimeOverlap         ,&TsubtitlesSettings::timeoverlap             ,0,0,_l(""),1,
     _l("subTimeOverlap"), 1,
   IDFF_subIsMinDuration       ,&TsubtitlesSettings::isMinDuration           ,0,0,_l(""),1,
     _l("subIsMinDuration"), 0,
   IDFF_subMinDurationType     ,&TsubtitlesSettings::minDurationType         ,0,2,_l(""),1,
     _l("subMinDurationType"), 1,
   IDFF_subMinDurationSub      ,&TsubtitlesSettings::minDurationSubtitle     ,0,3600000,_l(""),1,
     _l("subMinDurationSubtitle"), 3000,
   IDFF_subMinDurationLine     ,&TsubtitlesSettings::minDurationLine         ,0,3600000,_l(""),1,
     _l("subMinDurationLine"), 1500,
   IDFF_subMinDurationChar     ,&TsubtitlesSettings::minDurationChar         ,0,3600000,_l(""),1,
     _l("subMinDurationChar"), 30,

   IDFF_subFix                 ,&TsubtitlesSettings::fix                     ,1,1,_l(""),1,
     _l("subFix"),0,
   IDFF_subFixLang             ,&TsubtitlesSettings::fixLang                 ,0,5,_l(""),1,
     _l("subFixLang"),0,
   IDFF_subOpacity             ,&TsubtitlesSettings::opacity                 ,0,256,_l(""),1,
     _l("subOpacity"),256,
   IDFF_subSplitBorder         ,&TsubtitlesSettings::splitBorder             ,0,4096,_l(""),1,
     _l("subSplitBorder"),0,
   IDFF_subCC                  ,&TsubtitlesSettings::cc                      ,0,0,_l(""),1,
     _l("subCC"),1,
   IDFF_subWordWrap            ,&TsubtitlesSettings::wordWrap                ,0,2,_l(""),1,
     _l("subWordWrap"),0,
   IDFF_subExtendedTags        ,&TsubtitlesSettings::extendedTags            ,0,0,_l(""),1,
     _l("subExtendedTags"),0,
   0
  };
 addOptions(iopts);
 static const TstrOption sopts[]=
  {
   IDFF_subFilename            ,(TstrVal)&TsubtitlesSettings::flnm           ,MAX_PATH,0 ,_l(""),1,
     _l("subFlnm"),_l(""),
   IDFF_subTempFilename        ,(TstrVal)&TsubtitlesSettings::tempflnm       ,MAX_PATH,0 ,_l(""),1,
     NULL,_l(""),
   IDFF_subFixDict             ,(TstrVal)&TsubtitlesSettings::fixDict        ,60      ,0 ,_l(""),1,
     _l("subFixDict"),_l(""),
   0
  };
 addOptions(sopts);
 tempflnm[0]=0;

 static const TcreateParamList2<Tlang> listLang(langs,&Tlang::desc);setParamList(IDFF_subDefLang,&listLang);setParamList(IDFF_subDefLang2,&listLang);
 static const TcreateParamList1 listVobsubAA(vobsubAAs);setParamList(IDFF_subVobsubAA,&listVobsubAA);
 static const TcreateParamList1 listAlign(alignments);setParamList(IDFF_subAlign,&listAlign);
 static const TcreateParamList1 listDurations(durations);setParamList(IDFF_subMinDurationType,&listDurations);
 static const TcreateParamList1 listIls(fixIls);setParamList(IDFF_subFixLang,&listIls);
 static const TcreateParamList1 listWordWraps(wordWraps);setParamList(IDFF_subWordWrap,&listWordWraps);
}

void TsubtitlesSettings::copy(const TfilterSettings *src)
{
 memcpy(((uint8_t*)this)+sizeof(Toptions),((uint8_t*)src)+sizeof(Toptions),sizeof(*this)-sizeof(font)-sizeof(Toptions));
 font=((TsubtitlesSettings*)src)->font;
}

const char_t* TsubtitlesSettings::getLangDescr(const char_t *lang)
{
 //short lang=(short)Ttranslate::lang2int(langS);
 for (int i=1;langs[i].desc;i++)
  if (langs[i].lang && stricmp(langs[i].lang,lang)==0)
   return langs[i].desc;
 return lang;
}
const char_t* TsubtitlesSettings::getLangDescrIso(const char_t *isolang)
{
 for (int i=0;langs[i].desc;i++)
  if (langs[i].isolang && stricmp(langs[i].isolang,isolang)==0)
   return langs[i].desc;
 return isolang;
}

void TsubtitlesSettings::createFilters(size_t filtersorder,Tfilters *filters,TfilterQueue &queue) const
{
 idffOnChange(idffs,filters,queue.temporary);
 if ((is && show) || filters->isdvdproc)
  {
   TimgFilterSubtitles *sub=queueFilter<TimgFilterSubtitles>(filtersorder,filters,queue);
   if (!queue.temporary)
    {
     setOnChange(IDFF_subAutoFlnm,sub,&TimgFilterSubtitles::onSubFlnmChange);
     setOnChange(IDFF_subTimeOverlap,sub,&TimgFilterSubtitles::onSubFlnmChange);
     setOnChange(IDFF_subIsMinDuration,sub,&TimgFilterSubtitles::onSubFlnmChange);
     setOnChange(IDFF_subMinDurationType,sub,&TimgFilterSubtitles::onSubFlnmChange);
     setOnChange(IDFF_subMinDurationSub,sub,&TimgFilterSubtitles::onSubFlnmChange);
     setOnChange(IDFF_subMinDurationLine,sub,&TimgFilterSubtitles::onSubFlnmChange);
     setOnChange(IDFF_subMinDurationChar,sub,&TimgFilterSubtitles::onSubFlnmChange);
     setOnChange(IDFF_subFix,sub,&TimgFilterSubtitles::onSubFlnmChange);
     setOnChange(IDFF_subFixLang,sub,&TimgFilterSubtitles::onSubFlnmChange);
     setOnChange(IDFF_subFilename,sub,&TimgFilterSubtitles::onSubFlnmChangeStr);
     setOnChange(IDFF_subFixDict,sub,&TimgFilterSubtitles::onSubFlnmChangeStr);
     setOnChange(IDFF_subTempFilename,sub,&TimgFilterSubtitles::onSubFlnmChangeStr);
     setOnChange(IDFF_subWordWrap,sub,&TimgFilterSubtitles::onSubFlnmChange);
    }
  }
}
void TsubtitlesSettings::createPages(TffdshowPageDec *parent) const
{
 parent->addFilterPage<TsubtitlesPage>(&idffs);
 parent->addFilterPage<TsubtitlesTextPage>(&idffs);
 parent->addFilterPage<TfontPageSubtitles>(&idffs);
 parent->addFilterPage<TvobsubPage>(&idffs);
}

void TsubtitlesSettings::getPosHoriz(int x, char_t *s, Twindow *w, int id, size_t len)
{
 const char_t *posS;
 if (x<40) posS=_l("left");
 else if (x>60) posS=_l("right");
 else posS=_l("center");
 tsnprintf_s(s, len, _TRUNCATE, _l("%s %3i%% (%s)"), w ? w->_(id) : _l("Horizontal position:"), x, w ? w->_(id,posS) : posS);
}
void TsubtitlesSettings::getPosVert(int x, char_t *s, Twindow *w, int id, size_t len)
{
 const char_t *posS;
 if (x<40) posS=_l("top");
 else if (x>60) posS=_l("bottom");
 else posS=_l("center");
 tsnprintf_s(s, len, _TRUNCATE, _l("%s %3i%% (%s)"), w ? w->_(id) : _l("Vertical position:"), x, w ? w->_(id,posS) : posS);
}
void TsubtitlesSettings::getExpand(int is,int code,int *x,int *y)
{
 switch (code*(is?1:0))
  {
   case 0:*x=0;*y=0;return;
   case 1:*x=4;*y=3;return;
   case 2:*x=16;*y=9;return;
   default:*x=std::max(1,(int)HIWORD(code));*y=std::max(1,(int)LOWORD(code));return;
  }
}
void TsubtitlesSettings::getExpand(int *x,int *y) const
{
 getExpand(isExpand,expandCode,x,y);
}

bool TsubtitlesSettings::getTip(unsigned int pageId,char_t *buf,size_t len)
{
 if (pageId==1)
  {
   char_t tipS[256];
   char_t horiz[256],vert[256];
   getPosHoriz(posX, horiz, NULL, 0, countof(horiz));
   getPosVert(posY, vert, NULL, 0, countof(vert));
   tsnprintf_s(tipS, countof(tipS), _TRUNCATE, _l("%s, %s"), horiz, vert);
   if (delay!=delayDef)
    strncatf(tipS, countof(tipS), _l("\nDelay: %i ms"),delay);
   if (speed!=speedDef || speed2!=speedDef)
    strncatf(tipS, countof(tipS), _l("\nSpeed: %i/%i"),speed,speed2);
   ff_strncpy(buf,tipS,len);
   buf[len-1]='\0';
  }
 else if (pageId==2)
  font.getTip(buf,len);
 return true;
}

void TsubtitlesSettings::reg_op(TregOp &t)
{
 TfilterSettingsVideo::reg_op(t);
 font.reg_op(t);
 if (isExpand==-1)
  isExpand=expandCode?1:0;
}

const int* TsubtitlesSettings::getResets(unsigned int pageId)
{
 if (pageId==0 || pageId==3)
  {
   static const int idResets[]={
    IDFF_subVobsubAA,IDFF_subVobsubAAswgauss,IDFF_subVobsubChangePosition,IDFF_subVobsubScale,
    0};
   return idResets;
  }
 else
  return NULL;
}
