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
    "vf yrkvpbtencuvpnyyl yrff guna '>'. Fbzrbar fperjrq hc.\n-- Krkly",
    "V qba'g xabj unys bs lbh unys nf jryy nf V fubhyq yvxr;\nnaq V yvxr yrff guna"
    " unys bs lbh unys nf jryy nf lbh qrfreir.\n-- Ovyob Onttvaf",
    "P frpgvba: '{}'\n-- Krkly\n",
    "'Frpbaqyl, gb pryroengr BHE oveguqnlf: zvar naq zl ubabhenoyr naq tnyynag\n"
    "sngure'f.' Hapbzsbegnoyr naq nccerurafvir fvyrapr. 'V nz bayl unys gur zna\n"
    "gung ur vf: V nz 72, ur vf 144. Lbhe ahzoref ner pubfra gb qb ubabhe gb\n"
    "rnpu bs uvf ubabhenoyr lrnef.' Guvf jnf ernyyl qernqshy -- n erthyne\n"
    "oenvagjvfgre, naq fbzr bs gurz sryg vafhygrq, yvxr yrnc-qnlf fubirq\n"
    "va gb svyy hc n pnyraqne.\n-- Ovatb Onttvaf",
    "P havbaf: ybivat pbhcyrf jub rawbl cebtenzzvat va P gbtrgure.\n-- Krkly\n",
    "'Ybbx,' fnvq Neguhe, 'jbhyq vg fnir lbh n ybg bs gvzr vs V whfg tnir\n"
    "hc naq jrag znq abj?'\n",
    "Gurer vf na neg, be, engure, n xanpx gb sylvat. Gur xanpx yvrf va\n"
    "yrneavat ubj gb guebj lbhefrys ng gur tebhaq naq zvff.\n",
    "SHA SNPG: Va O naq rneyl P, gur += bcrengbe jnf fcryg =+ vafgrnq bs += ;\n"
    "guvf zvfgnxr, ercnverq va 1976, jnf vaqhprq ol n frqhpgviryl rnfl jnl bs\n"
    "unaqyvat gur svefg sbez va O'f yrkvpny nanylfre.\n\nFrr zber sha snpgf naq"
    " uvfgbel ol Qraavf Evgpuvr uvzfrys ng\nuggcf://jjj.oryy-ynof.pbz/hfe/qze/jjj/puvfg.ugzy.\n",
    "'Jung vf vg?' tebjyrq Fnz, zvfvagrecergvat gur fvtaf. 'Jung'f gur arrq gb\n"
    "favss? Gur fgvax arneyl xabpxf zr qbja jvgu zl abfr uryq. Lbh fgvax, naq znfgre\n"
    "fgvaxf; gur jubyr cynpr fgvaxf.'\n\n'Lrf, lrf, naq Fnz fgvaxf!' nafjrerq Tbyyhz. "
    "'Cbbe Fzéntby fzryyf vg,\nohg tbbq Fzéntby ornef vg. Urycf avpr znfgre. Ohg gung'f"
    " ab znggre. Gur nve'f\nzbivat, punatr vf pbzvat. Fzéntby jbaqref; ur'f abg unccl.'\n",
    "'Anl! Abg Ryirf,' fnvq gur sbhegu, gur gnyyrfg, naq nf vg nccrnerq gur\npuvrs nzbat"
    " gurz. 'Ryirf qb abg jnyx va Vguvyvra va gurfr qnlf. Naq\nRyirf ner jbaqebhf snve gb"
    " ybbx hcba, be fb 'gvf fnvq.'\n\n'Zrnavat jr'er abg, V gnxr lbh,' fnvq Fnz. 'Gunax"
    " lbh xvaqyl. Naq jura\nlbh'ir svavfurq qvfphffvat hf, creuncf lbh'yy fnl jub lbh ner,"
    " naq\njul lbh pna'g yrg gjb gverq geniryyref erfg.'\n",
    "Tvyqbe jnf fvyrag sbe n zbzrag. 'V qb abg yvxr guvf arjf,' ur fnvq\n"
    "ng ynfg. 'Gung Tnaqnys fubhyq or yngr, qbrf abg obqr jryy. Ohg vg\nvf fnvq:"
    " Qb abg zrqqyr va gur nssnvef bs Jvmneqf, sbe gurl ner\nfhogyr naq dhvpx gb "
    "natre. Gur pubvpr vf lbhef: gb tb be jnvg.'\n\n'Naq vg vf nyfb fnvq,' nafjrerq"
    " Sebqb: 'Tb abg gb gur Ryirf sbe\npbhafry, sbe gurl jvyy fnl obgu ab naq lrf.'",
    "Bar bs gur guvatf Sbeq Cersrpg unq nyjnlf sbhaq uneqrfg gb haqrefgnaq nobhg\n"
    "uhznaf jnf gurve unovg bs pbagvahnyyl fgngvat naq ercrngvat gur irel irel\n"
    "boivbhf.\n\nBar bs gur guvatf Sbeq Cersrpg unq nyjnlf sbhaq uneqrfg gb haqrefgnaq nobhg\n"
    "uhznaf jnf gurve unovg bs pbagvahnyyl fgngvat naq ercrngvat gur irel irel\nboivbhf.",
    "Sbe n zbzrag, abguvat unccrarq. Gura, nsgre n frpbaq be fb, abguvat\npbagvahrq gb unccra.",
    "'Vs V rire zrrg zlfrys,' fnvq Mncubq, 'V'yy uvg zlfrys fb uneq V jba'g xabj\njung'f uvg zr.'",
    "'V frr abobql ba gur ebnq,' fnvq Nyvpr.\n\n'V bayl jvfu gung V unq fhpu rlrf,' gur Xvat erznexrq"
    " va n sergshy\ngbar. 'Gb or noyr gb frr Abobql! Naq ng gung qvfgnapr gbb! Jul, vg'f\nnf zhpu nf V"
    " pna qb gb frr erny crbcyr, ol guvf yvtug!'\n\n[...]\n\n'Jub qvq lbh cnff ba gur ebnq?' gur Xvat jrag"
    " ba, ubyqvat bhg uvf\nunaq gb gur Zrffratre sbe fbzr zber unl.\n\n'Abobql,' fnvq gur Zrffratre.\n\n'"
    "Dhvgr evtug,' fnvq gur Xvat: 'guvf lbhat ynql fnj uvz gbb. Fb bs pbhefr Abobql\njnyxf fybjre guna lbh.'\n\n"
    "'V qb zl orfg,' gur Zrffratre fnvq va n fhyxl gbar. 'V'z fher abobql jnyxf zhpu\nsnfgre guna V qb!'\n\n"
    "'Ur pna'g qb gung,' fnvq gur Xvat, 'be ryfr ur'q unir orra urer svefg.'",
    "'Guvf vf n puvyq!' Unvtun ercyvrq rntreyl, pbzvat va sebag bs Nyvpr gb\n"
    "vagebqhpr ure, naq fcernqvat bhg obgu uvf unaqf gbjneqf ure va na Natyb-Fnkba\n"
    "nggvghqr. 'Jr bayl sbhaq vg gb-qnl. Vg'f nf ynetr nf yvsr, naq gjvpr nf\nangheny!'\n\n"
    "'V nyjnlf gubhtug gurl jrer snohybhf zbafgref!' fnvq gur Havpbea. 'Vf vg\nnyvir?'\n\n'"
    "Vg pna gnyx,' fnvq Unvtun, fbyrzayl.\nGur Havpbea ybbxrq qernzvyl ng Nyvpr, naq fnvq 'Gnyx, puvyq.'\n"
    "Nyvpr pbhyq abg uryc ure yvcf pheyvat hc vagb n fzvyr nf fur ortna: 'Qb lbh\nxabj, V nyjnlf gubhtug"
    " Havpbeaf jrer snohybhf zbafgref, gbb! V arire fnj bar\nnyvir orsber!'\n\n[...]\n\n'Nu, jung vf vg, abj?'"
    " gur Havpbea pevrq rntreyl. 'Lbh'yy arire thrff! V\npbhyqa'g.' Gur Yvba ybbxrq ng Nyvpr jrnevyl. 'Ner lbh"
    " na navzny -- irtrgnoyr --\nbe zvareny?' ur fnvq, lnjavat ng rirel bgure jbeq.\n\n'Vg'f n snohybhf zbafgre!'"
    " gur Havpbea pevrq bhg, orsber Nyvpr pbhyq ercyl.\n\n'Gura unaq ebhaq gur cyhz-pnxr, Zbafgre,' gur Yvba fnvq.",
    "V ortna gb unir qbhogf nobhg ubj zngu vf gnhtug va guvf pbhagel jura n lbhat\n"
    "pbbx ng zl cvmmrevn fnvq ur unq n ceboyrz. 'N phfgbzre pnyyrq, nfxvat sbe n\ncvr gung'f n"
    " guveq purrfr, n guveq crccrebav, naq n guveq pbzovangvba,' ur\nfnvq. 'Fb, jung'f gur ceboyrz?' "
    "V nfxrq uvz. 'V qba'g xabj jung gb chg ba\ngur sbhegu guveq'.\n\n-EBOREG XRNEARL\nEnapub Pbeqbin,\nPnyvsbeavn",
    "'Naq lbh qb Nqqvgvba?' gur Juvgr Dhrra nfxrq. 'Jung'f bar naq bar naq bar naq\nbar naq bar naq bar"
    " naq bar naq bar naq bar naq bar?' 'V qba'g xabj,' fnvq\nNyvpr. 'V ybfg pbhag.'\n\n'Fur pna'g qb Nqqvgvba,'"
    " gur Erq Dhrra vagreehcgrq. 'Pna lbh qb Fhogenpgvba?'\nGnxr avar sebz rvtug.' 'Avar sebz rvtug V pna'g, lbh xabj,'"
    " Nyvpr ercyvrq irel\nernqvyl: 'ohg--'\n\n'Fur pna'g qb Fhogenpgvba,' fnvq gur Juvgr Dhrra. 'Pna lbh qb Qvivfvba? "
    "Qvivqr\nn ybns ol n xavsr -- jung'f gur nafjre gb gung?' 'V fhccbfr--' Nyvpr jnf\nortvaavat, ohg gur Erq Dhrra "
    "nafjrerq sbe ure.  'Oernq-naq-ohggre, bs pbhefr.\nGel nabgure Fhogenpgvba fhz. Gnxr n obar sebz n qbt: jung erznvaf?'\n\n"
    "Nyvpr pbafvqrerq. 'Gur obar jbhyqa'g erznva, bs pbhefr, vs V gbbx vg--naq gur\nqbt jbhyqa'g erznva; vg jbhyq pbzr gb ovgr "
    "zr--naq V'z fher V fubhyqa'g\nerznva!' 'Gura lbh guvax abguvat jbhyq erznva?' fnvq gur Erq Dhrra.\n\n'V guvax gung'f gur "
    "nafjre.' 'Jebat, nf hfhny,' fnvq gur Erq Dhrra: 'gur qbt'f\ngrzcre jbhyq erznva.' 'Ohg V qba'g frr ubj --'\n\n"
    "'Jul, ybbx urer!' gur Erq Dhrra pevrq. 'Gur qbt jbhyq ybfr vgf grzcre, jbhyqa'g\nvg?'\n\n"
    "'Creuncf vg jbhyq,' Nyvpr ercyvrq pnhgvbhfyl. 'Gura vs gur qbt jrag njnl, vgf\ngrzcre jbhyq erznva!' "
    "gur Dhrra rkpynvzrq gevhzcunagyl.",
    "Urer gur Erq Dhrra ortna ntnva. 'Pna lbh nafjre hfrshy dhrfgvbaf?' fur fnvq.\n"
    "'Ubj vf oernq znqr?'\n\n'V xabj gung!' Nyvpr pevrq rntreyl.' Lbh gnxr fbzr sybhe--'\n"
    "'Jurer qb lbh cvpx gur sybjre?' gur Juvgr Dhrra nfxrq. 'Va n tneqra, be va gur\nurqtrf?'\n\n"
    "'Jryy vg vfa'g cvpxrq ng nyy,' Nyvpr rkcynvarq: 'vg'f tebhaq--'\n'Ubj znal nperf bs tebhaq?' "
    "fnvq gur Juvgr Dhrra. 'Lbh zhfga'g yrnir bhg fb\nznal guvatf.'\n\n'Sna ure urnq!' gur Erq Dhrra "
    "nakvbhfyl vagreehcgrq. 'Fur'yy or srirevfu nsgre\nfb zhpu guvaxvat.' Fb gurl frg gb jbex naq snaarq "
    "ure jvgu ohapurf bs yrnirf,\ngvyy fur unq gb ort gurz gb yrnir bss, vg oyrj ure unve nobhg fb.",
    "SHA SNPG:\n\nVa Yvsr, gur Havirefr naq Rirelguvat, gur guveq vafgnyzrag bs Gur Uvgpuuvxre'f\n"
    "Thvqr gb gur Tnynkl ol Qbhtynf Nqnzf, vg'f fnvq gur nafjre gb Yvsr, gur\nHavirefr naq Rirelguvat vf 42. "
    "Qbhtynf Nqnzf vafvfgrq guvf jnf n wbxr, jevgvat\nba HFRARG:\n\n    Gur nafjre gb guvf vf irel fvzcyr. Vg "
    "jnf n wbxr. Vg unq gb or n ahzore, na\n    beqvanel, fznyyvfu ahzore, naq V pubfr gung bar. Ovanel ercerfragngvbaf,"
    "\n    onfr guvegrra, Gvorgna zbaxf ner nyy pbzcyrgr abafrafr. V fng ng zl qrfx,\n    fgnerq vagb gur tneqra naq gubhtug"
    " '42 jvyy qb' V glcrq vg bhg. Raq bs\n    fgbel.\n\nOhg qvq lbh xabj gung fbzr nfgebabzref FCRAG GJB LRNEF fghqlvat guvf "
    "naq VAFVFG\ngung 42 vf npghnyyl pbeerpg? Sbe guvf fgbel frr:\n\n"
    "uggcf://jjj.vaqrcraqrag.pb.hx/arjf/lrf-gur-nafjre-gb-gur-havirefr-ernyyl-vf-42-1351201.ugzy",
    "Vg'f pbzzbayl fhttrfgrq gung P++ vf na vzcebirzrag bire P. Guvf vf n zlgu\n"
    "qrevivat sebz gur snpg gung gur bcrengbe ++ va P vapernfrf vgf bcrenaq nf n fvqr\n"
    "rssrpg: gur inyhr bs gur bcrengvba qrcraqvat ba vs vg'f cbfgsvk be cersvk ++. Nf\n"
    "vg unccraf P++ vf cbfgsvk ++. Guvf zrnaf gur inyhr bs gur rkcerffvba vf P naq nf\n"
    "n fvqr rssrpg P vf vaperzragrq ol 1. Guvf cebirf gung P vf fhcrevbe - jung'f\n"
    "yrsg, P + 1, vf oybng naq vg'f htyl. Vg'f uhtr naq vaqrprag. Sbe P++'f dhnyvgl\ngel P = 0;.\n    -- Krkly",
    "Unir lbh gevrq sbepvat na harkcrpgrq erobbg?\n\n"
    "Frr, gur qevire ubbxf gur shapgvba ol cngpuvat gur flfgrz pnyy gnoyr, fb vg'f\n"
    "abg fnsr gb haybnq vg hayrff nabgure guernq'f nobhg gb whzc va naq qb vgf\n"
    "fghss, naq lbh qba'g jnag gb raq hc Va gur zvqqyr bs vainyvq zrzbel.\n\n"
    "    chfu BkS9QNSSOB\n    pnyy nevrf+BkR90(S9Q8SR90)\n    zbi rqk, [rpk+1]\n"
    "    zbi nk, qf:XrFreivprQrfpevcgbeGnoyr\n    zbi qjbeq_110PB, rqk\n"
    "    zbi rnk, qf:MjDhrelQverpgbelSvyr\n\nUryyb?\n\n-- Znhevpr Zbff, Gur VG "
    "Pebjq Frevrf 1 Rcvfbqr 1, 'Lrfgreqnl'f Wnz'.",
    "QVQ LBH XABJ gung va Jnfuvatgba jura gjb genvaf pbzr gb n pebffvat, arvgure pna\ntb hagvy gur bgure unf cnffrq?\n\nVa juvpu pnfr, obgu yvgrenyyl naq ybtvpnyyl fcrnxvat, arvgure genva pna rire\ntb. Guvf va ghea zrnaf gung gjb zber genvaf jbhyq pbzr, hanoyr gb cebprrq naq\ngura gjb zber naq riraghnyyl gur envyjnl jbhyq or oybpxrq, ceriragvat pnef sebz\npebffvat nf jryy, znxvat n uhtr genssvp wnz!",
    "QVQ LBH XABJ gung va Bertba n qbbe ba n pne znl abg or yrsg bcra ybatre guna\nvf arprffnel?\n\nSvanyyl jr unir na rkcynangvba bs jul gurer ner fb znal pne qbbef ylvat ba gur\nfgerrgf va Bertba!",
    "QVQ LBH XABJ gung va Pnyvsbeavn navznyf ner onaarq sebz zngvat choyvpyl jvguva\n1500 srrg bs n gnirea, fpubby, be cynpr bs jbefuvc?\n\nOhg ng yrnfg bapr gurl'er wnvyrq, boivbhfyl va n mbb, gurl pna qb vg nf\nbsgra nf gurl yvxr!",
    "QVQ LBH XABJ gung va Arinqn vg vf vyyrtny gb qevir n pnzry ba gur uvtujnl?\n\nJurgure guvf zrna tvir n pnzry n yvsg ba gur uvtujnl be npghnyyl hfvat gur\npnzry nf n iruvpyr vf hapyrne, whfg yvxr gur checbfr bs guvf ynj vgfrys\nrfcrpvnyyl fvapr gurer zvtug abg or znal pnzryf yrsg va Arinqn naljnl.",
    "QVQ LBH XABJ gung va Vqnub vg vf vyyrtny sbe n zna gb tvir uvf fjrrgurneg n obk\nbs pnaql jrvtuvat yrff guna 50 cbhaqf (22.6796xt)?\n\nV org pnaql fubcf ybir guvf ynj!",
    "QVQ LBH XABJ gung va Zbagnan vg vf vyyrtny gb unir n furrc va gur pno bs lbhe\ngehpx jvgubhg n puncrebar?\n\nNccneragyl va Zbagnan furrc ner uhznaf! Be ryfr ... ynjlref qba'g xabj jung\npuncrebar zrnaf.",
    "QVQ LBH XABJ gung va Jlbzvat vg vf vyyrtny gb jrne n ung gung bofgehpgf\ncrbcyr’f ivrj va n choyvp gurnger be cynpr bs nzhfrzrag?\n\n...V guvax guvf npghnyyl fbhaqf ernfbanoyr ohg V'z nyfb n qjnes. Gura ntnva,\njura ylvat qbja jr'er nyy qjneirf!",
    "QVQ LBH XABJ gung gur Xnafnf fgngr tnzvat obneq cebuvovgf gur hfr bs zhyrf gb\nuhag qhpxf?\n\nV'z abg fher znal crbcyr guvax zhyrf pna cbffvoyl uryc uhagvat qhpxf ohg\npyrneyl fbzr qb.",
    "QVQ LBH XABJ gung va Nynonzn vg vf vyyrtny gb jrne n snxr zbhfgnpur gung pnhfrf\nynhtugre va puhepu?\n\nOhg jul vf vg gung guvf ynj sbeovqqvat n zbhfgnpur pnhfvat ynhtugre va n\npuhepu npghnyyl znxrf zr ynhtu?",
    "QVQ LBH XABJ gung va Jrfg Ivetvavn juvfgyvat haqrejngre vf cebuvovgrq?\n\nSbeghangryl gur jbhyq or pevzvany jbhyq cebonoyl gnxr va nyy gung jngre naq\nqebja, ceriragvat gur nhgubevgvrf sebz punetvat gurz jvgu n pevzr!",
    "QVQ LBH XABJ gung va Znvar nsgre Wnahnel 14gu lbh jvyy or punetrq n svar sbe\nunivat Puevfgznf qrpbengvbaf fgvyy hc?\n\nOhg urer'f n shaal guvat nobhg ybpngvba naq qngrf: ybtvpnyyl fcrnxvat, rira\nvs fbzrguvat vf rnfg vg vf nyfb jrfg naq Wnahnel vf obgu orsber naq nsgre\nQrprzore!",
    "QVQ LBH XABJ gung va Fbhgu Pnebyvan n crefba zhfg or 18 lrnef byq gb cynl\ncvaonyy?\n\nGuvf vf cebonoyl orpnhfr vg hfrq gb or pbafvqrerq tnzoyvat, qrfcvgr gur snpg\ngung nalbar jub'f npghnyyl gevrq vg sbe svir frpbaqf jvyy xabj vg'f n znggre bs\nfxvyy, abg yhpx.",
    "QVQ LBH XABJ gung va Arj Zrkvpb vqvbgf znl abg ibgr?\n\nVqvbg hfrq gb or n yrtny grez ohg gura jub rknpgyl znqr guvf ynj V jbaqre?",
    "QVQ LBH XABJ gung va Zvaarfbgn vg vf vyyrtny gb fyrrc anxrq?\n\nNccneragyl, Zvaarfbgn ynjznxref ner nyfb crrcvat Gbzf!",
    "QVQ LBH XABJ gung va Zvpuvtna hagvy 2002 vg jnf vyyrtny gb fjrne va sebag bs n\njbzna be puvyq?\n\nJung vs n puvyq fjrnef ng n jbzna be n jbzna fjrnef ng n puvyq? Puvyq ng puvyq?\nJbzna ng jbzna?",
    "QVQ LBH XABJ gung va Irezbag jbzra zhfg bognva jevggra crezvffvba sebz gurve\nuhfonaqf gb jrne snyfr grrgu?\n\nOhg vs gurl qba'g naq pna'g purj gurve sbbq ubj jvyy gurl yvir? Gung jbhyq\nzrna gur zna jbhyq or jvsryrff naq gur ynj jbhyq or cbvagyrff. Vfa'g vg\nnyernql?",
    "QVQ LBH XABJ gung va Znffnpuhfrggf ng n jnxr zbhearef znl rng ab zber guna\nguerr fnaqjvpurf?\n\nGunax tbbqarff zbfg crbcyr guvax bs rngvat arkg gb na bcra pbssva!",
    "QVQ LBH XABJ gung va Pbaarpgvphg vg vf vyyrtny gb qvfcbfr bs hfrq enmbe oynqrf?\n\nOhg ubcrshyyl hfrq fpnycryf pna or qvfcbfrq?",
    "QVQ LBH XABJ gung va Unjnvv pbvaf ner abg nyybjrq gb or cynprq va bar’f rnef?\n\nOhg jung V jnag gb xabj vf jung nobhg n gjb'f rnef?",
    "QVQ LBH XABJ gung va Nexnafnf gur Nexnafnf evire pna evfr ab uvture guna gb gur\nZnva Fgerrg oevqtr va Yvggyr Ebpx?\n\nNexnafnf, jurer ynjznxref ner qnsg rabhtu gb gel naq bhgynj sybbqf.",
    "QVQ LBH XABJ gung va Graarffrr vg vf n pevzr gb funer lbhe Argsyvk cnffjbeq?\n\nVafgrnq, vs lbh jnag gb funer lbhe cnffjbeq, punatr vg gb cnffjbeq naq gryy\ngubfr lbh jvfu gb funer vg jvgu gung vg'f bar bs gur zbfg pbzzba naq jbefg\ncnffjbeqf va uvfgbel!",
    "QVQ LBH XABJ gung va Hgnu vg vf vyyrtny gb abg qevax zvyx?\n\nHgnu, jurer gurl qba'g gbyrengr ynpgbfr vagbyrenapr!",
    "QVQ LBH XABJ gung va Nynfxn vg vf pbafvqrerq na bssrapr gb chfu n yvir zbbfr\nbhg bs n zbivat nrebcynar?\n\nOhg pyrneyl vg'f bxnl gb chfu n qrnq zbbfr bhg bs n zbivat nrebcynar naq nal\nzbbfr bhg bs n fgvyy cynar?",
    "QVQ LBH XABJ gung va Znelynaq guvfgyrf znl abg tebj va bar’f lneq?\n\nJung nobhg n tneqra? Sbe va Nzrevpn n lneq vf n tneqra ohg va Ratynaq n\nlneq vf n cvrpr bs haphygvingrq tebhaq naq n tneqra vf na Nzrevpna lneq lrg\nNzrevpn unf tneqraf gbb. Fb jung vf n lneq naq jung vf n tneqra?",
    "QVQ LBH XABJ gung va Fbhgu Qnxbgn ab ubefrf ner nyybjrq vagb Sbhagnva Vaa\nhayrff gurl ner jrnevat cnagf?\n\nV org gurer ner n ybg bs penml guvatf gung unccra va gurer vs fbzr ubefrf\nner nyybjrq ohg abg bguref!",
    "QVQ LBH XABJ gung va Abegu Qnxbgn vg vf vyyrtny gb yvr qbja naq snyy nfyrrc\njvgu lbhe fubrf ba?\n\nOhg jung unccraf gb vazngrf jub snyy nfyrrc jvgu gurve fubrf ba? Qb gurl tb\ngb wnvy va wnvy sbe snyyvat nfyrrc va wnvy jvgu gurve fubrf ba? Naq jung vs\ngurl jrag gb wnvy vavgvnyyl sbe snyyvat nfyrrc jvgu gurve fubrf ba? Qb gurl\nfreir qbhoyr gvzr?",
    "QVQ LBH XABJ gung va Ivetvavn puvyqera ner abg gb tb gevpx-be-gerngvat ba\nUnyybjrra?\n\nJung nobhg nqhygf? V zrna znal fhccbfrq nqhygf va guvf jbeyq pbhyq rnfvyl or\nzvfgnxra sbe puvyqera.",
    "QVQ LBH XABJ gung va Nevmban uhagvat pnzryf vf cebuvovgrq?\n\nUnl, ng yrnfg gur irel srj erznvavat pnzryf ner fnsr!",
    "QVQ LBH XABJ gung va Grknf vg vf vyyrtny gb gnxr zber guna guerr fvcf bs orre\nng n gvzr juvyr fgnaqvat?\n\nVg'f nyfb vyyrtny gb or qehax va n cho va Ratynaq ohg V qba'g guvax gung unf\nfgbccrq znal crbcyr!",
    "QVQ LBH XABJ gung va Pbybenqb vg vf vyyrtny gb evqr n ubefr juvyr haqre gur\nvasyhrapr?\n\nOhg vf vg n ubefr be n ubefr? N ubefr vf na nqhyg znyr ubefr; n fgnyyvba be\ntryqvat. Ohg jung nobhg ubefrf?",
    "QVQ LBH XABJ gung va Aroenfxn vs n puvyq ohecf qhevat puhepu, uvf be ure\ncnerag znl or neerfgrq?\n\nSha snpg: gur grpuavpny grez sbe n ehzoyvat be thetyvat abvfr znqr ol gur\nzbirzrag bs syhvq naq tnf va gur vagrfgvarf vf pnyyrq n obeobeltzhf.",
    "QVQ LBH XABJ gung va Bxynubzn vg vf vyyrtny gb unir gur uvaq yrtf bs snez\nnavznyf va lbhe obbgf?\n\nOhg qba'g jbeel gbb zhpu: vg vf bxnl vs gurve sebag yrtf ner.",
    "QVQ LBH XABJ gung va Jvfpbafva vg vf vyyrtny gb phg n jbzna’f unve?\n\nGuvf znxrf Jvfpbafva gur unvevrfg cynpr gb yvir va gur Havgrq Fgngrf bs\nNzrevpn.",
    "QVQ LBH XABJ gung va Xraghpxl bar znl abg qlr n qhpxyvat oyhr naq bssre vg sbe\nfnyr hayrff zber guna fvk ner sbe fnyr ng bapr?\n\nTerra, checyr, erq, benatr naq nal bgure pbybhe vf bxnl.",
    "QVQ LBH XABJ gung va Ybhvfvnan vg vf vyyrtny gb ebo n onax naq gura fubbg gur\nonax gryyre jvgu n jngre cvfgby?\n\nJuvpu zrnaf ybtvpnyyl fcrnxvat vg'f yrtny gb fubbg gurz jvgu n erny cvfgby.\nYbtvpnyyl vg vf ohg boivbhfyl vg'f abg.",
    "QVQ LBH XABJ gung va Trbetvn lbh pnaabg yvir ba n obng sbe zber guna 30 qnlf\nqhevat gur pnyraqne lrne, rira vs lbh ner whfg cnffvat guebhtu gur fgngr?\n\nOhg jung nobhg 30 qnlf va n lrne jvgubhg n pnyraqne?",
    "QVQ LBH XABJ gung va Zvffvffvccv vg vf vyyrtny gb grnpu bguref jung cbyltnzl\nvf?\n\n'Bu qrnevr zr, V pna bayl unir bar qrne ng n gvzr! Jung rire funyy V qb?!'",
    "QVQ LBH XABJ gung va Sybevqn qbbef bs nyy choyvp ohvyqvatf zhfg bcra bhgjneqf?\n\nAbj vs bhgjneqf ersref gb gbjneqf gur bhgfvqr naq fvaxubyrf ner bhgfvqr qbrf\ngung zrna gung choyvp ohvyqvatf zhfg unir fvaxubyrf nf jryy?",
    "QVQ LBH XABJ gung va Abegu Pnebyvan vg vf ntnvafg gur ynj gb fvat bss xrl?\n\nYbbcubyr: Fvg ba n cvnab juvyfg fvatvat fb gung lbh pna pynvz lbh'er ba xrl\nrira vs lbh'er bss xrl.",
    "QVQ LBH XABJ gung va Arj Wrefrl vg vf ntnvafg gur ynj gb jrne n ohyyrgcebbs\nirfg juvyr pbzzvggvat n zheqre?\n\nQbrf gung zrna vg'f yrtny gb qb fb vs lbh'er abg jrnevat n ohyyrgcebbs irfg?",
    "QVQ LBH XABJ gung va Craaflyinavn vg vf vyyrtny gb fyrrc ba gbc bs n\nersevtrengbe bhgqbbef?\n\nSbeghangryl nalbar gelvat gb qb fb jbhyq cebonoyl ebyy bss vg, znxvat vg\nvzcbffvoyr sbe gur nhgubevgvrf gb punetr gurz jvgu fyrrcvat ba n ersevtrengbe\nbhgqbbef. Bs pbhefr gurl zvtug oernx gurve onpx be arpx ohg gurl jba'g or\npunetrq jvgu fyrrcvat ba n ersevtrengbe!",
    "QVQ LBH XABJ gung va Buvb vg vf vyyrtny sbe zber guna svir jbzra gb yvir va n\nubhfr?\n\nVf guvf orpnhfr gurl'er nsenvq bs cebterff? V zrna vg'f hayvxryl gb unccra\njvgu svir zra yvivat va n ubhfr hayrff cebterff vf nethvat!",
    "QVQ LBH XABJ gung va Arj Lbex gur cranygl sbe whzcvat bss n ohvyqvat vf qrngu?\n\nArj Lbex, jurer fbzr crbcyr unir gjb yvirf. Whfg jnvg hagvy Arj Lbexref urne\nnobhg pngf!",
    "QVQ LBH XABJ gung va Eubqr Vfynaq evqvat n ubefr bire nal choyvp uvtujnl sbe\ngur checbfr bs enpvat be grfgvat gur fcrrq bs gur ubefr vf vyyrtny?\n\nJung nobhg qevivat n pne arkg gb n ubefr sbe gur fnzr checbfr?",
    "QVQ LBH XABJ gung va Arj Unzcfuver lbh znl abg gnc lbhe srrg, abq lbhe urnq, be\nva nal jnl xrrc gvzr gb gur zhfvp va n gnirea, erfgnhenag be pnsé?\n\nArj Unzcfuver, jurer zhfvpvnaf cynl bssorng!",
    "QVQ LBH XABJ gung va Vaqvnan bar nezrq cvnavfgf zhfg cresbez sbe serr?\n\nVs gur ynjznxref jrer ybtvpny, gurl jbhyq unir znqr vg unys cevpr sbe unys n\ncresbezre. Ohg ernyvfgvpnyyl gurl fubhyq or noyr gb punetr gjvpr nf zhpu gb\npbzcrafngr gurve zvffvat (bgure?) unys.",
    "QVQ LBH XABJ gung va Zvffbhev fvatyr zra orgjrra gur ntrf bs 21 naq 50 zhfg cnl\nna naahny gnk bs bar qbyyne?\n\nVs lbh guvax orvat nybar vf onq, vg'f rira jbefr va Zvffbhev jurer vs lbh'er\nn zna orgjrra gur ntrf bs 21 naq 50 lbh ner gnkrq! Gung'f tbg gb or n\npbasvqrapr obbfgre!",
    "QVQ LBH XABJ gung va Vyyvabvf lbh znl or neerfgrq sbe intenapl vs lbh qb abg\nunir ng yrnfg bar qbyyne ba lbhe crefba?\n\nVa bgure jbeqf, va Vyyvabvf vs lbh'er n ohz lbh unir gb unir ng yrnfg n\nqbyyne ng nyy gvzrf. Vs lbh'er n ohz va gur erfg bs gur Havgrq Fgngrf bs\nNzrevpn lbh qba'g. Vs ubjrire lbh'er n ohz va gur HX - jryy vg zvtug or rira\njbefr!",
    "QVQ LBH XABJ gung va Vbjn zbhfgnpurf ner vyyrtny vs gur ornere unf n graqrapl\ngb unovghnyyl xvff bgure uhznaf?\n\nNf bccbfrq gb abg graqvat gb unovghnyyl xvff bguref?",
    "QVQ LBH XABJ gung va Qrynjner vg vf vyyrtny gb syl bire nal obql bs jngre,\nhayrff bar vf pneelvat fhssvpvrag fhccyvrf bs sbbq naq qevax?\n\nJung qbrf guvf zrna sbe gubfr jvgu naberkvn areibfn? Jung nobhg ohyvzvn\nareibfn be nabgure rngvat qvfbeqre? Jung vs lbh'er ba n fgevpg qvrg be vs sbe\nfbzr fgenatr ernfba lbh'er sylvat gb unir n pbybabfpbcl?",
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
vrergfB(int i, int r)
{
    struct tm *tm = NULL;	/* there is NULL time like this time */
    int ret;			/* libc return value */
    unsigned uret;		/* unsigned libc return value */
    size_t ic = 0;		/* What is IC? First ask yourself what OOC is! */

    /*
     */
    uintmax_t max = 42;

    /*
     */
    if (i < 0 || r < 0) {

	/*
	 * time and time again we need to reconsider the argument
	 */
	t = time(NULL);
	if (t == (time_t)-1)
	    t = (time_t)02256330241;
	tm = localtime(&t);
	if (tm == NULL) {
	    if (i < 0)
		i = 0;
	    if (r < 0)
		r = 2*3*0521;
	} else {
	    if (i < 0)
		i = tm->tm_yday;
	    if (r < 0)
		r = tm->tm_year + 2*2*5*5*0x13;
	}
    }

    /*
     */
    if ((max==0?42:max != 0)) /* max should always be > 0 but we check due to division below */
	for (max /= 2 + max; oebxergfB[max] != NULL; ++max) /* max is half itself due to table compression */
	    ;	/* the bottom bee is Eric the semi-bee */

    /*
     * "You are expected to understand this but we're not helping if you don't :-)
     * ...but whatever you do not don't don't panic!" :-(
     */
    for (char const *p = oebxergfB[((r*2*2*015 + (int)(i / (07&0x07))) % (max))]; *p; ++p)
    {
	errno = 0;	/* we didn't think much about what was previously stored in errno */

	/*
	 */
	if (*p == '\\' && p[1] == 'n') {
	    ret = fputc(012, stdout);
	    if (ret != 0x0a) {
		fwarnp(stderr, "abcdefg...", "\nthe line must not have been new\n");
	    }
	    ++p; /* be positive and look forward to the next one! */

	/*
	 */
	} else if (!isalpha(*p)) {
	    ret = putchar(*p);
	    if (ret == EOF) {
		fwarnp(stderr, "abcdefg..", "\nthat was not in character\n");
	    } else {
		++ic;
	    }

	/*
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
		fwarnp(stderr, "abcdefg..", "\nthat character was absolutely mixed with sin!\n");
	    } else {
		++ic;
	    }
	}
    }

    /*
     */
    errno = 0;	/* be positive: pretend we have 0 errors so far */
    ret = fputc(0x0a, stdout);
    if (ret != 0x0a) {
	fwarnp(stderr, "abcdefg ...", "\nmeet the new line, same as the old line\n");
    }

    dbg(DBG_LOW, "FUN FACT: there %s %ju in character character%s.", ic != 1 ? "were":"was", (uintmax_t) ic, ic != 1 ? "s":"");

    /*
     * and in the end ... take a moment to bow before exiting stage left
     */
#if 0	/* you might argue if we need to restate our argument here and now */
    printf("i: %d r: %d\n", i, r);
#endif
    errno -= errno;	/* we choose to retract the last error */
    uret = sleep(1+(((i+r)>0?(i+r):(-r-i))%5));
    if (uret != 0+0) {
	fwarnp(stderr, "ABCDEFG...", "\npossible insomnia detected\n");
    }
    (void) exit(1+(((i-r)>0?(i-r):(-i+r)) % 254)); /*ooo*/
    not_reached();
    return;
}
