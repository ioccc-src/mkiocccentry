/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * foo - bar
 *
 * "Because sometimes code is foo and sometimes foo is fubar." :-)
 *
 * The JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes we need more O in C." :-)
 *
 * This tool is being co-developed by Cody Boone Ferguson and Landon Curt Noll.
 *
 * The concept of this file was developed by:
 *
 *	Cody Boone Ferguson
 *
 * after some discussion with Landon Curt Noll about the need (yes: need) for
 * it. Cody is quite okay if after looking at this file you think he's a bit
 * crazy and in fact he quite welcomes it and even relishes the idea! :-)
 *
 * "Because sometimes we all need a little craziness in our lives." :-)
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

/*
 * foo - bar
 */
#include "foo.h"

/*
 * static values
 */

/* Special stuff you're not supposed to not know about :-( .. and more stuff :-) */
static time_t t = 314159223+42;
static char const *oebxergfB[] =
{
    "'yrff' vf yrkvpbtencuvpnyyl terngre guna 'terngre'. '<', ubjrire,\n"
    "vf yrkvpbtencuvpnyyl yrff guna '>'. Fbzrbar fperjrq hc.\n"
    "-- Krkly",

    "V qba'g xabj unys bs lbh unys nf jryy nf V fubhyq yvxr;\n"
    "naq V yvxr yrff guna"
    " unys bs lbh unys nf jryy nf lbh qrfreir.\n"
    "-- Ovyob Onttvaf",

    "P frpgvba: '{}'\n"
    "-- Krkly\n",

    "'Frpbaqyl, gb pryroengr BHE oveguqnlf: zvar naq zl ubabhenoyr naq tnyynag\n"
    "sngure'f.' Hapbzsbegnoyr naq nccerurafvir fvyrapr. 'V nz bayl unys gur zna\n"
    "gung ur vf: V nz 72, ur vf 144. Lbhe ahzoref ner pubfra gb qb ubabhe gb\n"
    "rnpu bs uvf ubabhenoyr lrnef.' Guvf jnf ernyyl qernqshy -- n erthyne\n"
    "oenvagjvfgre, naq fbzr bs gurz sryg vafhygrq, yvxr yrnc-qnlf fubirq\n"
    "va gb svyy hc n pnyraqne.\n"
    "-- Ovatb Onttvaf",

    "P havbaf: ybivat pbhcyrf jub rawbl cebtenzzvat va P gbtrgure.\n"
    "-- Krkly\n",

    "'Ybbx,' fnvq Neguhe, 'jbhyq vg fnir lbh n ybg bs gvzr vs V whfg tnir\n"
    "hc naq jrag znq abj?'\n",



    "Orarsvg bs Pyvzngr Punatr?\n"
    "\n"
    "Nf gur jbeyq trgf qevre naq ubggre znal crbcyr ner haqrefgnaqnoyl nsenvq bs gur\n"
    "pbafrdhraprf sbe gurve yviryvubbq naq gur yviryvubbq bs gurve puvyqera naq\n"
    "tenaqpuvyqera.\n"
    "\n"
    "Vg'f orra xabja sbe nrbaf gung jr nyy arrq jngre gb fheivir ohg jvgu pyvzngr\n"
    "punatr gurer vf yrff jngre qhr gb ybatre, zber serdhrag qebhtugf. Pebcf ner ng\n"
    "evfx yrnqvat gb shegure snzvar naq qvfrnfr.\n"
    "\n"
    "Naq nf gurer ner fgvyy fbzr nyvir jub ner byq rabhtu gb erzrzore gur Terng\n"
    "Snzvar (zber pbzzbayl xabja bhgfvqr bs Verynaq nf gur Vevfu Cbgngb Snzvar)\n"
    "juvpu yrq gb nccebkvzngryl bar zvyyvba qrnguf naq ng yrnfg n zvyyvba riraghnyyl\n"
    "yrnivat Verynaq gurer ner fbzr gung ner gnxvat guvf rfcrpvnyyl frevbhf (gur\n"
    "nhgube'f zngr Qnir Dhvaa vf cnegvphyneyl fpnerq, fnlvat 'V erzrzore jura zl\n"
    "tenaqqnq gbyq zr jnl onpx va 1847 gung vs qvfrnfr qvq abg xvyy uvz svefg ur\n"
    "jbhyq tb onezl vs ur pbhyq abg fbba znxr fbzr tbbq byq snfuvbarq pbqqyr'). Znal\n"
    "cnegf bs gur jbeyq nyernql nccerpvngr guvf gubhtu gurl ab qbhog jvyy or\n"
    "pbashfrq ol gur jbeq 'pbqqyr'.\n"
    "\n"
    "Jbeyqjvqr gur evfvat grzcrengherf ner pnhfvat qnzntr gb vasenfgehpgherf, znxvat\n"
    "jvyqsverf zber serdhrag naq zber qnatrebhf naq vg'f zrygvat vpr ng\n"
    "erpbeq-oernxvat engrf. Guvf zrygvat vpr va ghea vf vapernfvat gur frnjngre\n"
    "yriry juvpu frevbhfyl raqnatref pbnfgny gbjaf naq znxrf vg uneqre sbe fbzr\n"
    "navznyf yvxr cbyne ornef gb rira fheivir.\n"
    "\n"
    "Ohg abj fbzr fpvragvfgf naq npgvivfgf ner uvtuyvtugvat n cbfvgvir tnva nobhg\n"
    "pyvzngr punatr. Nf gur cynarg trgf ubggre naq qevre, gur gbkvp tnf qvulqebtra\n"
    "zbabkvqr vf yrff cerinyrag.\n"
    "\n"
    "Sbe gur havasbezrq: rirel fvatyr sybbq unf n uvtu nzbhag bs QUZB; jvgubhg QUZB\n"
    "pyvzngr nssrpgvat npvq enva jbhyq abg or n ceboyrz; vg'f orra hfrq ol\n"
    "tbireazragf va gbegher naq zheqre; vg'f hfrq va ahpyrne jrncbaf; naq rira whfg\n"
    "oernguvat vg va pna rnfvyl xvyy lbh. Va snpg rirel crefba rkcbfrq gb vg\n"
    "riraghnyyl qvrf!\n"
    "\n"
    "Ohg qrfcvgr gurfr snpgf ab tbireazrag unf fhpprrqrq va onaavat guvf uvtuyl\n"
    "yrguny tnf. Irel srj unir rira gnyxrq nobhg vg. Pbhyq vg or gung gur varcgvghqr\n"
    "bs znaxvaq jvgu pyvzngr punatr zvtug or tbbq? Guvf vf lrg gb or nafjrerq ohg vg\n"
    "unf yrsg gur pyvzngr fprcgvpf funxvat gurve urnq nfxvat 'Uhu?'\n",

    "Gurer vf na neg, be, engure, n xanpx gb sylvat. Gur xanpx yvrf va\n"
    "yrneavat ubj gb guebj lbhefrys ng gur tebhaq naq zvff.\n",

    "SHA SNPG: Va O naq rneyl P, gur += bcrengbe jnf fcryg =+ vafgrnq bs += ;\n"
    "guvf zvfgnxr, ercnverq va 1976, jnf vaqhprq ol n frqhpgviryl rnfl jnl bs\n"
    "unaqyvat gur svefg sbez va O'f yrkvpny nanylfre.\n"
    "\n"
    "Frr zber sha snpgf naq"
    " uvfgbel ol Qraavf Evgpuvr uvzfrys ng\n"
    "uggcf://jjj.oryy-ynof.pbz/hfe/qze/jjj/puvfg.ugzy.\n",

    "'Jung vf vg?' tebjyrq Fnz, zvfvagrecergvat gur fvtaf. 'Jung'f gur arrq gb\n"
    "favss? Gur fgvax arneyl xabpxf zr qbja jvgu zl abfr uryq. Lbh fgvax, naq znfgre\n"
    "fgvaxf; gur jubyr cynpr fgvaxf.'\n"
    "\n"
    "'Lrf, lrf, naq Fnz fgvaxf!' nafjrerq Tbyyhz. "
    "'Cbbe Fzéntby fzryyf vg,\n"
    "ohg tbbq Fzéntby ornef vg. Urycf avpr znfgre. Ohg gung'f"
    " ab znggre. Gur nve'f\n"
    "zbivat, punatr vf pbzvat. Fzéntby jbaqref; ur'f abg unccl.'\n",

    "'Anl! Abg Ryirf,' fnvq gur sbhegu, gur gnyyrfg, naq nf vg nccrnerq gur\n"
    "puvrs nzbat"
    " gurz. 'Ryirf qb abg jnyx va Vguvyvra va gurfr qnlf. Naq\n"
    "Ryirf ner jbaqebhf snve gb"
    " ybbx hcba, be fb 'gvf fnvq.'\n"
    "\n"
    "'Zrnavat jr'er abg, V gnxr lbh,' fnvq Fnz. 'Gunax"
    " lbh xvaqyl. Naq jura\n"
    "lbh'ir svavfurq qvfphffvat hf, creuncf lbh'yy fnl jub lbh ner,"
    " naq\n"
    "jul lbh pna'g yrg gjb gverq geniryyref erfg.'\n",

    "Jul fubhyq lbh arire jrvtu lbhefrys jrnevat fvyire, tbyq be cyngvahz? Orpnhfr\n"
    "gurl'er urnil zrgnyf.\n\n -- Krkly",

    "Tvyqbe jnf fvyrag sbe n zbzrag. 'V qb abg yvxr guvf arjf,' ur fnvq\n"
    "ng ynfg. 'Gung Tnaqnys fubhyq or yngr, qbrf abg obqr jryy. Ohg vg\n"
    "vf fnvq:"
    " Qb abg zrqqyr va gur nssnvef bs Jvmneqf, sbe gurl ner\n"
    "fhogyr naq dhvpx gb "
    "natre. Gur pubvpr vf lbhef: gb tb be jnvg.'\n"
    "\n"
    "'Naq vg vf nyfb fnvq,' nafjrerq"
    " Sebqb: 'Tb abg gb gur Ryirf sbe\n"
    "pbhafry, sbe gurl jvyy fnl obgu ab naq lrf.'",

    "Bar bs gur guvatf Sbeq Cersrpg unq nyjnlf sbhaq uneqrfg gb haqrefgnaq nobhg\n"
    "uhznaf jnf gurve unovg bs pbagvahnyyl fgngvat naq ercrngvat gur irel irel\n"
    "boivbhf.\n"
    "\n"
    "Bar bs gur guvatf Sbeq Cersrpg unq nyjnlf sbhaq uneqrfg gb haqrefgnaq nobhg\n"
    "uhznaf jnf gurve unovg bs pbagvahnyyl fgngvat naq ercrngvat gur irel irel\n"
    "boivbhf.",

    "Sbe n zbzrag, abguvat unccrarq. Gura, nsgre n frpbaq be fb, abguvat\n"
    "pbagvahrq gb unccra.",

    "'Vs V rire zrrg zlfrys,' fnvq Mncubq, 'V'yy uvg zlfrys fb uneq V jba'g xabj\n"
    "jung'f uvg zr.'",

    "'V frr abobql ba gur ebnq,' fnvq Nyvpr.\n"
    "\n"
    "'V bayl jvfu gung V unq fhpu rlrf,' gur Xvat erznexrq"
    " va n sergshy\n"
    "gbar. 'Gb or noyr gb frr Abobql! Naq ng gung qvfgnapr gbb! Jul, vg'f\n"
    "nf zhpu nf V"
    " pna qb gb frr erny crbcyr, ol guvf yvtug!'\n"
    "\n"
    "[...]\n"
    "\n"
    "'Jub qvq lbh cnff ba gur ebnq?' gur Xvat jrag"
    " ba, ubyqvat bhg uvf\n"
    "unaq gb gur Zrffratre sbe fbzr zber unl.\n"
    "\n"
    "'Abobql,' fnvq gur Zrffratre.\n"
    "\n"
    "'"
    "Dhvgr evtug,' fnvq gur Xvat: 'guvf lbhat ynql fnj uvz gbb. Fb bs pbhefr Abobql\n"
    "jnyxf fybjre guna lbh.'\n"
    "\n"
    ""
    "'V qb zl orfg,' gur Zrffratre fnvq va n fhyxl gbar. 'V'z fher abobql jnyxf zhpu\n"
    "snfgre guna V qb!'\n"
    "\n"
    ""
    "'Ur pna'g qb gung,' fnvq gur Xvat, 'be ryfr ur'q unir orra urer svefg.'",

    "'Guvf vf n puvyq!' Unvtun ercyvrq rntreyl, pbzvat va sebag bs Nyvpr gb\n"
    "vagebqhpr ure, naq fcernqvat bhg obgu uvf unaqf gbjneqf ure va na Natyb-Fnkba\n"
    "nggvghqr. 'Jr bayl sbhaq vg gb-qnl. Vg'f nf ynetr nf yvsr, naq gjvpr nf\n"
    "angheny!'\n"
    "\n"
    ""
    "'V nyjnlf gubhtug gurl jrer snohybhf zbafgref!' fnvq gur Havpbea. 'Vf vg\n"
    "nyvir?'\n"
    "\n"
    "'"
    "Vg pna gnyx,' fnvq Unvtun, fbyrzayl.\n"
    "Gur Havpbea ybbxrq qernzvyl ng Nyvpr, naq fnvq 'Gnyx, puvyq.'\n"
    "Nyvpr pbhyq abg uryc ure yvcf pheyvat hc vagb n fzvyr nf fur ortna: 'Qb lbh\n"
    "xabj, V nyjnlf gubhtug"
    " Havpbeaf jrer snohybhf zbafgref, gbb! V arire fnj bar\n"
    "nyvir orsber!'\n"
    "\n"
    "[...]\n"
    "\n"
    "'Nu, jung vf vg, abj?'"
    " gur Havpbea pevrq rntreyl. 'Lbh'yy arire thrff! V\n"
    "pbhyqa'g.' Gur Yvba ybbxrq ng Nyvpr jrnevyl. 'Ner lbh"
    " na navzny -- irtrgnoyr --\n"
    "be zvareny?' ur fnvq, lnjavat ng rirel bgure jbeq.\n"
    "\n"
    "'Vg'f n snohybhf zbafgre!'"
    " gur Havpbea pevrq bhg, orsber Nyvpr pbhyq ercyl.\n"
    "\n"
    "'Gura unaq ebhaq gur cyhz-pnxr, Zbafgre,' gur Yvba fnvq.",

    "V ortna gb unir qbhogf nobhg ubj zngu vf gnhtug va guvf pbhagel jura n lbhat\n"
    "pbbx ng zl cvmmrevn fnvq ur unq n ceboyrz. 'N phfgbzre pnyyrq, nfxvat sbe n\n"
    "cvr gung'f n"
    " guveq purrfr, n guveq crccrebav, naq n guveq pbzovangvba,' ur\n"
    "fnvq. 'Fb, jung'f gur ceboyrz?' "
    "V nfxrq uvz. 'V qba'g xabj jung gb chg ba\n"
    "gur sbhegu guveq'.\n"
    "\n"
    "-EBOREG XRNEARL\n"
    "Enapub Pbeqbin,\n"
    "Pnyvsbeavn",

    "Jul jnf gur cnegvnyyl qrns qlfyrkvp zna hcfrg ng uvf jvsr? Fur nfxrq uvz vs ur\n"
    "tbg gur ebyyrq bngf naq ur gubhtug fur jnf pnyyvat uvz na byq bns.\n"
    "\n-- Krkly",

    "'Naq lbh qb Nqqvgvba?' gur Juvgr Dhrra nfxrq. 'Jung'f bar naq bar naq bar naq\n"
    "bar naq bar naq bar"
    " naq bar naq bar naq bar naq bar?' 'V qba'g xabj,' fnvq\n"
    "Nyvpr. 'V ybfg pbhag.'\n"
    "\n"
    "'Fur pna'g qb Nqqvgvba,'"
    " gur Erq Dhrra vagreehcgrq. 'Pna lbh qb Fhogenpgvba?'\n"
    "Gnxr avar sebz rvtug.' 'Avar sebz rvtug V pna'g, lbh xabj,'"
    " Nyvpr ercyvrq irel\n"
    "ernqvyl: 'ohg--'\n"
    "\n"
    "'Fur pna'g qb Fhogenpgvba,' fnvq gur Juvgr Dhrra. 'Pna lbh qb Qvivfvba? "
    "Qvivqr\n"
    "n ybns ol n xavsr -- jung'f gur nafjre gb gung?' 'V fhccbfr--' Nyvpr jnf\n"
    "ortvaavat, ohg gur Erq Dhrra "
    "nafjrerq sbe ure.  'Oernq-naq-ohggre, bs pbhefr.\n"
    "Gel nabgure Fhogenpgvba fhz. Gnxr n obar sebz n qbt: jung erznvaf?'\n"
    "\n"
    ""
    "Nyvpr pbafvqrerq. 'Gur obar jbhyqa'g erznva, bs pbhefr, vs V gbbx vg--naq gur\n"
    "qbt jbhyqa'g erznva; vg jbhyq pbzr gb ovgr "
    "zr--naq V'z fher V fubhyqa'g\n"
    "erznva!' 'Gura lbh guvax abguvat jbhyq erznva?' fnvq gur Erq Dhrra.\n"
    "\n"
    "'V guvax gung'f gur "
    "nafjre.' 'Jebat, nf hfhny,' fnvq gur Erq Dhrra: 'gur qbt'f\n"
    "grzcre jbhyq erznva.' 'Ohg V qba'g frr ubj --'\n"
    "\n"
    ""
    "'Jul, ybbx urer!' gur Erq Dhrra pevrq. 'Gur qbt jbhyq ybfr vgf grzcre, jbhyqa'g\n"
    "vg?'\n"
    "\n"
    ""
    "'Creuncf vg jbhyq,' Nyvpr ercyvrq pnhgvbhfyl. 'Gura vs gur qbt jrag njnl, vgf\n"
    "grzcre jbhyq erznva!' "
    "gur Dhrra rkpynvzrq gevhzcunagyl.",

    "Urer gur Erq Dhrra ortna ntnva. 'Pna lbh nafjre hfrshy dhrfgvbaf?' fur fnvq.\n"
    "'Ubj vf oernq znqr?'\n"
    "\n"
    "'V xabj gung!' Nyvpr pevrq rntreyl.' Lbh gnxr fbzr sybhe--'\n"
    "'Jurer qb lbh cvpx gur sybjre?' gur Juvgr Dhrra nfxrq. 'Va n tneqra, be va gur\n"
    "urqtrf?'\n"
    "\n"
    ""
    "'Jryy vg vfa'g cvpxrq ng nyy,' Nyvpr rkcynvarq: 'vg'f tebhaq--'\n"
    "'Ubj znal nperf bs tebhaq?' "
    "fnvq gur Juvgr Dhrra. 'Lbh zhfga'g yrnir bhg fb\n"
    "znal guvatf.'\n"
    "\n"
    "'Sna ure urnq!' gur Erq Dhrra "
    "nakvbhfyl vagreehcgrq. 'Fur'yy or srirevfu nsgre\n"
    "fb zhpu guvaxvat.' Fb gurl frg gb jbex naq snaarq "
    "ure jvgu ohapurf bs yrnirf,\n"
    "gvyy fur unq gb ort gurz gb yrnir bss, vg oyrj ure unve nobhg fb.",

    "SHA SNPG:\n"
    "\n"
    "Va Yvsr, gur Havirefr naq Rirelguvat, gur guveq vafgnyzrag bs Gur Uvgpuuvxre'f\n"
    "Thvqr gb gur Tnynkl ol Qbhtynf Nqnzf, vg'f fnvq gur nafjre gb Yvsr, gur\n"
    "Havirefr naq Rirelguvat vf 42. "
    "Qbhtynf Nqnzf vafvfgrq guvf jnf n wbxr, jevgvat\n"
    "ba HFRARG:\n"
    "\n"
    "    Gur nafjre gb guvf vf irel fvzcyr. Vg "
    "jnf n wbxr. Vg unq gb or n ahzore, na\n"
    "    beqvanel, fznyyvfu ahzore, naq V pubfr gung bar. Ovanel ercerfragngvbaf,"
    "\n"
    ""
    "    onfr guvegrra, Gvorgna zbaxf ner nyy pbzcyrgr abafrafr. V fng ng zl qrfx,\n"
    "    fgnerq vagb gur tneqra naq gubhtug"
    " '42 jvyy qb' V glcrq vg bhg. Raq bs\n"
    "    fgbel.\n"
    "\n"
    "Ohg qvq lbh xabj gung fbzr nfgebabzref FCRAG GJB LRNEF fghqlvat guvf "
    "naq VAFVFG\n"
    "gung 42 vf npghnyyl pbeerpg? Sbe guvf fgbel frr:\n"
    "\n"
    ""
    "uggcf://jjj.vaqrcraqrag.pb.hx/arjf/lrf-gur-nafjre-gb-gur-havirefr-ernyyl-vf-42-1351201.ugzy",

    "Vg'f pbzzbayl fhttrfgrq gung P++ vf na vzcebirzrag bire P. Guvf vf n zlgu\n"
    "qrevivat sebz gur snpg gung gur bcrengbe ++ va P vapernfrf vgf bcrenaq nf n fvqr\n"
    "rssrpg: gur inyhr bs gur bcrengvba qrcraqvat ba vs vg'f cbfgsvk be cersvk ++. Nf\n"
    "vg unccraf P++ vf cbfgsvk ++. Guvf zrnaf gur inyhr bs gur rkcerffvba vf P naq nf\n"
    "n fvqr rssrpg P vf vaperzragrq ol 1. Guvf cebirf gung P vf fhcrevbe - jung'f\n"
    "yrsg, P + 1, vf oybng naq vg'f htyl. Vg'f uhtr naq vaqrprag. Sbe P++'f dhnyvgl\n"
    "gel P = 0;.\n"
    "    -- Krkly",

    "Unir lbh gevrq sbepvat na harkcrpgrq erobbg?\n"
    "\n"
    ""
    "Frr, gur qevire ubbxf gur shapgvba ol cngpuvat gur flfgrz pnyy gnoyr, fb vg'f\n"
    "abg fnsr gb haybnq vg hayrff nabgure guernq'f nobhg gb whzc va naq qb vgf\n"
    "fghss, naq lbh qba'g jnag gb raq hc Va gur zvqqyr bs vainyvq zrzbel.\n"
    "\n"
    ""
    "    chfu BkS9QNSSOB\n"
    "    pnyy nevrf+BkR90(S9Q8SR90)\n"
    "    zbi rqk, [rpk+1]\n"
    "    zbi nk, qf:XrFreivprQrfpevcgbeGnoyr\n"
    "    zbi qjbeq_110PB, rqk\n"
    "    zbi rnk, qf:MjDhrelQverpgbelSvyr\n"
    "\n"
    "Uryyb?\n"
    "\n"
    "-- Znhevpr Zbff, Gur VG "
    "Pebjq Frevrf 1 Rcvfbqr 1, 'Lrfgreqnl'f Wnz'.",

    "QVQ LBH XABJ gung va Jnfuvatgba jura gjb genvaf pbzr gb n pebffvat, arvgure pna\n"
    "tb hagvy gur bgure unf cnffrq?\n"
    "\n"
    "Va juvpu pnfr, obgu yvgrenyyl naq ybtvpnyyl fcrnxvat, arvgure genva pna rire\n"
    "tb. Guvf va ghea zrnaf gung gjb zber genvaf jbhyq pbzr, hanoyr gb cebprrq naq\n"
    "gura gjb zber naq riraghnyyl gur envyjnl jbhyq or oybpxrq, ceriragvat pnef sebz\n"
    "pebffvat nf jryy, znxvat n uhtr genssvp wnz!",

    "QVQ LBH XABJ gung va Bertba n qbbe ba n pne znl abg or yrsg bcra ybatre guna\n"
    "vf arprffnel?\n"
    "\n"
    "Svanyyl jr unir na rkcynangvba bs jul gurer ner fb znal pne qbbef ylvat ba gur\n"
    "fgerrgf va Bertba!",

    "QVQ LBH XABJ gung va Pnyvsbeavn navznyf ner onaarq sebz zngvat choyvpyl jvguva\n"
    "1500 srrg bs n gnirea, fpubby, be cynpr bs jbefuvc?\n"
    "\n"
    "Ohg ng yrnfg bapr gurl'er wnvyrq, boivbhfyl va n mbb, gurl pna qb vg nf\n"
    "bsgra nf gurl yvxr!",

    "QVQ LBH XABJ gung va Arinqn vg vf vyyrtny gb qevir n pnzry ba gur uvtujnl?\n"
    "\n"
    "Jurgure guvf zrna tvir n pnzry n yvsg ba gur uvtujnl be npghnyyl hfvat gur\n"
    "pnzry nf n iruvpyr vf hapyrne, whfg yvxr gur checbfr bs guvf ynj vgfrys\n"
    "rfcrpvnyyl fvapr gurer zvtug abg or znal pnzryf yrsg va Arinqn naljnl.",

    "QVQ LBH XABJ gung va Vqnub vg vf vyyrtny sbe n zna gb tvir uvf fjrrgurneg n obk\n"
    "bs pnaql jrvtuvat yrff guna 50 cbhaqf (22.6796xt)?\n"
    "\n"
    "V org pnaql fubcf ybir guvf ynj!",

    "QVQ LBH XABJ gung va Zbagnan vg vf vyyrtny gb unir n furrc va gur pno bs lbhe\n"
    "gehpx jvgubhg n puncrebar?\n"
    "\n"
    "Nccneragyl va Zbagnan furrc ner uhznaf! Be ryfr ... ynjlref qba'g xabj jung\n"
    "puncrebar zrnaf.",

    "Vs na rnegudhnxr oernxf n zveebe qbrf gur ragver rnegu unir frira lrnef bs onq\n"
    "yhpx ?\n -- Krkly",

    "QVQ LBH XABJ gung va Jlbzvat vg vf vyyrtny gb jrne n ung gung bofgehpgf\n"
    "crbcyr'f ivrj va n choyvp gurnger be cynpr bs nzhfrzrag?\n"
    "\n"
    "...V guvax guvf npghnyyl fbhaqf ernfbanoyr ohg V'z nyfb n qjnes. Gura ntnva,\n"
    "jura ylvat qbja jr'er nyy qjneirf!",

    "QVQ LBH XABJ gung gur Xnafnf fgngr tnzvat obneq cebuvovgf gur hfr bs zhyrf gb\n"
    "uhag qhpxf?\n"
    "\n"
    "V'z abg fher znal crbcyr guvax zhyrf pna cbffvoyl uryc uhagvat qhpxf ohg\n"
    "pyrneyl fbzr qb.",

    "QVQ LBH XABJ gung va Nynonzn vg vf vyyrtny gb jrne n snxr zbhfgnpur gung pnhfrf\n"
    "ynhtugre va puhepu?\n"
    "\n"
    "Ohg jul vf vg gung guvf ynj sbeovqqvat n zbhfgnpur pnhfvat ynhtugre va n\n"
    "puhepu npghnyyl znxrf zr ynhtu?",

    "QVQ LBH XABJ gung va Jrfg Ivetvavn juvfgyvat haqrejngre vf cebuvovgrq?\n"
    "\n"
    "Sbeghangryl gur jbhyq or pevzvany jbhyq cebonoyl gnxr va nyy gung jngre naq\n"
    "qebja, ceriragvat gur nhgubevgvrf sebz punetvat gurz jvgu n pevzr!",

    "QVQ LBH XABJ gung va Znvar nsgre Wnahnel 14gu lbh jvyy or punetrq n svar sbe\n"
    "univat Puevfgznf qrpbengvbaf fgvyy hc?\n"
    "\n"
    "Ohg urer'f n shaal guvat nobhg ybpngvba naq qngrf: ybtvpnyyl fcrnxvat, rira\n"
    "vs fbzrguvat vf rnfg vg vf nyfb jrfg naq Wnahnel vf obgu orsber naq nsgre\n"
    "Qrprzore!",

    "QVQ LBH XABJ gung va Fbhgu Pnebyvan n crefba zhfg or 18 lrnef byq gb cynl\n"
    "cvaonyy?\n"
    "\n"
    "Guvf vf cebonoyl orpnhfr vg hfrq gb or pbafvqrerq tnzoyvat, qrfcvgr gur snpg\n"
    "gung nalbar jub'f npghnyyl gevrq vg sbe svir frpbaqf jvyy xabj vg'f n znggre bs\n"
    "fxvyy, abg yhpx.",

    "QVQ LBH XABJ gung va Arj Zrkvpb vqvbgf znl abg ibgr?\n"
    "\n"
    "Vqvbg hfrq gb or n yrtny grez ohg gura jub rknpgyl znqr guvf ynj V jbaqre?",

    "QVQ LBH XABJ gung va Zvaarfbgn vg vf vyyrtny gb fyrrc anxrq?\n"
    "\n"
    "Nccneragyl, Zvaarfbgn ynjznxref ner nyfb crrcvat Gbzf!",

    "QVQ LBH XABJ gung va Zvpuvtna hagvy 2002 vg jnf vyyrtny gb fjrne va sebag bs n\n"
    "jbzna be puvyq?\n"
    "\n"
    "Jung vs n puvyq fjrnef ng n jbzna be n jbzna fjrnef ng n puvyq? Puvyq ng puvyq?\n"
    "Jbzna ng jbzna?",

    "Jung qb lbh pnyy n svfurezna fryyvat uvf svfu? Frysvfu.\n"
    "-- Krkly",

    "QVQ LBH XABJ gung va Irezbag jbzra zhfg bognva jevggra crezvffvba sebz gurve\n"
    "uhfonaqf gb jrne snyfr grrgu?\n"
    "\n"
    "Ohg vs gurl qba'g naq pna'g purj gurve sbbq ubj jvyy gurl yvir? Gung jbhyq\n"
    "zrna gur zna jbhyq or jvsryrff naq gur ynj jbhyq or cbvagyrff. Vfa'g vg\n"
    "nyernql?",

    "QVQ LBH XABJ gung va Znffnpuhfrggf ng n jnxr zbhearef znl rng ab zber guna\n"
    "guerr fnaqjvpurf?\n"
    "\n"
    "Gunax tbbqarff zbfg crbcyr guvax bs rngvat arkg gb na bcra pbssva!",

    "QVQ LBH XABJ gung va Pbaarpgvphg vg vf vyyrtny gb qvfcbfr bs hfrq enmbe oynqrf?\n"
    "\n"
    "Ohg ubcrshyyl hfrq fpnycryf pna or qvfcbfrq?",

    "QVQ LBH XABJ gung va Unjnvv pbvaf ner abg nyybjrq gb or cynprq va bar'f rnef?\n"
    "\n"
    "Ohg jung V jnag gb xabj vf jung nobhg n gjb'f rnef?",

    "QVQ LBH XABJ gung va Nexnafnf gur Nexnafnf evire pna evfr ab uvture guna gb gur\n"
    "Znva Fgerrg oevqtr va Yvggyr Ebpx?\n"
    "\n"
    "Nexnafnf, jurer ynjznxref ner qnsg rabhtu gb gel naq bhgynj sybbqf.",

    "QVQ LBH XABJ gung va Graarffrr vg vf n pevzr gb funer lbhe Argsyvk cnffjbeq?\n"
    "\n"
    "Vafgrnq, vs lbh jnag gb funer lbhe cnffjbeq, punatr vg gb cnffjbeq naq gryy\n"
    "gubfr lbh jvfu gb funer vg jvgu gung vg'f bar bs gur zbfg pbzzba naq jbefg\n"
    "cnffjbeqf va uvfgbel!",

    "QVQ LBH XABJ gung va Hgnu vg vf vyyrtny gb abg qevax zvyx?\n"
    "\n"
    "Hgnu, jurer gurl qba'g gbyrengr ynpgbfr vagbyrenapr!",

    "QVQ LBH XABJ gung va Nynfxn vg vf pbafvqrerq na bssrapr gb chfu n yvir zbbfr\n"
    "bhg bs n zbivat nrebcynar?\n"
    "\n"
    "Ohg pyrneyl vg'f bxnl gb chfu n qrnq zbbfr bhg bs n zbivat nrebcynar naq nal\n"
    "zbbfr bhg bs n fgvyy cynar?",

    "QVQ LBH XABJ gung va Znelynaq guvfgyrf znl abg tebj va bar'f lneq?\n"
    "\n"
    "Jung nobhg n tneqra? Sbe va Nzrevpn n lneq vf n tneqra ohg va Ratynaq n\n"
    "lneq vf n cvrpr bs haphygvingrq tebhaq naq n tneqra vf na Nzrevpna lneq lrg\n"
    "Nzrevpn unf tneqraf gbb. Fb jung vf n lneq naq jung vf n tneqra?",

    "QVQ LBH XABJ gung va Fbhgu Qnxbgn ab ubefrf ner nyybjrq vagb Sbhagnva Vaa\n"
    "hayrff gurl ner jrnevat cnagf?\n"
    "\n"
    "V org gurer ner n ybg bs penml guvatf gung unccra va gurer vs fbzr ubefrf\n"
    "ner nyybjrq ohg abg bguref!",

    "QVQ LBH XABJ gung va Abegu Qnxbgn vg vf vyyrtny gb yvr qbja naq snyy nfyrrc\n"
    "jvgu lbhe fubrf ba?\n"
    "\n"
    "Ohg jung unccraf gb vazngrf jub snyy nfyrrc jvgu gurve fubrf ba? Qb gurl tb\n"
    "gb wnvy va wnvy sbe snyyvat nfyrrc va wnvy jvgu gurve fubrf ba? Naq jung vs\n"
    "gurl jrag gb wnvy vavgvnyyl sbe snyyvat nfyrrc jvgu gurve fubrf ba? Qb gurl\n"
    "freir qbhoyr gvzr?",

    "Jvaqbjf qevivat lbh penml? Ohl n fpnaare naq vafgnyy gur vapyhqrq FNAR\n"
    "yvoenel!\n -- Krkly",

    "QVQ LBH XABJ gung va Ivetvavn puvyqera ner abg gb tb gevpx-be-gerngvat ba\n"
    "Unyybjrra?\n"
    "\n"
    "Jung nobhg nqhygf? V zrna znal fhccbfrq nqhygf va guvf jbeyq pbhyq rnfvyl or\n"
    "zvfgnxra sbe puvyqera.",

    "QVQ LBH XABJ gung va Nevmban uhagvat pnzryf vf cebuvovgrq?\n"
    "\n"
    "Unl, ng yrnfg gur irel srj erznvavat pnzryf ner fnsr!",

    "QVQ LBH XABJ gung va Grknf vg vf vyyrtny gb gnxr zber guna guerr fvcf bs orre\n"
    "ng n gvzr juvyr fgnaqvat?\n"
    "\n"
    "Vg'f nyfb vyyrtny gb or qehax va n cho va Ratynaq ohg V qba'g guvax gung unf\n"
    "fgbccrq znal crbcyr!",

    "QVQ LBH XABJ gung va Pbybenqb vg vf vyyrtny gb evqr n ubefr juvyr haqre gur\n"
    "vasyhrapr?\n"
    "\n"
    "Ohg vf vg n ubefr be n ubefr? N ubefr vf na nqhyg znyr ubefr; n fgnyyvba be\n"
    "tryqvat. Ohg jung nobhg ubefrf?",

    "Frnjrrq: Znevwhnan sbe svfu.\n"
    "-- Krkly",

    "QVQ LBH XABJ gung va Aroenfxn vs n puvyq ohecf qhevat puhepu, uvf be ure\n"
    "cnerag znl or neerfgrq?\n"
    "\n"
    "Sha snpg: gur grpuavpny grez sbe n ehzoyvat be thetyvat abvfr znqr ol gur\n"
    "zbirzrag bs syhvq naq tnf va gur vagrfgvarf vf pnyyrq n obeobeltzhf.",

    "QVQ LBH XABJ gung va Bxynubzn vg vf vyyrtny gb unir gur uvaq yrtf bs snez\n"
    "navznyf va lbhe obbgf?\n"
    "\n"
    "Ohg qba'g jbeel gbb zhpu: vg vf bxnl vs gurve sebag yrtf ner.",

    "QVQ LBH XABJ gung va Jvfpbafva vg vf vyyrtny gb phg n jbzna'f unve?\n"
    "\n"
    "Guvf znxrf Jvfpbafva gur unvevrfg cynpr gb yvir va gur Havgrq Fgngrf bs\n"
    "Nzrevpn.",

    "QVQ LBH XABJ gung va Xraghpxl bar znl abg qlr n qhpxyvat oyhr naq bssre vg sbe\n"
    "fnyr hayrff zber guna fvk ner sbe fnyr ng bapr?\n"
    "\n"
    "Terra, checyr, erq, benatr naq nal bgure pbybhe vf bxnl.",

    "QVQ LBH XABJ gung va Ybhvfvnan vg vf vyyrtny gb ebo n onax naq gura fubbg gur\n"
    "onax gryyre jvgu n jngre cvfgby?\n"
    "\n"
    "Juvpu zrnaf ybtvpnyyl fcrnxvat vg'f yrtny gb fubbg gurz jvgu n erny cvfgby.\n"
    "Ybtvpnyyl vg vf ohg boivbhfyl vg'f abg.",

    "Tnaterar : na raivebazragnyyl-sevraqyl tnat.\n"
    "-- Krkly",

    "QVQ LBH XABJ gung va Trbetvn lbh pnaabg yvir ba n obng sbe zber guna 30 qnlf\n"
    "qhevat gur pnyraqne lrne, rira vs lbh ner whfg cnffvat guebhtu gur fgngr?\n"
    "\n"
    "Ohg jung nobhg 30 qnlf va n lrne jvgubhg n pnyraqne?",

    "Qvq lbh xabj gung n tebhc bs cevfbaref vf pnyyrq n 'cvgl'?\n",

    "Qvq lbh xabj gung nabgure grez sbe fabj yrbcneq vf 'bhapr'?\n"
    "\n"
    "Guvf vf sebz Zvqqyr Ratyvfu sebz Byq Serapu bapr, rneyvre ybapr (gur y- orvat\n"
    "zvfvagrecergrq nf gur qrsvavgr negvpyr), onfrq ba Yngva ylak, ylap-.\n",

    "Qvq lbh xabj gung n ener jbeq sbe n tebhc bs jvyq pngf (abg jvyqpngf) vf\n"
    "'qrfgehpgvba'?\n",

    "Bhe havgf bs grzcbeny zrnfherzrag, sebz frpbaqf ba hc gb zbaguf,\n"
    "ner fb pbzcyvpngrq, nflzzrgevpny naq qvfwhapgvir fb nf gb znxr\n"
    "pburerag zragny erpxbavat va gvzr nyy ohg vzcbffvoyr.  Vaqrrq, unq\n"
    "fbzr glenaavpny tbq pbagevirq gb rafynir bhe zvaqf gb gvzr, gb znxr\n"
    "vg nyy ohg vzcbffvoyr sbe hf gb rfpncr fhowrpgvba gb fbqqra\n"
    "ebhgvarf naq hacyrnfnag fhecevfrf, ur pbhyq uneqyl unir qbar orggre\n"
    "guna unaqvat qbja bhe cerfrag flfgrz.  Vg vf yvxr n frg bs\n"
    "gencrmbvqny ohvyqvat oybpxf, jvgu ab iregvpny be ubevmbagny\n"
    "fhesnprf, yvxr n ynathntr va juvpu gur fvzcyrfg gubhtug qrznaqf\n"
    "beangr pbafgehpgvbaf, hfryrff cnegvpyrf naq yratgul\n"
    "pvephzybphgvbaf.  Hayvxr gur zber fhpprffshy cnggreaf bs ynathntr\n"
    "naq fpvrapr, juvpu ranoyr hf gb snpr rkcrevrapr obyqyl be ng yrnfg\n"
    "yriry-urnqrqyl, bhe flfgrz bs grzcbeny pnyphyngvba fvyragyl naq\n"
    "crefvfgragyl rapbhentrf bhe greebe bs gvzr.\n"
    "\n"
    "... Vg vf nf gubhtu nepuvgrpgf unq gb zrnfher yratgu va srrg, jvqgu\n"
    "va zrgref naq urvtug va ryyf; nf gubhtu onfvp vafgehpgvba znahnyf\n"
    "qrznaqrq n xabjyrqtr bs svir qvssrerag ynathntrf.  Vg vf ab jbaqre\n"
    "gura gung jr bsgra ybbx vagb bhe bja vzzrqvngr cnfg be shgher, ynfg\n"
    "Ghrfqnl be n jrrx sebz Fhaqnl, jvgu srryvatf bs urycyrff pbashfvba.\n"
    "...\n"
    "\n"
    "--Eboreg Tehqva, 'Gvzr naq gur Neg bs Yvivat'.\n",

    "Qvq lbh xabj gung n tebhc bs euvabf vf pnyyrq n 'penfu'? Thrff jul!\n",

    "QVQ LBH XABJ gung va Zvffvffvccv vg vf vyyrtny gb grnpu bguref jung cbyltnzl\n"
    "vf?\n"
    "\n"
    "'Bu qrnevr zr, V pna bayl unir bar qrne ng n gvzr! Jung rire funyy V qb?!'",

    "QVQ LBH XABJ gung va Sybevqn qbbef bs nyy choyvp ohvyqvatf zhfg bcra bhgjneqf?\n"
    "\n"
    "Abj vs bhgjneqf ersref gb gbjneqf gur bhgfvqr naq fvaxubyrf ner bhgfvqr qbrf\n"
    "gung zrna gung choyvp ohvyqvatf zhfg unir fvaxubyrf nf jryy?",

    "QVQ LBH XABJ gung va Abegu Pnebyvan vg vf ntnvafg gur ynj gb fvat bss xrl?\n"
    "\n"
    "Ybbcubyr: Fvg ba n cvnab juvyfg fvatvat fb gung lbh pna pynvz lbh'er ba xrl\n"
    "rira vs lbh'er bss xrl.",

    "Jr nyy, vapyhqvat - naq vaqrrq rfcrpvnyyl - gur Rnegu, unir bhe snhygf.\n"
    "-- Krkly",

    "  Perngbef Nqzvg P, Havk Jrer Ubnk\n"
    "\n"
    "  SBE VZZRQVNGR ERYRNFR\n"
    "  Va na naabhaprzrag gung unf fghaarq gur pbzchgre vaqhfgel, Xra Gubzcfba,\n"
    "  Qraavf Evgpuvr, naq Oevna Xreavtuna nqzvggrq gung gur Havk bcrengvat flfgrz\n"
    "  naq P cebtenzzvat ynathntr perngrq ol gurz vf na rynobengr Ncevy Sbbyf cenax\n"
    "  xrcg nyvir sbe zber guna 20 lrnef. Fcrnxvat ng gur erprag HavkJbeyq Fbsgjner\n"
    "  Qrirybczrag Sbehz, Gubzcfba erirnyrq gur sbyybjvat:\n"
    "\n"
    "     'Va 1969, NG&G unq whfg grezvangrq gurve jbex jvgu gur TR/NG&G Zhygvpf\n"
    "     cebwrpg. Oevna naq V unq whfg fgnegrq jbexvat jvgu na rneyl eryrnfr bs\n"
    "     Cnfpny sebz Cebsrffbe Avpuynhf Jvegu'f RGU ynof va Fjvgmreynaq, naq jr\n"
    "     jrer vzcerffrq jvgu vgf ryrtnag fvzcyvpvgl naq cbjre. Qraavf unq whfg\n"
    "     svavfurq ernqvat Oberq bs gur Evatf, n uvynevbhf Angvbany Ynzcbba cnebql\n"
    "     bs gur terng Gbyxvra Ybeq bs gur Evatf gevybtl. Nf n ynex, jr qrpvqrq gb\n"
    "     qb cnebqvrf bs gur Zhygvpf raivebazrag naq Cnfpny. Qraavf naq V jrer\n"
    "     erfcbafvoyr sbe gur bcrengvat raivebazrag. Jr ybbxrq ng Zhygvpf naq\n"
    "     qrfvtarq gur arj flfgrz gb or nf pbzcyrk naq pelcgvp nf cbffvoyr gb\n"
    "     znkvzvmr pnfhny hfref' sehfgengvba yriryf, pnyyvat vg Havk nf n cnebql bs\n"
    "     Zhygvpf, nf jryy nf bgure zber evfdhr nyyhfvbaf.\n"
    "\n"
    "     'Gura Qraavf naq Oevna jbexrq ba n gehyl jnecrq irefvba bs Cnfpny, pnyyrq\n"
    "     N. Jura jr sbhaq bguref jrer npghnyyl gelvat gb perngr erny cebtenzf\n"
    "     jvgu N, jr dhvpxyl nqqrq nqqvgvbany pelcgvp srngherf naq ribyirq vagb O,\n"
    "     OPCY, naq svanyyl P. Jr fgbccrq jura jr tbg n pyrna pbzcvyr ba gur\n"
    "     sbyybjvat flagnk:\n"
    "\n"
    "      sbe(;C(\\\"\\a\\\"),E=;C(\\\"|\\\"))sbe(r=P;r=C(\\\"_\\\"+(*h++/\n"
    "	   8)%2))C(\\\"|\\\"+(*h/4)%2);\n"
    "\n"
    "   'Gb guvax gung zbqrea cebtenzzref jbhyq gel gb hfr n ynathntr gung nyybjrq\n"
    "   fhpu n fgngrzrag jnf orlbaq bhe pbzcerurafvba! Jr npghnyyl gubhtug bs\n"
    "   fryyvat guvf gb gur Fbivrgf gb frg gurve pbzchgre fpvrapr cebterff onpx 20\n"
    "   be zber lrnef. Vzntvar bhe fhecevfr jura NG&G naq bgure H.F. pbecbengvbaf\n"
    "   npghnyyl ortna gelvat gb hfr Havk naq P!  Vg unf gnxra gurz 20 lrnef gb\n"
    "   qrirybc rabhtu rkcregvfr gb trarengr rira znetvanyyl hfrshy nccyvpngvbaf\n"
    "   hfvat guvf 1960f grpuabybtvpny cnebql, ohg jr ner vzcerffrq jvgu gur\n"
    "   granpvgl (vs abg pbzzba frafr) bs gur trareny Havk naq P cebtenzzre.\n"
    "\n"
    "   'Va nal rirag, Oevna, Qraavf, naq V unir orra jbexvat rkpyhfviryl va\n"
    "   Yvfc ba gur Nccyr Znpvagbfu sbe gur cnfg srj lrnef naq srry ernyyl\n"
    "   thvygl nobhg gur punbf, pbashfvba, naq gehyl onq cebtenzzvat gung\n"
    "   unf erfhygrq sebz bhe fvyyl cenax fb ybat ntb.'\n"
    "\n"
    "Znwbe Havk naq P iraqbef naq phfgbzref, vapyhqvat NG&G, Zvpebfbsg,\n"
    "Urjyrgg-Cnpxneq, TGR, APE, naq QRP unir ershfrq pbzzrag ng guvf gvzr.  Obeynaq\n"
    "Vagreangvbany, n yrnqvat iraqbe bs Cnfpny naq P gbbyf, vapyhqvat gur cbchyne\n"
    "Gheob Cnfpny, Gheob P, naq Gheob P++, fgngrq gurl unq fhfcrpgrq guvf sbe n\n"
    "ahzore bs lrnef naq jbhyq pbagvahr gb raunapr gurve Cnfpny cebqhpgf naq unyg\n"
    "shegure rssbegf gb qrirybc P. Na VOZ fcbxrfzna oebxr vagb hapbagebyyrq ynhtugre\n"
    "naq unq gb cbfgcbar n unfgvyl pbairarq arjf pbasrerapr pbapreavat gur sngr bs\n"
    "gur EF/6000, zreryl fgngvat 'Jbexcynpr BF jvyy or ninvynoyr Erny Fbba Abj.' Va\n"
    "n pelcgvp fgngrzrag, Cebsrffbe Jvegu bs gur RGU Vafgvghgr naq sngure bs gur\n"
    "Cnfpny, Zbqhyn 2, naq Boreba fgehpgherq ynathntrf, zreryl fgngrq gung C. G.\n"
    "Oneahz jnf pbeerpg.",

    "QVQ LBH XABJ gung va Arj Wrefrl vg vf ntnvafg gur ynj gb jrne n ohyyrgcebbs\n"
    "irfg juvyr pbzzvggvat n zheqre?\n"
    "\n"
    "Qbrf gung zrna vg'f yrtny gb qb fb vs lbh'er abg jrnevat n ohyyrgcebbs irfg?",

    "QVQ LBH XABJ gung va Craaflyinavn vg vf vyyrtny gb fyrrc ba gbc bs n\n"
    "ersevtrengbe bhgqbbef?\n"
    "\n"
    "Sbeghangryl nalbar gelvat gb qb fb jbhyq cebonoyl ebyy bss vg, znxvat vg\n"
    "vzcbffvoyr sbe gur nhgubevgvrf gb punetr gurz jvgu fyrrcvat ba n ersevtrengbe\n"
    "bhgqbbef. Bs pbhefr gurl zvtug oernx gurve onpx be arpx ohg gurl jba'g or\n"
    "punetrq jvgu fyrrcvat ba n ersevtrengbe!",

    "QVQ LBH XABJ gung va Buvb vg vf vyyrtny sbe zber guna svir jbzra gb yvir va n\n"
    "ubhfr?\n"
    "\n"
    "Vf guvf orpnhfr gurl'er nsenvq bs cebterff? V zrna vg'f hayvxryl gb unccra\n"
    "jvgu svir zra yvivat va n ubhfr hayrff cebterff vf nethvat!",

    "QVQ LBH XABJ gung va Arj Lbex gur cranygl sbe whzcvat bss n ohvyqvat vf qrngu?\n"
    "\n"
    "Arj Lbex, jurer fbzr crbcyr unir gjb yvirf. Whfg jnvg hagvy Arj Lbexref urne\n"
    "nobhg pngf!",

    "Vs Cynarg Rnegu jnf zragnyyl vyy vg jbhyq or qvntabfrq ovcbyne.\n"
    "-- Krkly",


    "Jura V svaq zl pbqr va gbaf bs gebhoyr,\n"
    "Sevraqf naq pbyyrnthrf pbzr gb zr,\n"
    "Fcrnxvat jbeqf bs jvfqbz:\n"
    "'Jevgr va P.'\n"
    "\n"
    "Nf gur qrnqyvar snfg nccebnpurf,\n"
    "Naq ohtf ner nyy gung V pna frr,\n"
    "Fbzrjurer, fbzrbar juvfcref:\n"
    "'Jevgr va P.'\n"
    "\n"
    "Jevgr va P, jevgr va P,\n"
    "Jevgr va P, bu, jevgr va P.\n"
    "YVFC vf qrnq naq ohevrq,\n"
    "Jevgr va P.\n"
    "\n"
    "V hfrq gb jevgr n ybg bs SBEGENA,\n"
    "Sbe fpvrapr vg jbexrq synjyrffyl.\n"
    "Gel hfvat vg sbe tencuvpf!\n"
    "Jevgr va P.\n"
    "\n"
    "Vs lbh'ir whfg fcrag arneyl 30 ubhef\n"
    "Qrohttvat fbzr nffrzoyl,\n"
    "Fbba lbh jvyy or tynq gb\n"
    "Jevgr va P.\n"
    "\n"
    "Jevgr va P, jevgr va P,\n"
    "Jevgr va P, lrnu, jevgr va P.\n"
    "Bayl jvzcf hfr ONFVP.\n"
    "Jevgr va P.\n"
    "\n"
    "Jevgr va P, jevgr va P,\n"
    "Jevgr va P, bu, jevgr va P.\n"
    "Cnfpny jba'g dhvgr phg vg.\n"
    "Jevgr va P.\n"
    "\n"
    "Jevgr va P, jevgr va P,\n"
    "Jevgr va P, lrnu, jevgr va P.\n"
    "Qba'g rira zragvba PBOBY.\n"
    "Jevgr va P.\n"
    "\n"
    "Naq jura gur fperra vf shmml,\n"
    "Naq gur rqvgbe vf ohttvat zr.\n"
    "V'z fvpx bs barf naq mrebf,\n"
    "Jevgr va P.\n"
    "\n"
    "N gubhfnaq crbcyr fjrne gung G.C.\n"
    "Frira vf gur bar sbe zr.\n"
    "V ungr gur jbeq CEBPRQHER,\n"
    "Jevgr va P.\n"
    "\n"
    "Jevgr va P, jevgr va P,\n"
    "Jevgr va P, lrnu, jevgr va P.\n"
    "CY1 vf '80f,\n"
    "Jevgr va P.\n"
    "\n"
    "Jevgr va P, jevgr va P,\n"
    "Jevgr va P, lrnu, jevgr va P.\n"
    "Gur tbireazrag ybirf NQN,\n"
    "Jevgr va P.",

    "Ubj qb zngurzngvpvnaf terrg crbcyr? Ol fvar jnirf.\n"
    "-- Krkly",

    "Fvqrjnyx: Jnyxvat fvqrjnlf.\n"
    "-- Krkly",

    "'Jr sbhaq gur fnaq jnf abg yvxr gur fnaq va gur oebpuher. Lbhe oebpuher fubjf\n"
    "gur fnaq nf juvgr ohg vg jnf zber lryybj.'\n"
    "\n"
    "Creuncf lbh fubhyq qevax zber jngre fb lbh'er abg nf qrulqengrq? :-)",

    " Wra: Vg whfg frrzf yvxr n jrveq cynpr gb tb ba sver.\n"
    " Ebl: Vg'f n irel jrveq cynpr gb tb ba sver! N sver ng n Frn Cnexf? Vg'f gur\n"
    "      jrveqrfg guvat V'ir rire urneq!  V zrna, znlor... Znlor vs gurer jrer\n"
    "      cynfgvp frngf, znlor gurl pbhyq tb ba...\n"
    "Zbff: Gurl qba'g unir cynfgvp frngf ng Frn Cnexf.\n"
    " Ebl: Ubj qb lbh xabj?\n"
    "\n"
    "Zbff: Ebl, jura V jnf ba ubyvqnl, V yvirq ng Frn Cnexf.  Vg'f gur shaarfg,\n"
    "      jrggrfg, zbfg fcyvfu-fcynful cynpr va gur jbeyq.\n"
    " Ebl: Bxnl, qvq lbh tb naq frr gur frn yvba fubj?\n"
    "Zbff: Guerr gvzrf n qnl sbe gjb jrrxf.\n"
    " Ebl: Naq qvq gurl unir cynfgvp...\n"
    "Zbff: Ab, vg'f nyy fgbar fgrcf.\n"
    " Ebl: Gura jung jrag ba sver? V'ir purpxrq Jvxvcrqvn. Gurer vf ab zragvba bs n\n"
    "      sver ng nal Frn Cnexf.\n"
    "\n"
    " Wra: Qb lbh guvax znlor fur'f ylvat?\n"
    " Ebl: Ab. V zrna, jul jbhyq fur yvr? Naq vs fur jnf tbvat gb yvr, jul jbhyq fur\n"
    "      hfr guvf bar? N sver ng n Frn Cnexf? Vgf jerpxvat zl urnq! V zrna, vs fur\n"
    "      unq fnvq gung ure cneragf unq qebjarq, V'q or gur unccvrfg zna va gur\n"
    "      jbeyq. Ohg n sver? Ng n Frn Cnexf? V zrna. Vg'f whfg... Vg whfg... Bu,\n"
    "      vg'f whfg... Vg bcraf hc n ybg bs dhrfgvbaf.\n"
    " Wra: Jryy, znlor lbh pbhyq nfx ure.\n"
    " Ebl: Ab, ab, ab, ab. Jr pna arire gnyx nobhg vg ntnva.\n"
    " Wra: Jryy, znlor lbh pbhyq whfg.\n"
    " Ebl: Ab, ab. Ab, ab, ab. Jr pna arire gnyx nobhg vg sbe gur erfg bs bhe yvirf.\n"
    " Wra: Jryy, gura V fhccbfr lbh'er whfg tbvat gb unir gb gel naq fgbc guvaxvat\n"
    "      nobhg vg.\n"
    "Zbff: Gung'f nabgure terng vqrn! Lbh'er ba sver! Vs lbh'yy cneqba gur\n"
    "      rkcerffvba.\n"
    "\n"
    "-- Gur VG Pebjq, Frevrf 4 Rcvfbqr 4",

    "Lrfgreqnl,\n"
    "Nyy gubfr onpxhcf frrzrq n jnfgr bs cnl.\n"
    "Abj zl qngnonfr unf tbar njnl.\n"
    "Bu V oryvrir va lrfgreqnl.\n"
    "\n"
    "Fhqqrayl,\n"
    "Gurer'f abg unys gur svyrf gurer hfrq gb or,\n"
    "Naq gurer'f n qrnqyvar\n"
    "unatvat bire zr.\n"
    "Gur flfgrz penfurq fb fhqqrayl.\n"
    "\n"
    "V chfurq fbzrguvat jebat\n"
    "Jung vg jnf V pbhyq abg fnl.\n"
    "Abj zl qngn'f tbar\n"
    "naq V ybat sbe lrfgreqnl-nl-nl-nl.\n"
    "\n"
    "Lrfgreqnl,\n"
    "Gur arrq sbe onpx-hcf frrzrq fb sne njnl.\n"
    "Gubhtug nyy zl qngn jnf urer gb fgnl,\n"
    "Abj V oryvrir va lrfgreqnl.",

    "QVQ LBH XABJ gung va Eubqr Vfynaq evqvat n ubefr bire nal choyvp uvtujnl sbe\n"
    "gur checbfr bs enpvat be grfgvat gur fcrrq bs gur ubefr vf vyyrtny?\n"
    "\n"
    "Jung nobhg qevivat n pne arkg gb n ubefr sbe gur fnzr checbfr?",

    "Qvq lbh xabj gung gur grpuavpny grezf sbe pncvgny yrggref ner 'znwhfphyr' naq\n"
    "'hapvny' naq gur grpuavpny grez sbe ybjre pnfr yrggref vf 'zvahfphyr'?\n",

    "QVQ LBH XABJ gung va Arj Unzcfuver lbh znl abg gnc lbhe srrg, abq lbhe urnq, be\n"
    "va nal jnl xrrc gvzr gb gur zhfvp va n gnirea, erfgnhenag be pnsé?\n"
    "\n"
    "Arj Unzcfuver, jurer zhfvpvnaf cynl bssorng!",

    "Gjb purzvfgf jnyx vagb n one.\n"
    "\n"
    "Gur svefg purzvfg fnlf, 'V'yy unir n tynff bs U2B.'\n"
    "\n"
    "Gur frpbaq purzvfg fnlf, 'V'yy unir n tynff bs U2B gbb.'\n"
    "Gur frpbaq purzvfg qvrf.",

    "'Lbh ner n yvne. V nz ab zber n jvgpu guna lbh ner n jvmneq naq vs lbh gnxr\n"
    "njnl zl yvsr tbq jvyy tvir lbh oybbq gb qevax!'\n"
    "\n"
    "Ynfg jbeqf bs pbaqrzarq 'jvgpu' Fnenu Tbbq 19 Whyl 1692 gb Erireraq Avpbynf\n"
    "Ablrf nsgre ur gevrq gb trg ure gb 'pbasrff'. 25 lrnef yngre va 1717 ur jbhyq\n"
    "qvr sebz n unrzbeeuntr, pubxvat ba uvf bja oybbq.\n",

    "Shaal Trezna fnlvat:\n"
    "\n"
    "    Rva Znaa rva Jbeg\n"
    "    Rvar Senh rva Jöegreohpu.\n"
    "\n"
    "    Bar zna bar jbeq\n"
    "    bar jbzna bar qvpgvbanel.\n",

    "OERNXVAT ARJF sebz ANFN, 18 Znepu 2022, zber guna 52 lrnef nsgre gur svefg\n"
    "ynaqvat ba gur Zbba naq zber guna 62 lrnef nsgre gur perngvba bs ANFN.\n"
    "\n"
    "Va na naabhaprzrag gung gung unf fubpxrq fbzr crbcyr, znqr fbzr fzht naq chg\n"
    "bguref va qvforyvrs, ANFN unf sbe gur svefg gvzr naabhaprq gung obgu gur Zbba\n"
    "naq gur Fha ubfg yvsr naq jung jr nyy gubhtug jrer pbvapvqragny anzrf ner\n"
    "nalguvat ohg.\n"
    "\n"
    "Vg gheaf bhg gung zbba ornef bevtvanyyl pbzr sebz gur Zbba naq fha ornef\n"
    "bevtvanyyl pbzr sebz gur Fha. Gur qvrg bs zbba ornef vf zbfgyl ubarlzbbaf ba\n"
    "fgvpxf naq gur qvrg bs gur fha orne vf fha-onxrq ubarl pnxrf.\n"
    "\n"
    "Jung'f zber vf gung gur zbba ornef naq fha ornef ba Rnegu ner npghnyyl yrff\n"
    "vagryyvtrag guna gubfr ba gur Zbba naq Fha. Nppbeqvat gb n fcbxrfcrefba sebz\n"
    "ANFN gur zbba ornef ba gur Zbba naq fha ornef ba gur Fha pna gnyx gb rnpu bgure\n"
    "va jnlf gung zbfg uhznaf pna'g cbffvoyl sngubz.\n"
    "\n"
    "Jura fprcgvpf nfxrq jul gurer'f ab cubgbtencuvp rivqrapr bs gur ornef gur\n"
    "fcbxrfcrefba fnvq gung vg'f orpnhfr gur ornef ner rkgerzryl ful bs uhznaf naq\n"
    "nggrzcg gb nibvq nyy pbagnpg jvgu zna, ntnva fubjvat zber vagryyvtrapr guna\n"
    "gurve Rnegu qrfpraqnagf.\n"
    "\n"
    "Nf sbe ubj gur fha ornef pna cbffvoyl yvir ba gur Fha be gur zbba ornef ba gur\n"
    "Zbba, vg'f fbzrguvat gung nsgre nyy gurfr lrnef ANFN unf lrg gb svther bhg; vg\n"
    "frrzf vzcbffvoyr ohg nsgre nyy, gurfr ornef ner uvtuyl vagryyvtrag naq unir\n"
    "qrprvirq uhznaf sbe nyy gurfr lrnef naq guebhtu ribyhgvba gurl pregnvayl pnzr\n"
    "hc jvgu jnlf gb fheivir whfg yvxr jr unir qrfcvgr bhe frevbhf varcgvghqrf naq\n"
    "irel pybfr pnyyf gbjneqf rkgvapgvba.\n",

    "QVQ LBH XABJ gung va Vaqvnan bar nezrq cvnavfgf zhfg cresbez sbe serr?\n"
    "\n"
    "Vs gur ynjznxref jrer ybtvpny, gurl jbhyq unir znqr vg unys cevpr sbe unys n\n"
    "cresbezre. Ohg ernyvfgvpnyyl gurl fubhyq or noyr gb punetr gjvpr nf zhpu gb\n"
    "pbzcrafngr gurve zvffvat (bgure?) unys.",

    "QVQ LBH XABJ gung va Zvffbhev fvatyr zra orgjrra gur ntrf bs 21 naq 50 zhfg cnl\n"
    "na naahny gnk bs bar qbyyne?\n"
    "\n"
    "Vs lbh guvax orvat nybar vf onq, vg'f rira jbefr va Zvffbhev jurer vs lbh'er\n"
    "n zna orgjrra gur ntrf bs 21 naq 50 lbh ner gnkrq! Gung'f tbg gb or n\n"
    "pbasvqrapr obbfgre!",

    "QVQ LBH XABJ gung va Vyyvabvf lbh znl or neerfgrq sbe intenapl vs lbh qb abg\n"
    "unir ng yrnfg bar qbyyne ba lbhe crefba?\n"
    "\n"
    "Va bgure jbeqf, va Vyyvabvf vs lbh'er n ohz lbh unir gb unir ng yrnfg n\n"
    "qbyyne ng nyy gvzrf. Vs lbh'er n ohz va gur erfg bs gur Havgrq Fgngrf bs\n"
    "Nzrevpn lbh qba'g. Vs ubjrire lbh'er n ohz va gur HX - jryy vg zvtug or rira\n"
    "jbefr!",

    "QVQ LBH XABJ gung va Vbjn zbhfgnpurf ner vyyrtny vs gur ornere unf n graqrapl\n"
    "gb unovghnyyl xvff bgure uhznaf?\n"
    "\n"
    "Nf bccbfrq gb abg graqvat gb unovghnyyl xvff bguref?",


    "QVQ LBH XABJ gung va Qrynjner vg vf vyyrtny gb syl bire nal obql bs jngre,\n"
    "hayrff bar vf pneelvat fhssvpvrag fhccyvrf bs sbbq naq qevax?\n"
    "\n"
    "Jung qbrf guvf zrna sbe gubfr jvgu naberkvn areibfn? Jung nobhg ohyvzvn\n"
    "areibfn be nabgure rngvat qvfbeqre? Jung vs lbh'er ba n fgevpg qvrg be vs sbe\n"
    "fbzr fgenatr ernfba lbh'er sylvat gb unir n pbybabfpbcl?",

    "N zna jnf pebffvat n ebnq bar qnl jura n sebt pnyyrq bhg gb uvz naq fnvq, 'Vs\n"
    "lbh xvff zr, V'yy ghea vagb n ornhgvshy cevaprff.'\n"
    "\n"
    "Ur orag bire, cvpxrq hc gur sebt, naq chg vg va uvf cbpxrg. Gur sebt fcbxr hc\n"
    "ntnva naq fnvq, 'Vs lbh xvff zr naq ghea zr onpx vagb n ornhgvshy cevaprff, V\n"
    "jvyy gryy rirelbar ubj fzneg naq oenir lbh ner naq ubj lbh ner zl ureb.'\n"
    "\n"
    "Gur zna gbbx gur sebt bhg bs uvf cbpxrg, fzvyrq ng vg, naq erghearq vg gb uvf\n"
    "cbpxrg. Gur sebt fcbxr hc ntnva naq fnvq, 'Vs lbh xvff zr naq ghea zr onpx vagb\n"
    "n ornhgvshy cevaprff, V jvyy or lbhe ybivat pbzcnavba sbe na ragver jrrx.'\n"
    "\n"
    "Gur zna gbbx gur sebt bhg bs uvf cbpxrg, fzvyrq ng vg, naq erghearq vg gb uvf\n"
    "cbpxrg. Gur sebt gura pevrq bhg, 'Vs lbh xvff zr naq ghea zr onpx vagb n\n"
    "cevaprff, V'yy fgnl jvgu lbh sbe n lrne naq qb NALGUVAT lbh jnag.'\n"
    "\n"
    "Ntnva gur zna gbbx gur sebt bhg, fzvyrq ng vg, naq chg vg onpx vagb uvf cbpxrg.\n"
    "Svanyyl, gur sebt nfxrq, 'Jung vf gur znggre? V'ir gbyq lbh V'z n ornhgvshy\n"
    "cevaprff, gung V'yy fgnl jvgu lbh sbe n lrne naq qb nalguvat lbh jnag. Jul\n"
    "jba'g lbh xvff zr?'\n"
    "\n"
    "Gur zna fnvq, 'Ybbx, V'z n pbzchgre cebtenzzre. V qba'g unir gvzr sbe n\n"
    "tveysevraq, ohg n gnyxvat sebt vf pbby.'",
    NULL
};



