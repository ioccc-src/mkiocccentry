/*
 * location_tbl - location/country codes
 *
 * "Because there is an I in IOCCC." :-)
 *
 * Copyright (c) 2022,2023 by Landon Curt Noll.  All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright, this permission notice and text
 * this comment, and the disclaimer below appear in all of the following:
 *
 *       supporting documentation
 *       source copies
 *       source works derived from this source
 *       binaries derived from this source or from derived source
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * Share and enjoy! :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


/*
 * location - location/country codes
 */
#include "location.h"


/*
 * location/country codes
 *
 * We added all Officially assigned code elements,
 * all Exceptionally reserved code elements,
 * all Transitionally reserved code elements,
 * all Indeterminately reserved code elements,
 * all Formerly assigned code elements,
 * all User-assigned code elements,
 * as of 2023 June 04.  See:
 *
 *      https://en.wikipedia.org/wiki/ISO_3166-1#Officially_assigned_code_elements
 *      https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2
 *      https://www.iso.org/obp/ui/#iso:pub:PUB500001:en
 *      https://www.iso.org/obp/ui/#search
 *
 * See also:
 *
 *      https://www.iso.org/glossary-for-iso-3166.html
 *
 * This means you will find a few codes that belong to
 * entities that no longer are recognized, entities
 * that are/were temporary, codes that are reserved for
 * future use, codes for Road Traffic Conventions,
 * codes for Property Organizations, duplicate codes, etc.
 *
 * We do not list WIPO codes as they are not formally
 * reserved.  They refer to Patent and related WIPO
 * organizations: as such they do not represent places
 * where an IOCCC winner would live.
 *
 * In a most cases, the "officially known as" name is used.
 * We used Wikipedia as the source of the source of officially
 * known names.  Otherwise we used the common name.
 * See https://en.wikipedia.org/wiki/ISO_3166-2 for common names.
 * We used the "title case" (see https://en.wikipedia.org/wiki/ISO_3166-2)
 * where possible.
 *
 * Because mkiocccentry(1) recommends using the ISO 3166 code:
 *
 *	XX
 *
 * for specifying an anonymous location, the formal name of:
 *
 *	User-assigned code XX
 *
 * was renamed:
 *
 *	Anonymous location
 *
 * We mean no offense by this list: we simply tried to
 * include all ISO 3166 codes.  Please pardon any typos.
 */
