/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * location - location/country codes
 *
 * "Because there is an I in IOCCC." :-)
 *
 * Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */

#include <stdio.h>

/*
 * location
 *
 * #includes the header files we need.
 */
#include "location.h"


/*
 * location/country codes
 *
 * We list all Officially assigned code elements,
 * all Exceptionally reserved code elements,
 * all Transitionally reserved code elements,
 * all Indeterminately reserved code elements,
 * all Formerly assigned code elements,
 * all User-assigned code elements,
 * as of 2021 Dec 31.  See:
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
 * We mean no offense by this list: we simply tried to
 * include all ISO 3166 codes.  Please pardon any typos.
 * Spelling corrections welcome.
 *
 */
struct location loc[] = {
    {"AA", "User-assigned code AA"},	/* User-assigned code */
    {"AC", "Ascension Island"},	/* Exceptionally reserved code */
    {"AD", "Andorra"},
    {"AE", "United Arab Emirates"},
    {"AF", "Afghanistan"},
    {"AG", "Antigua and Barbuda"},
    {"AI", "Anguilla"},
    {"AL", "Albania"},
    {"AM", "Armenia"},
    {"AN", "Netherlands Antilles"},	/* Transitionally reserved code */
    {"AO", "Angola"},
/*  {"AP", "African Regional Industrial Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"AQ", "Antarctica"},
    {"AR", "Argentina"},
    {"AS", "American Samoa"},
    {"AT", "Austria"},
    {"AU", "Australia"},
    {"AW", "Aruba"},
    {"AX", "\xc3\x85land Islands"},
    {"AZ", "Azerbaijan"},
    {"BA", "Bosnia and Herzegovina"},
    {"BB", "Barbados"},
    {"BD", "Bangladesh"},
    {"BE", "Belgium"},
    {"BF", "Burkina Faso"},
    {"BG", "Bulgaria"},
    {"BH", "Bahrain"},
    {"BI", "Burundi"},
    {"BJ", "Benin"},
    {"BL", "Saint Barth\xc3\xa9lemy"},
    {"BM", "Bermuda"},
    {"BN", "Brunei Darussalam"},
    {"BO", "Bolivia (Plurinational State of)"},
    {"BQ", "Bonaire, Sint Eustatius and Saba"},
    {"BR", "Brazil"},
    {"BS", "Bahamas"},
    {"BT", "Bhutan"},
    {"BU", "Burma"},		/* Transitionally reserved code */
    {"BV", "Bouvet Island"},
    {"BW", "Botswana"},
/*  {"BX", "Benelux Trademarks and Designs Office"}, */ /* WIPO Indeterminately reserved code */
    {"BY", "Belarus"},
    {"BZ", "Belize"},
    {"CA", "Canada"},
    {"CC", "Cocos (Keeling) Islands (the)"},
    {"CD", "Congo, Democratic Republic of the"},
    {"CF", "Central African Republic"},
    {"CG", "Congo (the)"},
    {"CH", "Switzerland"},
    {"CI", "C\xc3\xb4te d'Ivoire"},
    {"CK", "Cook Islands"},
    {"CL", "Chile"},
    {"CM", "Cameroon"},
    {"CN", "China"},
    {"CO", "Colombia"},
    {"CP", "Clipperton Island"},	/* Exceptionally reserved code */
    {"CQ", "island of Sark"},	/* Exceptionally reserved code */
    {"CR", "Costa Rica"},
    {"CS", "Serbia and Montenegro"},	/* Transitionally reserved code */
    {"CT", "Canton and Enderbury Island"},	/* Formerly assigned code */
    {"CU", "Cuba"},
    {"CV", "Cabo Verde"},
    {"CW", "Cura\xc3\xa7" "ao"},
    {"CX", "Christmas Island"},
    {"CY", "Cyprus"},
    {"CZ", "Czechia"},
    {"DD", "German Democratic Republic"},	/* Formerly assigned code */
    {"DE", "Germany"},
    {"DG", "Diego Garcia"},	/* Exceptionally reserved code */
    {"DJ", "Djibouti"},
    {"DK", "Denmark"},
    {"DM", "Dominica"},
    {"DO", "Dominican Republic"},
    {"DY", "Benin"},		/* Indeterminately reserved code */
    {"DZ", "Algeria"},
    {"EA", "Ceuta, Melilla"},	/* Exceptionally reserved code */
    {"EC", "Ecuador"},
    {"EE", "Estonia"},
/*  {"EF", "Union of Countries under the European Community Patent Convention"}, */ /* WIPO Indeterminately reserved code */
    {"EG", "Egypt"},
    {"EH", "Western Sahara"},
/*  {"EM", "European Trademark Office"}, */ /* WIPO Indeterminately reserved code */
/*  {"EP", "European Patent Organization"}, */ /* WIPO Indeterminately reserved code */
    {"ER", "Eritrea"},
    {"ES", "Spain"},
    {"ET", "Ethiopia"},
    {"EU", "European Union"},	/* Exceptionally reserved code */
/*  {"EV", "Eurasian Patent Organization"}, */ /* WIPO Indeterminately reserved code */
    {"EW", "Estonia"},		/* Indeterminately reserved code */
    {"EZ", "European OTC derivatives"},	/* Exceptionally reserved code */
    {"FI", "Finland"},
    {"FJ", "Fiji"},
    {"FK", "Falkland Islands (the) [Malvinas]"},
    {"FL", "Liechtenstein"},	/* Indeterminately reserved code */
    {"FM", "Micronesia (Federated States of)"},
    {"FO", "Faroe Islands"},
    {"FQ", "French Southern and Antarctic Territories"},	/* Formerly assigned code */
    {"FR", "France"},
    {"FX", "France, Metropolitan"},	/* Exceptionally reserved code */
    {"GA", "Gabon"},
    {"GB", "United Kingdom of Great Britain and Northern Ireland (the)"},
/*  {"GC", "Patent Office of the Cooperation Council for the Arab States of the Gulf"}, */ /* WIPO Indeterminately reserved code */
    {"GD", "Grenada"},
    {"GE", "Georgia"},
    {"GF", "French Guiana"},
    {"GG", "Guernsey"},
    {"GH", "Ghana"},
    {"GI", "Gibraltar"},
    {"GL", "Greenland"},
    {"GM", "Gambia"},
    {"GN", "Guinea"},
    {"GP", "Guadeloupe"},
    {"GQ", "Equatorial Guinea"},
    {"GR", "Greece"},
    {"GS", "South Georgia and the South Sandwich Islands"},
    {"GT", "Guatemala"},
    {"GU", "Guam"},
    {"GW", "Guinea-Bissau"},
    {"GY", "Guyana"},
    {"HK", "Hong Kong"},
    {"HM", "Heard Island and McDonald Islands"},
    {"HN", "Honduras"},
    {"HR", "Croatia"},
    {"HT", "Haiti"},
    {"HU", "Hungary"},
    {"HV", "Upper Volta"},	/* Formerly assigned code */
/*  {"IB", "International Bureau of WIPO"}, */ /* WIPO Indeterminately reserved code */
    {"IC", "Canary Islands"},	/* Exceptionally reserved code */
    {"ID", "Indonesia"},
    {"IE", "Ireland"},
    {"IL", "Israel"},
    {"IM", "Isle of Man"},
    {"IN", "India"},
    {"IO", "British Indian Ocean Territory"},
    {"IQ", "Iraq"},
    {"IR", "Iran (Islamic Republic of)"},
    {"IS", "Iceland"},
    {"IT", "Italy"},
    {"JA", "Jamaica"},		/* Indeterminately reserved code */
    {"JE", "Jersey"},
    {"JM", "Jamaica"},
    {"JO", "Jordan"},
    {"JP", "Japan"},
    {"JT", "Johnston Island"},	/* Formerly assigned code */
    {"KE", "Kenya"},
    {"KG", "Kyrgyzstan"},
    {"KH", "Cambodia"},
    {"KI", "Kiribati"},
    {"KM", "Comoros"},
    {"KN", "Saint Kitts and Nevis"},
    {"KP", "Korea (the Democratic People's Republic of)"},
    {"KR", "Korea (the Republic of)"},
    {"KW", "Kuwait"},
    {"KY", "Cayman Islands"},
    {"KZ", "Kazakhstan"},
    {"LA", "Lao People's Democratic Republic"},
    {"LB", "Lebanon"},
    {"LC", "Saint Lucia"},
    {"LF", "Libya Fezzan"},	/* Indeterminately reserved code */
    {"LI", "Liechtenstein"},
    {"LK", "Sri Lanka"},
    {"LR", "Liberia"},
    {"LS", "Lesotho"},
    {"LT", "Lithuania"},
    {"LU", "Luxembourg"},
    {"LV", "Latvia"},
    {"LY", "Libya"},
    {"MA", "Morocco"},
    {"MC", "Monaco"},
    {"MD", "Moldova (the Republic of)"},
    {"ME", "Montenegro"},
    {"MF", "Saint Martin (French part)"},
    {"MG", "Madagascar"},
    {"MH", "Marshall Islands (the)"},
    {"MI", "Midway Islands"},	/* Formerly assigned code */
    {"MK", "North Macedonia"},
    {"ML", "Mali"},
    {"MM", "Myanmar"},
    {"MN", "Mongolia"},
    {"MO", "Macao"},
    {"MP", "Northern Mariana Islands"},
    {"MQ", "Martinique"},
    {"MR", "Mauritania"},
    {"MS", "Montserrat"},
    {"MT", "Malta"},
    {"MU", "Mauritius"},
    {"MV", "Maldives"},
    {"MW", "Malawi"},
    {"MX", "Mexico"},
    {"MY", "Malaysia"},
    {"MZ", "Mozambique"},
    {"NA", "Namibia"},
    {"NC", "New Caledonia"},
    {"NE", "Niger"},
    {"NF", "Norfolk Island"},
    {"NG", "Nigeria"},
    {"NH", "New Hebrides"},	/* Formerly assigned code */
    {"NI", "Nicaragua"},
    {"NL", "Netherlands"},
    {"NO", "Norway"},
    {"NP", "Nepal"},
    {"NQ", "Dronning Maud Land"},	/* Formerly assigned code */
    {"NR", "Nauru"},
    {"NT", "Neutral Zone"},	/* Transitionally reserved code */
    {"NU", "Niue"},
    {"NZ", "New Zealand"},
/*  {"OA", "African Intellectual Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"OO", "user-assigned escape code"},	/* User-assigned code */
    {"OM", "Oman"},
    {"PA", "Panama"},
    {"PC", "Pacific Islands Trust Territory"},	/* Formerly assigned code */
    {"PE", "Peru"},
    {"PF", "French Polynesia"},
    {"PG", "Papua New Guinea"},
    {"PH", "Philippines"},
    {"PI", "Philippines"},	/* Indeterminately reserved code */
    {"PK", "Pakistan"},
    {"PL", "Poland"},
    {"PM", "Saint Pierre and Miquelon"},
    {"PN", "Pitcairn"},
    {"PR", "Puerto Rico"},
    {"PS", "Palestine, State of"},
    {"PT", "Portugal"},
    {"PU", "United States Miscellaneous Pacific Islands"},	/* Formerly assigned code */
    {"PW", "Palau"},
    {"PY", "Paraguay"},
    {"PZ", "Panama Canal Zone"},	/* Formerly assigned code */
    {"QA", "Qatar"},
    {"QM", "User-assigned code QM"},	/* User-assigned code */
    {"QN", "User-assigned code QN"},	/* User-assigned code */
    {"QO", "User-assigned code QO"},	/* User-assigned code */
    {"QP", "User-assigned code QP"},	/* User-assigned code */
    {"QQ", "User-assigned code QQ"},	/* User-assigned code */
    {"QR", "User-assigned code QR"},	/* User-assigned code */
    {"QS", "User-assigned code QS"},	/* User-assigned code */
    {"QT", "User-assigned code QT"},	/* User-assigned code */
    {"QU", "User-assigned code QU"},	/* User-assigned code */
    {"QV", "User-assigned code QV"},	/* User-assigned code */
    {"QW", "User-assigned code QW"},	/* User-assigned code */
    {"QX", "User-assigned code QX"},	/* User-assigned code */
    {"QY", "User-assigned code QY"},	/* User-assigned code */
    {"QZ", "User-assigned code QZ"},	/* User-assigned code */
    {"RA", "Argentina"},	/* Indeterminately reserved code */
    {"RB", "Bolivia or Botswana"},	/* Indeterminately reserved code */
    {"RC", "China"},		/* Indeterminately reserved code */
    {"RE", "R\xc3\xa9union"},
    {"RH", "Haiti"},		/* Indeterminately reserved code */
    {"RI", "Indonesia"},	/* Indeterminately reserved code */
    {"RL", "Lebanon"},		/* Indeterminately reserved code */
    {"RM", "Madagascar"},	/* Indeterminately reserved code */
    {"RN", "Niger"},		/* Indeterminately reserved code */
    {"RO", "Romania"},
    {"RP", "Philippines"},	/* Indeterminately reserved code */
    {"RS", "Serbia"},
    {"RU", "Russian Federation"},
    {"RW", "Rwanda"},
    {"SA", "Saudi Arabia"},
    {"SB", "Solomon Islands"},
    {"SC", "Seychelles"},
    {"SD", "Sudan"},
    {"SE", "Sweden"},
    {"SF", "Finland"},		/* Indeterminately reserved code */
    {"SG", "Singapore"},
    {"SH", "Saint Helena, Ascension and Tristan da Cunha"},
    {"SI", "Slovenia"},
    {"SJ", "Svalbard and Jan Mayen"},
    {"SK", "Slovakia"},
    {"SL", "Sierra Leone"},
    {"SM", "San Marino"},
    {"SN", "Senegal"},
    {"SO", "Somalia"},
    {"SR", "Suriname"},
    {"SS", "South Sudan"},
    {"ST", "Sao Tome and Principe"},
    {"SU", "USSR"},		/* Exceptionally reserved code */
    {"SV", "El Salvador"},
    {"SX", "Sint Maarten (Dutch part)"},
    {"SY", "Syrian Arab Republic"},
    {"SZ", "Eswatini"},
    {"TA", "Tristan da Cunha"},	/* Exceptionally reserved code */
    {"TC", "Turks and Caicos Islands"},
    {"TD", "Chad"},
    {"TF", "French Southern Territories"},
    {"TG", "Togo"},
    {"TH", "Thailand"},
    {"TJ", "Tajikistan"},
    {"TK", "Tokelau"},
    {"TL", "Timor-Leste"},
    {"TM", "Turkmenistan"},
    {"TN", "Tunisia"},
    {"TO", "Tonga"},
    {"TP", "Turkey"},		/* Transitionally reserved code */
    {"TR", "Turkey"},
    {"TT", "Trinidad and Tobago"},
    {"TV", "Tuvalu"},
    {"TW", "Taiwan"},
    {"TZ", "Tanzania, United Republic of"},
    {"UA", "Ukraine"},
    {"UG", "Uganda"},
    {"UK", "United Kingdom"},	/* Exceptionally reserved code */
    {"UM", "United States Minor Outlying Islands"},
    {"UN", "United Nations"},	/* Exceptionally reserved code */
    {"US", "United States of America"},
    {"UY", "Uruguay"},
    {"UZ", "Uzbekistan"},
    {"VA", "Holy See"},
    {"VC", "Saint Vincent and the Grenadines"},
    {"VD", "Democratic Republic of Viet-Nam"},	/* Formerly assigned code */
    {"VE", "Venezuela (Bolivarian Republic of)"},
    {"VG", "Virgin Islands (British)"},
    {"VI", "Virgin Islands (U.S.)"},
    {"VN", "Viet Nam"},
    {"VU", "Vanuatu"},
    {"WF", "Wallis and Futuna"},
    {"WG", "Grenada"},		/* Indeterminately reserved code */
    {"WK", "Wake Island"},	/* Formerly assigned code */
    {"WL", "Saint Lucia"},	/* Indeterminately reserved code */
/*  {"WO", "World Intellectual Property Organization"}, */ /* WIPO Indeterminately reserved code */
    {"WS", "Samoa"},
    {"WV", "Saint Vincent"},	/* Indeterminately reserved code */
    {"XA", "User-assigned code XA"},	/* User-assigned code */
    {"XB", "User-assigned code XB"},	/* User-assigned code */
    {"XC", "User-assigned code XC"},	/* User-assigned code */
    {"XD", "User-assigned code XD"},	/* User-assigned code */
    {"XE", "User-assigned code XE"},	/* User-assigned code */
    {"XF", "User-assigned code XF"},	/* User-assigned code */
    {"XG", "User-assigned code XG"},	/* User-assigned code */
    {"XH", "User-assigned code XH"},	/* User-assigned code */
    {"XI", "User-assigned code XI"},	/* User-assigned code */
    {"XJ", "User-assigned code XJ"},	/* User-assigned code */
    {"XK", "User-assigned code XK"},	/* User-assigned code */
    {"XL", "User-assigned code XL"},	/* User-assigned code */
    {"XM", "User-assigned code XM"},	/* User-assigned code */
    {"XN", "User-assigned code XN"},	/* User-assigned code */
    {"XO", "User-assigned code XO"},	/* User-assigned code */
    {"XP", "User-assigned code XP"},	/* User-assigned code */
    {"XR", "User-assigned code XR"},	/* User-assigned code */
    {"XS", "User-assigned code XS"},	/* User-assigned code */
    {"XT", "User-assigned code XT"},	/* User-assigned code */
    {"XU", "User-assigned code XU"},	/* User-assigned code */
    {"XV", "User-assigned code XV"},	/* User-assigned code */
    {"XW", "User-assigned code XW"},	/* User-assigned code */
    {"XX", "User-assigned code XX for anonymous locations"},	/* User-assigned code */
    {"XY", "User-assigned code XY"},	/* User-assigned code */
    {"XZ", "User-assigned code XZ"},	/* User-assigned code */
    {"YD", "Democratic Yemen"},	/* Formerly assigned code */
    {"YE", "Yemen"},
    {"YT", "Mayotte"},
    {"YU", "Yugoslavia"},	/* Transitionally reserved code */
    {"YV", "Venezuela"},	/* Indeterminately reserved code */
    {"ZA", "South Africa"},
    {"ZM", "Zambia"},
    {"ZR", "Zaire"},		/* Transitionally reserved code */
    {"ZW", "Zimbabwe"},
    {"ZZ", "User-assigned code ZZ"},	/* User-assigned code */
    {NULL, NULL}		/* MUST BE LAST */
};

size_t SIZEOF_LOCATION_TABLE = TBLLEN(loc);

/* check_location_table	    - make sure that there are no embedded NULL elements
 *			      in the location table and that the last element is
 *			      NULL
 *
 * This function verifies that the only NULL element in the table is the very
 * last element: if it's not there or there's another NULL element it's a
 * problem that has to be fixed.
 *
 * This function does not return on error.
 */
void
check_location_table(void)
{
    size_t max = SIZEOF_LOCATION_TABLE;

    size_t i;

    for (i = 0; i < max - 1 && loc[i].code != NULL; )
	++i;

    if (max - 1 != i) {
	err(333, __func__, "found embedded NULL element in location table; fix table in location.c and recompile");
	not_reached();
    }
    if (loc[i].code != NULL || loc[i].name != NULL) {
	err(10, __func__, "no final NULL element found in location table.c; fix table in location.c and recompile");
	not_reached();
    }
}