/*
 * vrergfB - "What ever can 'vrergfB' mean ?!"
 *
 * Given:
 *	42
 *
 * returns:
 *	"No it doesn't"!
 */
void
vrergfB(int four, int two)
{
    struct tm *tm = NULL;	/* there is NULL time like this time */
    int ret;			/* libc return value */
    unsigned uret;		/* unsigned libc return value */
    size_t ic = 0;		/* What is IC? First ask yourself what OOC is! */

    /*
     * The next comment is as empty as this one.
     */
    uintmax_t max = 42;

    /*
     * The above comment isn't any more empty than this one but the next one
     * might be.
     */
    if (four < 0 || two < 0) {

	/*
	 * time and time again we need to reconsider the argument
	 */
	t = time(NULL);
	if (t == (time_t)-1)
	    t = (time_t)02256330241;
	tm = localtime(&t);
	if (tm == NULL) {
	    if (four < 0)
		four = 0;
	    if (two < 0)
		two = 2*3*0521;
	} else {
	    if (four < 0)
		four = tm->tm_yday;
	    if (two < 0)
		two = tm->tm_year + 2*2*5*5*0x13;
	}
    }

    /*
     * It turns out the above comment might not have been empty but can we be
     * sure of this?
     */
    if ((max=42)) /* max should always be > 0 but we check due to division below */
	for (max /= 2 /* max is half itself due to table compression */+ max; oebxergfB[max] != NULL; ++max)
	    ;	/* the bottom bee is Eric the semi-bee */

    /*
     * "You are expected to understand this but we're not helping if you don't :-)
     * ...but whatever you do not don't don't panic!" :-(
     */
    for (char const *p = oebxergfB[((two*2*2*015 + (int)(four / (07&0x07))) % max)]; *p; ++p)
    {
	errno = 0;	/* we didn't think much about what was previously stored in errno */

	/*
	 * Q: We really don't need this test, so why is it here?
	 * A: Because! :-)
	 *
	 * PS: Don't try removing it without knowing what it's for.
	 *
	 * PPS: Don't try removing it even if you know what it's about.
	 *
	 * PPPS: Even if you don't know what it's about we still insist you do
	 * not remove it.
	 */
	if (*p == '\\' && p[1] == 'n') {
	    ret = fputc(012, stdout);
	    if (ret != 0x0a) {
		fwarnp(stderr, "abcdefg...", "the line must not have been new after all\n");
	    }
	    ++p; /* be positive and look forward to the next one! */
	/*
	 * This comment is important in understanding absolutely nothing.
	 */
	} else if (*p == '\\' && p[1] == '"') {
	    ret = fputc('"', stdout);
	    if (ret != '"') {
		fwarnp(stderr, "abcdefg...", "don't panic but the quote was unquoted!");
	    }
	    ++p; /* be positive and look backwards to the next one! */
	/*
	 * This comment is absolutely critical in understanding everything
	 * below the comment that is important in understanding absolutely
	 * nothing.
	 */
	/*
	 * This comment is obfuscated and so is the next comment.
	 */
	} else if (!isascii(*p)) {
	    ret = putchar(*p);
	    if (ret == EOF) {
		fwarnp(stderr, "\b\b\b\b\b\b\b\b\bEOF", "if we didn't ASCII stupid question don't give us a stupid ANSI!\n");
	    } else {
		++ic;
	    }

	/*
	 * The previous comment wasn't obfuscated but this one wasn't either.
	 */
	} else if (!isalpha(*p)) {
	    ret = putchar(*p);
	    if (ret == EOF) {
		fwarnp(stderr, "abcdefg..", "that was NOT in character :-(\n");
	    } else {
		++ic;
	    }

	/*
	 * This comment explains the below comments because they don't explain
	 * themselves.
	 */
	} else {
	    /* case: just in case we consider the case */
	    ret = putchar(islower(*p) /* this is a functional equivalent to a question mark: */ ?
		/* absolute trigonometric runaround */
		"nxistdwhowakprqfcvgzhjskelyybume"[((int)fabs(0x3BF261*sin((double)((*p-0141+(42-(1<<4)))%(0x2E4%42)))))&0x1f]
		: /* you are looking at the functional equivalent of a colon */
		/* try to absolutely positively trig this upper class character */
		"NXAUTMWORWCKBLQPZDXVQJSTFHYlGIEE"[((int)fabs(043431066*sin((double)((1+*p-0x42+(0xd<<1))%((2*0x0DF)%42)))))&037]
		);
	    if (ret == EOF) {
		fwarnp(stderr, "abcdefg..", "that character was absolutely mixed with sin!\n");
	    } else {
		++ic;
	    }
	}
    }

    /*
     * The next comment might be empty.
     */

    /*
     * This comment isn't empty but the next one isn't empty and the above one
     * was empty.
     */
    errno = 0;	/* be positive: pretend we have 0 errors so far */
    ret = fputc(0x0a, stdout);
    if (ret != 0x0a) {
	fwarnp(stderr, "abcdefg ...", "\nmeet the new line, same as the old line\n");
    }
    /*
     * This comment is empty but so is the next one.
     */
    dbg(DBG_LOW, "FUN FACT: there %s %ju in character character%s.", ic != 1 ? "were":"was", (uintmax_t) ic, ic != 1 ? "s":"");

    /*
     * and in the end ... take a moment to bow before exiting stage left
     */
#if 0	/* you might argue if we need to restate our argument here and now */
    printf("four: %d two: %d\n", four, two);
#endif
    errno -= errno;	/* we choose to retract the last error */
    uret = sleep(1+(((four+two)>0?(four+two):(-two-four))%5));
    if (uret != 0+0) {
	fwarnp(stderr, __func__, "possible insomnia detected ... whee! :-)");
    }
    (void) exit(1+(((four-two)>0?(four-two):(-four+two)) % 254)); /*ooo*/
    not_reached();
    /*
     * If you've read this far you've read more than the necessary and you
     * should pat yourself on the back for a job well done.
     */
    return;
}
/*
 * If you get here you're not as lazy as the C pre-processor which just removes
 * comments.
 */