struct location loc[] = {
    {"AA", "User-assigned code AA", "User-assigned code AA"},	/* User-assigned code */
    {"AC", "Ascension Island", "Ascension Island"},	/* Exceptionally reserved code */
    {"AD", "Principality of Andorra", "Andorra"},
    {"AE", "United Arab Emirates", "United Arab Emirates"},
    {"AF", "Islamic Emirate of Afghanistan", "Afghanistan"},
    {"AG", "Antigua and Barbuda", "Antigua and Barbuda"},
    {"AI", "Anguilla", "Anguilla"},
    {"AL", "Republic of Albania", "Albania"},
    {"AM", "Republic of Armenia,", "Armenia,"},
    {"AN", "Netherlands Antilles", "Netherlands Antilles"},	/* Transitionally reserved code */
    {"AO", "Republic of Angola", "Angola"},
/*  {"AP", "African Regional Industrial Property Organization", "African Regional Industrial Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"AQ", "Antarctica", "Antarctica"},
    {"AR", "Argentine Republic", "Argentina"},
    {"AS", "American Samoa", "American Samoa"},
    {"AT", "Republic of Austria", "Austria"},
    {"AU", "Commonwealth of Australia", "Australia"},
    {"AW", "Country of Aruba", "Aruba"},
    {"AX", "\xc3\x85land Islands", "\xc3\x85land Islands"},
    {"AZ", "Republic of Azerbaijan", "Azerbaijan"},
    {"BA", "Bosnia and Herzegovina", "Bosnia and Herzegovina"},
    {"BB", "Barbados", "Barbados"},
    {"BD", "People's Republic of Bangladesh", "Bangladesh"},
    {"BE", "Kingdom of Belgium", "Belgium"},
    {"BF", "Burkina Faso", "Burkina Faso"},
    {"BG", "Republic of Bulgaria", "Bulgaria"},
    {"BH", "Kingdom of Bahrain", "Bahrain"},
    {"BI", "Republic of Burundi", "Republic of Burundi"},
    {"BJ", "Republic of Benin", "Benin"},
    {"BL", "Collectivit\xc3\xa9 territoriale de Saint-Barth\xc3\xa9lemy", "Saint Barth\xc3\xa9lemy"},
    {"BM", "Bermuda", "Bermuda"},
    {"BN", "Brunei Darussalam", "Brunei Darussalam"},
    {"BO", "Plurinational State of Bolivia", "Bolivia"},
    {"BQ", "Caribbean Netherlands", "Bonaire, Sint Eustatius and Saba"},
    {"BR", "Federative Republic of Brazil", "Brazil"},
    {"BS", "Commonwealth of The Bahamas", "Bahamas"},
    {"BT", "Kingdom of Bhutan", "Bhutan"},
    {"BU", "Burma", "Burma"},		/* Transitionally reserved code */
    {"BV", "Bouvet Island", "Bouvet Island"},
    {"BW", "Republic of Botswana", "Botswana"},
/*  {"BX", "Benelux Trademarks and Designs Office", "Benelux Trademarks and Designs Office"}, */ /* WIPO Indeterminately reserved code */
    {"BY", "Republic of Belarus", "Belarus"},
    {"BZ", "Belize", "Belize"},
    {"CA", "Canada", "Canada"},
    {"CC", "Territory of Cocos (Keeling) Islands", "Cocos (Keeling) Islands"},
    {"CD", "Democratic Republic of the Congo", "Democratic Republic of the Congo"},
    {"CF", "Ubangi-Shari", "Central African Republic"},
    {"CG", "Republic of the Congo", "Congo"},
    {"CH", "Swiss Confederation", "Switzerland"},
    {"CI", "Republic of C\xc3\xb4te d'Ivoire", "C\xc3\xb4te d'Ivoire"},
    {"CK", "Cook Islands", "Cook Islands"},
    {"CL", "Republic of Chile", "Chile"},
    {"CM", "Republic of Cameroon", "Cameroon"},
    {"CN", "People's Republic of China", "China"},
    {"CO", "Republic of Colombia", "Colombia"},
    {"CP", "Clipperton Island", "Clipperton Island"},	/* Exceptionally reserved code */
    {"CQ", "Island of Sark", "Sark"},	/* Exceptionally reserved code */
    {"CR", "Republic of Costa Rica", "Costa Rica"},
    {"CS", "Federal Republic of Yugoslavia", "Serbia and Montenegro"},	/* Transitionally reserved code */
    {"CT", "Canton and Enderbury Island", "Canton and Enderbury Island"},	/* Formerly assigned code */
    {"CU", "Republic of Cuba", "Cuba"},
    {"CV", "Republic of Cabo Verde", "Cabo Verde"},
    {"CW", "Country of Cura\xc3\xa7", "Cura\xc3\xa7"},
    {"CX", "Territory of Christmas Island", "Christmas Island"},
    {"CY", "Republic of Cyprus", "Cyprus"},
    {"CZ", "Czech Republic", "Czechia"},
    {"DD", "German Democratic Republic", "East Germany"},	/* Formerly assigned code */
    {"DE", "Federal Republic of Germany", "Germany"},
    {"DG", "Diego Garcia", "Diego Garcia"},	/* Exceptionally reserved code */
    {"DJ", "Republic of Djibouti", "Djibouti"},
    {"DK", "Denmark", "Denmark"},
    {"DM", "Commonwealth of Dominica", "Dominica"},
    {"DO", "Dominican Republic", "Dominican Republic"},
    {"DY", "Republic of Benin", "Benin"},		/* Indeterminately reserved code */
    {"DZ", "People's Democratic Republic of Algeria", "Algeria"},
    {"EA", "Ceuta and Melilla", "Ceuta and Melilla"},	/* Exceptionally reserved code */
    {"EC", "Republic of Ecuador", "Ecuador"},
    {"EE", "Republic of Estonia", "Estonia"},
/*  {"EF", "Union of Countries under the European Community Patent Convention", "Union of Countries under the European Community Patent Convention"}, */ /* WIPO Indeterminately reserved code */
    {"EG", "Arab Republic of Egypt", "Egypt"},
    {"EH", "Western Sahara", "Western Sahara"},
/*  {"EM", "European Trademark Office", "European Trademark Office"}, */ /* WIPO Indeterminately reserved code */
/*  {"EP", "European Patent Organization", "European Patent Organization"}, */ /* WIPO Indeterminately reserved code */
    {"ER", "State of Eritrea", "Eritrea"},
    {"ES", "Kingdom of Spain", "Spain"},
    {"ET", "Federal Democratic Republic of Ethiopia", "Ethiopia"},
    {"EU", "European Union", "European Union"},	/* Exceptionally reserved code */
/*  {"EV", "Eurasian Patent Organization", "Eurasian Patent Organization"}, */ /* WIPO Indeterminately reserved code */
    {"EW", "Republic of Estonia", "Estonia"},		/* Indeterminately reserved code */
    {"EZ", "European OTC derivatives", "European OTC derivatives"},	/* Exceptionally reserved code */
    {"FI", "Republic of Finland ", "Finland"},
    {"FJ", "Republic of Fiji", "Fiji"},
    {"FK", "Falkland Islands", "Falkland Islands"},
    {"FL", "Principality of Liechtenstein", "Liechtenstein"},	/* Indeterminately reserved code */
    {"FM", "Federated States of Micronesia", "Micronesia"},
    {"FO", "Faroe Islands", "Faroes"},
    {"FQ", "French Southern and Antarctic Territories", "French Southern Lands"},	/* Formerly assigned code */
    {"FR", "French Republic", "France"},
    {"FX", "France, Metropolitan", "France, Metropolitan"},	/* Exceptionally reserved code */
    {"GA", "Gabonese Republic", "Gabon"},
    {"GB", "United Kingdom of Great Britain and Northern Ireland", "United Kingdom"},
/*  {"GC", "Patent Office of the Cooperation Council for the Arab States of the Gulf", "Patent Office of the Cooperation Council for the Arab States of the Gulf"}, */ /* WIPO Indeterminately reserved code */
    {"GD", "Grenada", "Grenada"},
    {"GE", "Georgia", "Georgia"},
    {"GF", "French Guiana", "French Guiana"},
    {"GG", "Bailiwick of Guernsey", "Guernsey"},
    {"GH", "Republic of Ghana", "Ghana"},
    {"GI", "Gibraltar", "Gibraltar"},
    {"GL", "Greenland", "Greenland"},
    {"GM", "Republic of The Gambia", "Gambia"},
    {"GN", "Republic of Guinea", "Guinea"},
    {"GP", "Guadeloupe", "Guadeloupe"},
    {"GQ", "Republic of Equatorial Guinea", "Equatorial Guinea"},
    {"GR", "Hellenic Republic", "Greece"},
    {"GS", "South Georgia and the South Sandwich Islands", "South Georgia and the South Sandwich Islands"},
    {"GT", "Republic of Guatemala", "Guatemala"},
    {"GU", "Guam", "Guam"},
    {"GW", "Republic of Guinea-Bissau", "Guinea-Bissau"},
    {"GY", "Co-operative Republic of Guyana", "Guyana"},
    {"HK", "Hong Kong Special Administrative Region of the People's Republic of China", "Hong Kong"},
    {"HM", "Territory of Heard Island and McDonald Islands", "Heard Island and McDonald Islands"},
    {"HN", "Republic of Honduras", "Honduras"},
    {"HR", "Republic of Croatia", "Croatia"},
    {"HT", "Republic of Croatia", "Haiti"},
    {"HU", "Hungary", "Hungary"},
    {"HV", "French Upper Volta", "Upper Volta"},	/* Formerly assigned code */
/*  {"IB", "International Bureau of WIPO", "International Bureau of WIPO"}, */ /* WIPO Indeterminately reserved code */
    {"IC", "Canary Islands", "the Canaries"},	/* Exceptionally reserved code */
    {"ID", "Republic of Indonesia", "Indonesia"},
    {"IE", "Republic of Ireland", "Ireland"},
    {"IL", "State of Israel", "Israel"},
    {"IM", "Isle of Man", "Mann"},
    {"IN", "Republic of India", "India"},
    {"IO", "British Indian Ocean Territory", "British Indian Ocean Territory"},
    {"IQ", "Republic of Iraq,", "Iraq"},
    {"IR", "Islamic Republic of Iran", "Iran"},
    {"IS", "Iceland", "Iceland"},
    {"IT", "Italian Republic", "Italy"},
    {"JA", "Jamaica", "Jamaica"},		/* Indeterminately reserved code */
    {"JE", "Bailiwick of Jersey", "Jersey"},
    {"JM", "Jamaica", "Jamaica"},
    {"JO", "Hashemite Kingdom of Jordan", "Jordan"},
    {"JP", "Japan", "Japan"},
    {"JT", "Johnston Island", "Johnston Island"},	/* Formerly assigned code */
    {"KE", "Republic of Kenya", "Kenya"},
    {"KG", "Kyrgyz Republic", "Kyrgyzstan"},
    {"KH", "Kingdom of Cambodia", "Cambodia"},
    {"KI", "Republic of Kiribati", "Kiribati"},
    {"KM", "Union of the Comoros", "Comoros"},
    {"KN", "Federation of Saint Christopher and Nevis", "Saint Kitts and Nevis"},
    {"KP", "Democratic People's Republic of Korea", "North Korea"},
    {"KR", "Republic of Korea", "South Korea"},
    {"KW", "State of Kuwait", "Kuwait"},
    {"KY", "Cayman Islands", "Cayman Islands"},
    {"KZ", "Republic of Kazakhstan", "Kazakhstan"},
    {"LA", "Lao People's Democratic Republic", "Lao People's Democratic Republic"},
    {"LB", "Republic of Lebanon", "Lebanon"},
    {"LC", "Saint Lucia", "Saint Lucia"},
    {"LF", "Libya Fezzan", "Fezzan"},	/* Indeterminately reserved code */
    {"LI", "Principality of Liechtenstein", "Liechtenstein"},
    {"LK", "Democratic Socialist Republic of Sri Lanka", "Sri Lanka"},
    {"LR", "Republic of Liberia", "Liberia"},
    {"LS", "Kingdom of Lesotho", "Lesotho"},
    {"LT", "Republic of Lithuania", "Lithuania"},
    {"LU", "Grand Duchy of Luxembourg", "Luxembourg"},
    {"LV", "Republic of Latvia", "Latvia"},
    {"LY", "State of Libya", "Libya"},
    {"MA", "Kingdom of Morocco", "Morocco"},
    {"MC", "Principality of Monaco", "Monaco"},
    {"MD", "Republic of Moldova", "Moldova"},
    {"ME", "Montenegro", "Montenegro"},
    {"MF", "Collectivity of Saint Martin", "Saint Martin"},
    {"MG", "Republic of Madagascar", "Madagascar"},
    {"MH", "Republic of the Marshall Islands", "Marshall Islands"},
    {"MI", "Midway Atoll", "Midway Islands"},	/* Formerly assigned code */
    {"MK", "Republic of North Macedonia", "North Macedonia"},
    {"ML", "Republic of Mali", "Mali"},
    {"MM", "Republic of the Union of Myanmar", "Myanmar"},
    {"MN", "Mongolia", "Mongolia"},
    {"MO", "Macao Special Administrative Region of the People's Republic of China", "Macao"},
    {"MP", "Commonwealth of the Northern Mariana Islands", "Northern Mariana Islands"},
    {"MQ", "Martinique", "Martinique"},
    {"MR", "Islamic Republic of Mauritania", "Mauritania"},
    {"MS", "Montserrat", "Montserrat"},
    {"MT", "Republic of Malta", "Malta"},
    {"MU", "Republic of Mauritius", "Mauritius"},
    {"MV", "Republic of Maldives", "Maldives"},
    {"MW", "Republic of Malawi", "Malawi"},
    {"MX", "United Mexican States", "Mexico"},
    {"MY", "Malaysia", "Malaysia"},
    {"MZ", "Republic of Mozambique", "Mozambique"},
    {"NA", "Republic of Namibia", "Namibia"},
    {"NC", "New Caledonia", "New Caledonia"},
    {"NE", "Republic of the Niger", "Niger"},
    {"NF", "Territory of Norfolk Island", "Norfolk Island"},
    {"NG", "Federal Republic of Nigeria", "Nigeria"},
    {"NH", "New Hebrides Condominium", "New Hebrides"},	/* Formerly assigned code */
    {"NI", "Republic of Nicaragua", "Nicaragua"},
    {"NL", "Kingdom of the Netherlands", "Netherlands"},
    {"NO", "Kingdom of Norway", "Norway"},
    {"NP", "Federal Democratic Republic of Nepal", "Nepal"},
    {"NQ", "Dronning Maud Land", "Dronning Maud Land"},	/* Formerly assigned code */
    {"NR", "Republic of Nauru", "Nauru"},
    {"NT", "Neutral Zone", "Neutral Zone"},	/* Transitionally reserved code */
    {"NU", "Niue", "Niue"},
    {"NZ", "New Zealand", "New Zealand"},
/*  {"OA", "African Intellectual Property Organization", "African Intellectual Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"OO", "user-assigned escape code", "user-assigned escape code"},	/* User-assigned code */
    {"OM", "Sultanate of Oman", "Oman"},
    {"PA", "Republic of Panama", "Panama"},
    {"PC", "Trust Territory of the Pacific Islands", "Pacific Islands Trust Territory"},	/* Formerly assigned code */
    {"PE", "Republic of Peru", "Peru"},
    {"PF", "French Polynesia", "French Polynesia"},
    {"PG", "Independent State of Papua New Guinea", "Papua New Guinea"},
    {"PH", "Republic of the Philippines", "Philippines"},
    {"PI", "Republic of the Philippines", "Philippines"},	/* Indeterminately reserved code */
    {"PK", "Islamic Republic of Pakistan", "Pakistan"},
    {"PL", "Republic of Poland", "Poland"},
    {"PM", "Territorial Collectivity of Saint-Pierre and Miquelon", "Saint Pierre and Miquelon"},
    {"PN", "Pitcairn, Henderson, Ducie and Oeno Islands", "Pitcairn"},
    {"PR", "Commonwealth of Puerto Rico", "Puerto Rico"},
    {"PS", "State of Palestine", "Palestine"},
    {"PT", "Portuguese Republic", "Portugal"},
    {"PU", "United States Miscellaneous Pacific Islands", "United States Miscellaneous Pacific Islands"},	/* Formerly assigned code */
    {"PW", "Republic of Palau,", "Palau"},
    {"PY", "Republic of Paraguay", "Paraguay"},
    {"PZ", "Panama Canal Zone", "Canal Zone"},	/* Formerly assigned code */
    {"QA", "State of Qatar", "Qatar"},
    {"QM", "User-assigned code QM", "User-assigned code QM"},	/* User-assigned code */
    {"QN", "User-assigned code QN", "User-assigned code QN"},	/* User-assigned code */
    {"QO", "User-assigned code QO", "User-assigned code QO"},	/* User-assigned code */
    {"QP", "User-assigned code QP", "User-assigned code QP"},	/* User-assigned code */
    {"QQ", "User-assigned code QQ", "User-assigned code QQ"},	/* User-assigned code */
    {"QR", "User-assigned code QR", "User-assigned code QR"},	/* User-assigned code */
    {"QS", "User-assigned code QS", "User-assigned code QS"},	/* User-assigned code */
    {"QT", "User-assigned code QT", "User-assigned code QT"},	/* User-assigned code */
    {"QU", "User-assigned code QU", "User-assigned code QU"},	/* User-assigned code */
    {"QV", "User-assigned code QV", "User-assigned code QV"},	/* User-assigned code */
    {"QW", "User-assigned code QW", "User-assigned code QW"},	/* User-assigned code */
    {"QX", "User-assigned code QX", "User-assigned code QX"},	/* User-assigned code */
    {"QY", "User-assigned code QY", "User-assigned code QY"},	/* User-assigned code */
    {"QZ", "User-assigned code QZ", "User-assigned code QZ"},	/* User-assigned code */
    {"RA", "Argentine Republic", "Argentina"},	/* Indeterminately reserved code */
    {"RB", "Bolivia or Botswana", "Bolivia or Botswana"},	/* Indeterminately reserved code */
    {"RC", "China", "China"},		/* Indeterminately reserved code */
    {"RE", "Department of R\xc3\xa9union", "R\xc3\xa9union"},
    {"RH", "Republic of Haiti", "Haiti"},		/* Indeterminately reserved code */
    {"RI", "Republic of Indonesia", "Indonesia"},	/* Indeterminately reserved code */
    {"RL", "Republic of Lebanon", "Lebanon"},		/* Indeterminately reserved code */
    {"RM", "Republic of Madagascar", "Madagascar"},	/* Indeterminately reserved code */
    {"RN", "Republic of the Niger", "Niger"},		/* Indeterminately reserved code */
    {"RO", "Romania", "Romania"},
    {"RP", "Republic of the Philippines", "Philippines"},	/* Indeterminately reserved code */
    {"RS", "Republic of Serbia", "Serbia"},
    {"RU", "Russian Federation", "Russia"},
    {"RW", "Republic of Rwanda", "Rwanda"},
    {"SA", "Kingdom of Saudi Arabia", "Saudi Arabia"},
    {"SB", "Solomon Islands", "Solomon Islands"},
    {"SC", "Republic of Seychelles", "Seychelles"},
    {"SD", "Republic of the Sudan", "Sudan"},
    {"SE", "Kingdom of Sweden", "Sweden"},
    {"SF", "Finland", "Finland"},		/* Indeterminately reserved code */
    {"SG", "Republic of Singapore", "Singapore"},
    {"SH", "Saint Helena, Ascension and Tristan da Cunha", "Saint Helena"},
    {"SI", "Republic of Slovenia", "Slovenia"},
    {"SJ", "Svalbard and Jan Mayen", "Svalbard and Jan Mayen"},
    {"SK", "Slovak Republic", "Slovakia"},
    {"SL", "Republic of Sierra Leone", "Sierra Leone"},
    {"SM", "Republic of San Marino", "San Marino"},
    {"SN", "Republic of Senegal", "Senegal"},
    {"SO", "Federal Republic of Somalia", "Somalia"},
    {"SR", "Republic of Suriname", "Suriname"},
    {"SS", "Republic of South Sudan", "South Sudan"},
    {"ST", "Democratic Republic of Sa\xe2 Tom\xe9 and Pr\xedncipe", "Sao Tome and Principe"},
    {"SU", "Union of Soviet Socialist Republics", "Soviet Union"},		/* Exceptionally reserved code */
    {"SV", "Republic of El Salvador", "El Salvador"},
    {"SX", "Sint Maarten", "Sint Maarten"},
    {"SY", "Syrian Arab Republic", "Syria"},
    {"SZ", "Kingdom of Eswatini", "Eswatini"},
    {"TA", "Tristan da Cunha", "Tristan"},	/* Exceptionally reserved code */
    {"TC", "Turks and Caicos Islands", "Turks and Caicos Islands"},
    {"TD", "Republic of Chad", "Chad"},
    {"TF", "French Southern and Antarctic Lands", "French Southern Territories"},
    {"TG", "Togolese Republic", "Togo"},
    {"TH", "Kingdom of Thailand", "Thailand"},
    {"TJ", "Republic of Tajikistan", "Tajikistan"},
    {"TK", "Tokelau Islands", "Tokelau"},
    {"TL", "Democratic Republic of Timor-Leste", "East Timor"},
    {"TM", "Turkmenistan", "Turkmenistan"},
    {"TN", "Republic of Tunisia", "Tunisia"},
    {"TO", "Kingdom of Tonga", "Tonga"},
    {"TP", "Republic of T\xc3\x9crkiye", "Turkey"},		/* Transitionally reserved code */
    {"TR", "Republic of T\xc3\x9crkiye", "Turkey"},
    {"TT", "Republic of Trinidad and Tobago", "Trinidad and Tobago"},
    {"TV", "Tuvalu", "Tuvalu"},
    {"TW", "Taiwan", "Taiwan"},		/* set the name and common_name to be the same for this location */
    {"TZ", "United Republic of Tanzania", "Tanzania"},
    {"UA", "Ukraine", "Ukraine"},
    {"UG", "Republic of Uganda", "Uganda"},
    {"UK", "United Kingdom of Great Britain and Northern Ireland", "United Kingdom"},	/* Exceptionally reserved code */
    {"UM", "United States Minor Outlying Islands", "United States Minor Outlying Islands"},
    {"UN", "United Nations", "United Nations"},	/* Exceptionally reserved code */
    {"US", "United States of America", "United States"},
    {"UY", "Oriental Republic of Uruguay", "Uruguay"},
    {"UZ", "Republic of Uzbekistan", "Uzbekistan"},
    {"VA", "Vatican City State", "Vatican"},
    {"VC", "Saint Vincent and the Grenadines", "Saint Vincent and the Grenadines"},
    {"VD", "Democratic Republic of Viet-Nam", "North Vietnam"},	/* Formerly assigned code */
    {"VE", "Bolivarian Republic of Venezuela", "Venezuela"},
    {"VG", "British Virgin Islands", "British Virgin Islands"},
    {"VI", "United States Virgin Islands", "US Virgin Islands"},
    {"VN", "Socialist Republic of Vietnam", "Viet Nam"},
    {"VU", "Republic of Vanuatu", "Vanuatu"},
    {"WF", "Territory of the Wallis and Futuna Islands", "Wallis and Futuna"},
    {"WG", "Grenada", "Grenada"},		/* Indeterminately reserved code */
    {"WK", "Wake Island", "Wake Island"},	/* Formerly assigned code */
    {"WL", "Saint Lucia", "Saint Lucia"},	/* Indeterminately reserved code */
/*  {"WO", "World Intellectual Property Organization", "World Intellectual Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"WS", "Independent State of Samoa", "Samoa"},
    {"WV", "Saint Vincent", "Saint Vincent"},	/* Indeterminately reserved code */
    {"XA", "Canary Islands", "the Canaries"},		/* User-assigned code - XA is being used by Switzerland */
    {"XB", "User-assigned code XB", "User-assigned code XB"},	/* User-assigned code */
    {"XC", "User-assigned code XC", "User-assigned code XC"},	/* User-assigned code */
    {"XD", "User-assigned code XD", "User-assigned code XD"},	/* User-assigned code */
    {"XE", "User-assigned code XE", "User-assigned code XE"},	/* User-assigned code */
    {"XF", "User-assigned code XF", "User-assigned code XF"},	/* User-assigned code */
    {"XG", "User-assigned code XG", "User-assigned code XG"},	/* User-assigned code */
    {"XH", "User-assigned code XH", "User-assigned code XH"},	/* User-assigned code */
    {"XI", "Northern Ireland", "Northern Ireland"},		/* User-assigned code - used by the UK Government as an EORI code prefix */
    {"XJ", "User-assigned code XJ", "User-assigned code XJ"},	/* User-assigned code */
    {"XK", "Kosovo", "Kosovo"},			/* User-assigned code - used by the European Commission for Kosovo */
    {"XL", "User-assigned code XL", "User-assigned code XL"},	/* User-assigned code */
    {"XM", "User-assigned code XM", "User-assigned code XM"},	/* User-assigned code */
    {"XN", "User-assigned code XN", "User-assigned code XN"},	/* User-assigned code */
    {"XO", "User-assigned code XO", "User-assigned code XO"},	/* User-assigned code */
    {"XP", "User-assigned code XP", "User-assigned code XP"},	/* User-assigned code */
    {"XR", "User-assigned code XR", "User-assigned code XR"},	/* User-assigned code */
    {"XS", "User-assigned code XS", "User-assigned code XS"},	/* User-assigned code */
    {"XT", "User-assigned code XT", "User-assigned code XT"},	/* User-assigned code */
    {"XU", "User-assigned code XU", "User-assigned code XU"},	/* User-assigned code */
    {"XV", "User-assigned code XV", "User-assigned code XV"},	/* User-assigned code */
    {"XW", "User-assigned code XW", "User-assigned code XW"},	/* User-assigned code */
    {"XX", "Anonymous location", "Anonymous location"},	/* User-assigned code - used by IOCCC for Anonymous location */
    {"XY", "User-assigned code XY", "User-assigned code XY"},	/* User-assigned code */
    {"XZ", "International waters", "International waters"},	/* User-assigned code - UN/LOCODE assigns XZ to represent international waters */
    {"YD", "People's Democratic Republic of Yemen", "South Yemen"},	/* User-assigned code - Formerly assigned code */
    {"YE", "Republic of Yemen", "Yemen"},
    {"YT", "Department of Mayotte", "Mayotte"},
    {"YU", "Yugoslavia", "Yugoslavia"},	/* Transitionally reserved code */
    {"YV", "Bolivarian Republic of Venezuela", "Venezuela"},	/* Indeterminately reserved code */
    {"ZA", "Republic of South Africa", "South Africa"},
    {"ZM", "Republic of Zambia", "Zambia"},
    {"ZR", "Republic of Zaire", "Zaire"},		/* User-assigned code - Transitionally reserved code */
    {"ZW", "Republic of Zimbabwe", "Zimbabwe"},
    {"ZZ", "Unknown location", "Unknown location"},	/* User-assigned code - used by IOCCC for Unknown location */
    {NULL, NULL, NULL}		/* MUST BE LAST */
};

size_t SIZEOF_LOCATION_TABLE = TBLLEN(loc);
