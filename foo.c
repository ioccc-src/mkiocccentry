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
    "'twll' ul twzuespayknueyttf pawybwa bnym 'pawybwa'. '<', nsxwvwa,\n"
    "ul twzuespayknueyttf twll bnym '>'. Usiwsmw leawxwd qk.\n"
    "-- Xwzft",

    "C dsm'b hmsx nyto so fsq nyto yl xwtt yl C lnsqtd tuhw;\n"
    "ymd C tuhw twll bnym"
    " nyto so fsq nyto yl xwtt yl fsq dwlwavw.\n"
    "-- Nutrs Nyppuml",

    "I lwebusm: '{}'\n"
    "-- Xwzft\n",

    "'Uwesmdtf, bs ewtwraybw DSM ruabndyfl: iumw ymd if nsmsqayrtw ymd pyttymb\n"
    "oybnwa'l.' Smesiosabyrtw ymd ykkawnwmluvw lutwmew. 'C yi smtf nyto bnw iym\n"
    "bnyb nw ul: C yi 72, nw ul 144. Zsqa mqirwal yaw enslwm bs ds nsmsqa bs\n"
    "wyen so nul nsmsqayrtw fwyal.' Tnul xyl awyttf dawydoqt -- y awpqtya\n"
    "rayumbxulbwa, ymd lsiw so bnwi owtb umlqtbwd, tuhw twyk-dyfl lnsvwd\n"
    "um bs outt qk y eytwmdya.\n"
    "-- Numps Nyppuml",

    "I qmusml: tsvump esqktwl xns wmgsf kaspayiiump um I bspwbnwa.\n"
    "-- Xwzft\n",

    "'Yssh,' lyud Gabnqa, 'xsqtd ub lyvw fsq y tsb so buiw uo C gqlb pyvw\n"
    "qk ymd xwmb iyd msx?'\n",


    "C dsm'b nyvw y lnydsx; C iqadwawd nui fwyal yps rweyqlw nw xsqtdm'b lbsk\n"
    "osttsxump iw.\n"
     "  -- Xwzft",

    "Fsrsdf ul kwaoweb. C yi msrsdf. Tnwawosaw C yi kwaoweb ymd fsq'aw msb! :)\n"
    "   -- Xwzft\n",

    "Nwmwoub so Ituiybw Inympw?\n"
    "\n"
    "Gl bnw xsatd pwbl dauwa ymd nsbbwa iymf kwsktw yaw qmdwalbymdyrtf yoayud so bnw\n"
    "esmlwcqwmewl osa bnwua tuvwtunssd ymd bnw tuvwtunssd so bnwua enutdawm ymd\n"
    "paymdenutdawm.\n"
    "\n"
    "Cb'l rwwm hmsxm osa ywsml bnyb xw ytt mwwd xybwa bs lqavuvw rqb xubn etuiybw\n"
    "enympw bnwaw ul twll xybwa dqw bs tsmpwa, isaw oawcqwmb dasqpnbl. Iaskl yaw yb\n"
    "aulh twydump bs oqabnwa oyiumw ymd dulwylw.\n"
    "\n"
    "Gmd yl bnwaw yaw lbutt lsiw ytuvw xns yaw std wmsqpn bs awiwirwa bnw Wawyb\n"
    "Hyiumw (isaw esiismtf hmsxm sqbludw so Cawtymd yl bnw Cauln Esbybs Hyiumw)\n"
    "xnuen twd bs ykkaszuiybwtf smw iuttusm dwybnl ymd yb twylb y iuttusm wvwmbqyttf\n"
    "twyvump Cawtymd bnwaw yaw lsiw bnyb yaw byhump bnul wlkweuyttf lwausql (bnw\n"
    "yqbnsa'l iybw Ryvw Pqumm ul kyabueqtyatf leyawd, lyfump 'C awiwirwa xnwm if\n"
    "paymddyd bstd iw xyf ryeh um 1847 bnyb uo dulwylw dud msb hutt nui oualb nw\n"
    "xsqtd ps ryaif uo nw esqtd msb lssm iyhw lsiw pssd std oylnusmwd esddtw'). Lymf\n"
    "kyabl so bnw xsatd ytawydf ykkaweuybw bnul bnsqpn bnwf ms dsqrb xutt rw\n"
    "esmoqlwd rf bnw xsad 'esddtw'.\n"
    "\n"
    "Vsatdxudw bnw aulump bwikwaybqawl yaw eyqlump dyiypw bs umoaylbaqebqawl, iyhump\n"
    "xutdouawl isaw oawcqwmb ymd isaw dympwasql ymd ub'l iwtbump uew yb\n"
    "awesad-rawyhump aybwl. Tnul iwtbump uew um bqam ul umeawylump bnw lwyxybwa\n"
    "twvwt xnuen lwausqltf wmdympwal esylbyt bsxml ymd iyhwl ub nyadwa osa lsiw\n"
    "ymuiytl tuhw kstya rwyal bs wvwm lqavuvw.\n"
    "\n"
    "Nqb msx lsiw leuwmbulbl ymd yebuvulbl yaw nupntupnbump y kslubuvw pyum yrsqb\n"
    "etuiybw enympw. Gl bnw ktymwb pwbl nsbbwa ymd dauwa, bnw bszue pyl dunfdaspwm\n"
    "ismszudw ul twll kawvytwmb.\n"
    "\n"
    "Hsa bnw qmumosaiwd: wvwaf lumptw otssd nyl y nupn yisqmb so RKLD; xubnsqb RKLD\n"
    "etuiybw yoowebump yeud ayum xsqtd msb rw y kasrtwi; ub'l rwwm qlwd rf\n"
    "psvwamiwmbl um bsabqaw ymd iqadwa; ub'l qlwd um mqetwya xwyksml; ymd wvwm gqlb\n"
    "rawybnump ub um eym wylutf hutt fsq. Cm oyeb wvwaf kwalsm wzkslwd bs ub\n"
    "wvwmbqyttf duwl!\n"
    "\n"
    "Nqb dwlkubw bnwlw oyebl ms psvwamiwmb nyl lqeewwdwd um rymmump bnul nupntf\n"
    "twbnyt pyl. Qwaf owx nyvw wvwm bythwd yrsqb ub. Isqtd ub rw bnyb bnw umwkbubqdw\n"
    "so iymhumd xubn etuiybw enympw iupnb rw pssd? Tnul ul fwb bs rw ymlxwawd rqb ub\n"
    "nyl twob bnw etuiybw lewkbuel lnyhump bnwua nwyd ylhump 'Kqn?'\n",

    "Tnwaw ul ym yab, sa, aybnwa, y hmyeh bs otfump. Tnw hmyeh tuwl um\n"
    "twyamump nsx bs bnasx fsqalwto yb bnw pasqmd ymd iull.\n",

    "HSF HGIT: Cm N ymd wyatf I, bnw += skwaybsa xyl lkwtb =+ umlbwyd so += ;\n"
    "bnul iulbyhw, awkyuawd um 1976, xyl umdqewd rf y lwdqebuvwtf wylf xyf so\n"
    "nymdtump bnw oualb osai um N'l twzueyt ymytflwa.\n"
    "\n"
    "Uww isaw oqm oyebl ymd"
    " nulbsaf rf Rwmmul Mubenuw nuilwto yb\n"
    "nbbkl://xxx.rwtt-tyrl.esi/qla/dia/xxx/enulb.nbit.\n",

    "'Vnyb ul ub?' pasxtwd Uyi, iulumbwakawbump bnw lupml. 'Vnyb'l bnw mwwd bs\n"
    "lmuoo? Tnw lbumh mwyatf hmsehl iw dsxm xubn if mslw nwtd. Zsq lbumh, ymd iylbwa\n"
    "lbumhl; bnw xnstw ktyew lbumhl.'\n"
    "\n"
    "'Zwl, fwl, ymd Uyi lbumhl!' ymlxwawd Wsttqi. "
    "'Essa Uiéypst liwttl ub,\n"
    "rqb pssd Uiéypst rwyal ub. Kwtkl muew iylbwa. Nqb bnyb'l"
    " ms iybbwa. Tnw yua'l\n"
    "isvump, enympw ul esiump. Uiéypst xsmdwal; nw'l msb nykkf.'\n",


    "Uflbwi waasa: Rwbwebwd qlwa yebuvubf.\n"
    "   -- Xwzft",

    "'Fyf! Fsb Otvwl,' lyud bnw osqabn, bnw byttwlb, ymd yl ub ykkwyawd bnw\n"
    "enuwo yismp"
    " bnwi. 'Otvwl ds msb xyth um Cbnutuwm um bnwlw dyfl. Gmd\n"
    "Otvwl yaw xsmdasql oyua bs"
    " tssh qksm, sa ls 'bul lyud.'\n"
    "\n"
    "'Lwymump xw'aw msb, C byhw fsq,' lyud Uyi. 'Tnymh"
    " fsq humdtf. Gmd xnwm\n"
    "fsq'vw oumulnwd duleqllump ql, kwanykl fsq'tt lyf xns fsq yaw,"
    " ymd\n"
    "xnf fsq eym'b twb bxs buawd bayvwttwal awlb.'\n",

    "Vnf lnsqtd fsq mwvwa xwupn fsqalwto xwyaump lutvwa, pstd sa ktybumqi? Nweyqlw\n"
    "bnwf'aw nwyvf iwbytl.\n\n -- Xwzft",

    "Wutdsa xyl lutwmb osa y isiwmb. 'C ds msb tuhw bnul mwxl,' nw lyud\n"
    "yb tylb. 'Tnyb Wymdyto lnsqtd rw tybw, dswl msb rsdw xwtt. Nqb ub\n"
    "ul lyud:"
    " Rs msb iwddtw um bnw yooyual so Vujyadl, osa bnwf yaw\n"
    "lqrbtw ymd cqueh bs "
    "ympwa. Tnw ensuew ul fsqal: bs ps sa xyub.'\n"
    "\n"
    "'Gmd ub ul ytls lyud,' ymlxwawd"
    " Hasds: 'Ws msb bs bnw Otvwl osa\n"
    "esqmlwt, osa bnwf xutt lyf rsbn ms ymd fwl.'",

    "Dmw so bnw bnumpl Hsad Eawoweb nyd ytxyfl osqmd nyadwlb bs qmdwalbymd yrsqb\n"
    "nqiyml xyl bnwua nyrub so esmbumqyttf lbybump ymd awkwybump bnw vwaf vwaf\n"
    "srvusql.\n"
    "\n"
    "Dmw so bnw bnumpl Hsad Eawoweb nyd ytxyfl osqmd nyadwlb bs qmdwalbymd yrsqb\n"
    "nqiyml xyl bnwua nyrub so esmbumqyttf lbybump ymd awkwybump bnw vwaf vwaf\n"
    "srvusql.",

    "Hsa y isiwmb, msbnump nykkwmwd. Tnwm, yobwa y lwesmd sa ls, msbnump\n"
    "esmbumqwd bs nykkwm.",

    "'Co C wvwa iwwb iflwto,' lyud Jyknsd, 'C'tt nub iflwto ls nyad C xsm'b hmsx\n"
    "xnyb'l nub iw.'",

    "'C lww msrsdf sm bnw asyd,' lyud Gtuew.\n"
    "\n"
    "'C smtf xuln bnyb C nyd lqen wfwl,' bnw Bump awiyahwd"
    " um y oawboqt\n"
    "bsmw. 'Ts rw yrtw bs lww Fsrsdf! Gmd yb bnyb dulbymew bss! Vnf, ub'l\n"
    "yl iqen yl C"
    " eym ds bs lww awyt kwsktw, rf bnul tupnb!'\n"
    "\n"
    "[...]\n"
    "\n"
    "'Vns dud fsq kyll sm bnw asyd?' bnw Bump xwmb"
    " sm, nstdump sqb nul\n"
    "nymd bs bnw Lwllwmpwa osa lsiw isaw nyf.\n"
    "\n"
    "'Fsrsdf,' lyud bnw Lwllwmpwa.\n"
    "\n"
    "'"
    "Pqubw aupnb,' lyud bnw Bump: 'bnul fsqmp tydf lyx nui bss. Us so esqalw Fsrsdf\n"
    "xythl ltsxwa bnym fsq.'\n"
    "\n"
    ""
    "'C ds if rwlb,' bnw Lwllwmpwa lyud um y lqthf bsmw. 'C'i lqaw msrsdf xythl iqen\n"
    "oylbwa bnym C ds!'\n"
    "\n"
    ""
    "'Kw eym'b ds bnyb,' lyud bnw Bump, 'sa wtlw nw'd nyvw rwwm nwaw oualb.'",

    "'Tnul ul y enutd!' Kyupny awktuwd wypwatf, esiump um oasmb so Gtuew bs\n"
    "umbasdqew nwa, ymd lkawydump sqb rsbn nul nymdl bsxyadl nwa um ym Gmpts-Uyzsm\n"
    "ybbubqdw. 'Vw smtf osqmd ub bs-dyf. Cb'l yl tyapw yl tuow, ymd bxuew yl\n"
    "mybqayt!'\n"
    "\n"
    ""
    "'C ytxyfl bnsqpnb bnwf xwaw oyrqtsql ismlbwal!' lyud bnw Smuesam. 'Cl ub\n"
    "ytuvw?'\n"
    "\n"
    "'"
    "Cb eym byth,' lyud Kyupny, lstwimtf.\n"
    "Tnw Smuesam tsshwd dawyiutf yb Gtuew, ymd lyud 'Tyth, enutd.'\n"
    "Gtuew esqtd msb nwtk nwa tukl eqatump qk umbs y liutw yl lnw rwpym: 'Rs fsq\n"
    "hmsx, C ytxyfl bnsqpnb"
    " Smuesaml xwaw oyrqtsql ismlbwal, bss! C mwvwa lyx smw\n"
    "ytuvw rwosaw!'\n"
    "\n"
    "[...]\n"
    "\n"
    "'Gn, xnyb ul ub, msx?'"
    " bnw Smuesam eauwd wypwatf. 'Zsq'tt mwvwa pqwll! C\n"
    "esqtdm'b.' Tnw Yusm tsshwd yb Gtuew xwyautf. 'Gaw fsq"
    " ym ymuiyt -- vwpwbyrtw --\n"
    "sa iumwayt?' nw lyud, fyxmump yb wvwaf sbnwa xsad.\n"
    "\n"
    "'Cb'l y oyrqtsql ismlbwa!'"
    " bnw Smuesam eauwd sqb, rwosaw Gtuew esqtd awktf.\n"
    "\n"
    "'Tnwm nymd asqmd bnw ktqi-eyhw, Lsmlbwa,' bnw Yusm lyud.",

    "C rwpym bs nyvw dsqrbl yrsqb nsx iybn ul byqpnb um bnul esqmbaf xnwm y fsqmp\n"
    "essh yb if kujjwauy lyud nw nyd y kasrtwi. 'G eqlbsiwa eyttwd, ylhump osa y\n"
    "kuw bnyb'l y"
    " bnuad enwwlw, y bnuad kwkkwasmu, ymd y bnuad esirumybusm,' nw\n"
    "lyud. 'Us, xnyb'l bnw kasrtwi?' "
    "C ylhwd nui. 'C dsm'b hmsx xnyb bs kqb sm\n"
    "bnw osqabn bnuad'.\n"
    "\n"
    "-MDNOMT BOGMFOZ\n"
    "Mymens Isadsvy,\n"
    "Iytuosamuy",

    "Vnf xyl bnw kyabuyttf dwyo dfltwzue iym qklwb yb nul xuow? Unw ylhwd nui uo nw\n"
    "psb bnw asttwd sybl ymd nw bnsqpnb lnw xyl eyttump nui ym std syo.\n"
    "\n-- Xwzft",

    "'Gmd fsq ds Gddubusm?' bnw Vnubw Pqwwm ylhwd. 'Vnyb'l smw ymd smw ymd smw ymd\n"
    "smw ymd smw ymd smw"
    " ymd smw ymd smw ymd smw ymd smw?' 'C dsm'b hmsx,' lyud\n"
    "Gtuew. 'C tslb esqmb.'\n"
    "\n"
    "'Unw eym'b ds Gddubusm,'"
    " bnw Mwd Pqwwm umbwaaqkbwd. 'Iym fsq ds Uqrbayebusm?'\n"
    "Tyhw mumw oasi wupnb.' 'Fumw oasi wupnb C eym'b, fsq hmsx,'"
    " Gtuew awktuwd vwaf\n"
    "awydutf: 'rqb--'\n"
    "\n"
    "'Unw eym'b ds Uqrbayebusm,' lyud bnw Vnubw Pqwwm. 'Iym fsq ds Ruvulusm? "
    "Ruvudw\n"
    "y tsyo rf y hmuow -- xnyb'l bnw ymlxwa bs bnyb?' 'C lqkkslw--' Gtuew xyl\n"
    "rwpummump, rqb bnw Mwd Pqwwm "
    "ymlxwawd osa nwa.  'Nawyd-ymd-rqbbwa, so esqalw.\n"
    "Taf ymsbnwa Uqrbayebusm lqi. Tyhw y rsmw oasi y dsp: xnyb awiyuml?'\n"
    "\n"
    ""
    "Gtuew esmludwawd. 'Tnw rsmw xsqtdm'b awiyum, so esqalw, uo C bssh ub--ymd bnw\n"
    "dsp xsqtdm'b awiyum; ub xsqtd esiw bs rubw "
    "iw--ymd C'i lqaw C lnsqtdm'b\n"
    "awiyum!' 'Tnwm fsq bnumh msbnump xsqtd awiyum?' lyud bnw Mwd Pqwwm.\n"
    "\n"
    "'C bnumh bnyb'l bnw "
    "ymlxwa.' 'Vasmp, yl qlqyt,' lyud bnw Mwd Pqwwm: 'bnw dsp'l\n"
    "bwikwa xsqtd awiyum.' 'Nqb C dsm'b lww nsx --'\n"
    "\n"
    ""
    "'Vnf, tssh nwaw!' bnw Mwd Pqwwm eauwd. 'Tnw dsp xsqtd tslw ubl bwikwa, xsqtdm'b\n"
    "ub?'\n"
    "\n"
    ""
    "'Ewanykl ub xsqtd,' Gtuew awktuwd eyqbusqltf. 'Tnwm uo bnw dsp xwmb yxyf, ubl\n"
    "bwikwa xsqtd awiyum!' "
    "bnw Pqwwm wzetyuiwd bauqiknymbtf.",

    "Kwaw bnw Mwd Pqwwm rwpym ypyum. 'Iym fsq ymlxwa qlwoqt cqwlbusml?' lnw lyud.\n"
    "'Ksx ul rawyd iydw?'\n"
    "\n"
    "'C hmsx bnyb!' Gtuew eauwd wypwatf.' Zsq byhw lsiw otsqa--'\n"
    "'Vnwaw ds fsq kueh bnw otsxwa?' bnw Vnubw Pqwwm ylhwd. 'Cm y pyadwm, sa um bnw\n"
    "nwdpwl?'\n"
    "\n"
    ""
    "'Vwtt ub ulm'b kuehwd yb ytt,' Gtuew wzktyumwd: 'ub'l pasqmd--'\n"
    "'Ksx iymf yeawl so pasqmd?' "
    "lyud bnw Vnubw Pqwwm. 'Zsq iqlbm'b twyvw sqb ls\n"
    "iymf bnumpl.'\n"
    "\n"
    "'Hym nwa nwyd!' bnw Mwd Pqwwm "
    "ymzusqltf umbwaaqkbwd. 'Unw'tt rw owvwauln yobwa\n"
    "ls iqen bnumhump.' Us bnwf lwb bs xsah ymd oymmwd "
    "nwa xubn rqmenwl so twyvwl,\n"
    "butt lnw nyd bs rwp bnwi bs twyvw soo, ub rtwx nwa nyua yrsqb ls.",

    "HSF HGIT:\n"
    "\n"
    "Cm Yuow, bnw Smuvwalw ymd Ovwafbnump, bnw bnuad umlbytiwmb so Tnw Kubennuhwa'l\n"
    "Wqudw bs bnw Wytyzf rf Rsqptyl Gdyil, ub'l lyud bnw ymlxwa bs Yuow, bnw\n"
    "Smuvwalw ymd Ovwafbnump ul 42. "
    "Rsqptyl Gdyil umlulbwd bnul xyl y gshw, xaubump\n"
    "sm SUOFOT:\n"
    "\n"
    "    Tnw ymlxwa bs bnul ul vwaf luiktw. Cb "
    "xyl y gshw. Cb nyd bs rw y mqirwa, ym\n"
    "    sadumyaf, liyttuln mqirwa, ymd C enslw bnyb smw. Numyaf awkawlwmbybusml,"
    "\n"
    ""
    "    rylw bnuabwwm, Turwbym ismhl yaw ytt esiktwbw msmlwmlw. C lyb yb if dwlh,\n"
    "    lbyawd umbs bnw pyadwm ymd bnsqpnb"
    " '42 xutt ds' C bfkwd ub sqb. Omd so\n"
    "    lbsaf.\n"
    "\n"
    "Nqb dud fsq hmsx bnyb lsiw ylbasmsiwal UEOFT TVD ZOGMU lbqdfump bnul "
    "ymd CFUCUT\n"
    "bnyb 42 ul yebqyttf esaaweb? Hsa bnul lbsaf lww:\n"
    "\n"
    ""
    "nbbkl://xxx.umdwkwmdwmb.es.qh/mwxl/fwl-bnw-ymlxwa-bs-bnw-qmuvwalw-awyttf-ul-42-1351201.nbit",

    "Cb'l esiismtf lqppwlbwd bnyb I++ ul ym uikasvwiwmb svwa I. Tnul ul y ifbn\n"
    "dwauvump oasi bnw oyeb bnyb bnw skwaybsa ++ um I umeawylwl ubl skwaymd yl y ludw\n"
    "wooweb: bnw vytqw so bnw skwaybusm dwkwmdump sm uo ub'l kslbouz sa kawouz ++. Gl\n"
    "ub nykkwml I++ ul kslbouz ++. Tnul iwyml bnw vytqw so bnw wzkawllusm ul I ymd yl\n"
    "y ludw wooweb I ul umeawiwmbwd rf 1. Tnul kasvwl bnyb I ul lqkwausa - xnyb'l\n"
    "twob, I + 1, ul rtsyb ymd ub'l qptf. Cb'l nqpw ymd umdwewmb. Hsa I++'l cqytubf\n"
    "baf I = 0;.\n"
    "    -- Xwzft",

    "Kyvw fsq bauwd osaeump ym qmwzkwebwd awrssb?\n"
    "\n"
    ""
    "Uww, bnw dauvwa nsshl bnw oqmebusm rf kybenump bnw lflbwi eytt byrtw, ls ub'l\n"
    "msb lyow bs qmtsyd ub qmtwll ymsbnwa bnawyd'l yrsqb bs gqik um ymd ds ubl\n"
    "lbqoo, ymd fsq dsm'b xymb bs wmd qk Cm bnw iuddtw so umvytud iwisaf.\n"
    "\n"
    ""
    "    kqln DzH9RGHHND\n"
    "    eytt yauwl+DzO90(H9R8HO90)\n"
    "    isv wdz, [wez+1]\n"
    "    isv yz, dl:BwUwavuewRwleaukbsaTyrtw\n"
    "    isv dxsad_110ID, wdz\n"
    "    isv wyz, dl:JxPqwafRuawebsafHutw\n"
    "\n"
    "Kwtts?\n"
    "\n"
    "-- Lyqauew Lsll, Tnw CT "
    "Iasxd Uwauwl 1 Okulsdw 1, 'Zwlbwadyf'l Ayi'.",

    "RCR ZDS BFDV bnyb um Vylnumpbsm xnwm bxs bayuml esiw bs y easllump, mwubnwa eym\n"
    "ps qmbut bnw sbnwa nyl kyllwd?\n"
    "\n"
    "Cm xnuen eylw, rsbn tubwayttf ymd tspueyttf lkwyhump, mwubnwa bayum eym wvwa\n"
    "ps. Tnul um bqam iwyml bnyb bxs isaw bayuml xsqtd esiw, qmyrtw bs kasewwd ymd\n"
    "bnwm bxs isaw ymd wvwmbqyttf bnw ayutxyf xsqtd rw rtsehwd, kawvwmbump eyal oasi\n"
    "easllump yl xwtt, iyhump y nqpw bayooue gyi!",

    "RCR ZDS BFDV bnyb um Dawpsm y dssa sm y eya iyf msb rw twob skwm tsmpwa bnym\n"
    "ul mwewllyaf?\n"
    "\n"
    "Humyttf xw nyvw ym wzktymybusm so xnf bnwaw yaw ls iymf eya dssal tfump sm bnw\n"
    "lbawwbl um Dawpsm!",

    "RCR ZDS BFDV bnyb um Iytuosamuy ymuiytl yaw rymmwd oasi iybump kqrtuetf xubnum\n"
    "1500 owwb so y byvwam, lensst, sa ktyew so xsalnuk?\n"
    "\n"
    "Nqb yb twylb smew bnwf'aw gyutwd, srvusqltf um y jss, bnwf eym ds ub yl\n"
    "sobwm yl bnwf tuhw!",

    "RCR ZDS BFDV bnyb um Fwvydy ub ul uttwpyt bs dauvw y eyiwt sm bnw nupnxyf?\n"
    "\n"
    "Vnwbnwa bnul iwym puvw y eyiwt y tuob sm bnw nupnxyf sa yebqyttf qlump bnw\n"
    "eyiwt yl y vwnuetw ul qmetwya, gqlb tuhw bnw kqakslw so bnul tyx ublwto\n"
    "wlkweuyttf lumew bnwaw iupnb msb rw iymf eyiwtl twob um Fwvydy ymfxyf.",

    "RCR ZDS BFDV bnyb um Cdyns ub ul uttwpyt osa y iym bs puvw nul lxwwbnwyab y rsz\n"
    "so eymdf xwupnump twll bnym 50 ksqmdl (22.6796hp)?\n"
    "\n"
    "C rwb eymdf lnskl tsvw bnul tyx!",

    "RCR ZDS BFDV bnyb um Lsmbymy ub ul uttwpyt bs nyvw y lnwwk um bnw eyr so fsqa\n"
    "baqeh xubnsqb y enykwasmw?\n"
    "\n"
    "Gkkyawmbtf um Lsmbymy lnwwk yaw nqiyml! Da wtlw ... tyxfwal dsm'b hmsx xnyb\n"
    "enykwasmw iwyml.",

    "Co ym wyabncqyhw rawyhl y iuaasa dswl bnw wmbuaw wyabn nyvw lwvwm fwyal so ryd\n"
    "tqeh ?\n -- Xwzft",

    "RCR ZDS BFDV bnyb um Vfsiump ub ul uttwpyt bs xwya y nyb bnyb srlbaqebl\n"
    "kwsktw'l vuwx um y kqrtue bnwybaw sa ktyew so yiqlwiwmb?\n"
    "\n"
    "...C bnumh bnul yebqyttf lsqmdl awylsmyrtw rqb C'i ytls y dxyao. Tnwm ypyum,\n"
    "xnwm tfump dsxm xw'aw ytt dxyavwl!",

    "RCR ZDS BFDV bnyb bnw Bymlyl lbybw pyiump rsyad kasnurubl bnw qlw so iqtwl bs\n"
    "nqmb dqehl?\n"
    "\n"
    "C'i msb lqaw iymf kwsktw bnumh iqtwl eym ksllurtf nwtk nqmbump dqehl rqb\n"
    "etwyatf lsiw ds.",

    "RCR ZDS BFDV bnyb um Gtyryiy ub ul uttwpyt bs xwya y oyhw isqlbyenw bnyb eyqlwl\n"
    "tyqpnbwa um enqaen?\n"
    "\n"
    "Nqb xnf ul ub bnyb bnul tyx osaruddump y isqlbyenw eyqlump tyqpnbwa um y\n"
    "enqaen yebqyttf iyhwl iw tyqpn?",

    "RCR ZDS BFDV bnyb um Vwlb Quapumuy xnulbtump qmdwaxybwa ul kasnurubwd?\n"
    "\n"
    "Hsabqmybwtf bnw xsqtd rw eauiumyt xsqtd kasryrtf byhw um ytt bnyb xybwa ymd\n"
    "dasxm, kawvwmbump bnw yqbnsaubuwl oasi enyapump bnwi xubn y eauiw!",

    "RCR ZDS BFDV bnyb um Lyumw yobwa Aymqyaf 14bn fsq xutt rw enyapwd y oumw osa\n"
    "nyvump Inaulbiyl dwesaybusml lbutt qk?\n"
    "\n"
    "Nqb nwaw'l y oqmmf bnump yrsqb tseybusm ymd dybwl: tspueyttf lkwyhump, wvwm\n"
    "uo lsiwbnump ul wylb ub ul ytls xwlb ymd Aymqyaf ul rsbn rwosaw ymd yobwa\n"
    "Rwewirwa!",

    "RCR ZDS BFDV bnyb um Usqbn Iyastumy y kwalsm iqlb rw 18 fwyal std bs ktyf\n"
    "kumrytt?\n"
    "\n"
    "Tnul ul kasryrtf rweyqlw ub qlwd bs rw esmludwawd pyirtump, dwlkubw bnw oyeb\n"
    "bnyb ymfsmw xns'l yebqyttf bauwd ub osa ouvw lwesmdl xutt hmsx ub'l y iybbwa so\n"
    "lhutt, msb tqeh.",

    "RCR ZDS BFDV bnyb um Fwx Lwzues udusbl iyf msb vsbw?\n"
    "\n"
    "Cdusb qlwd bs rw y twpyt bwai rqb bnwm xns wzyebtf iydw bnul tyx C xsmdwa?",

    "RCR ZDS BFDV bnyb um Lummwlsby ub ul uttwpyt bs ltwwk myhwd?\n"
    "\n"
    "Gkkyawmbtf, Lummwlsby tyxiyhwal yaw ytls kwwkump Tsil!",

    "RCR ZDS BFDV bnyb um Luenupym qmbut 2002 ub xyl uttwpyt bs lxwya um oasmb so y\n"
    "xsiym sa enutd?\n"
    "\n"
    "Vnyb uo y enutd lxwyal yb y xsiym sa y xsiym lxwyal yb y enutd? Inutd yb enutd?\n"
    "Vsiym yb xsiym?",

    "Vnyb ds fsq eytt y oulnwaiym lwttump nul ouln? Uwtouln.\n"
    "-- Xwzft",

    "RCR ZDS BFDV bnyb um Qwaismb xsiwm iqlb srbyum xaubbwm kwaiullusm oasi bnwua\n"
    "nqlrymdl bs xwya oytlw bwwbn?\n"
    "\n"
    "Nqb uo bnwf dsm'b ymd eym'b enwx bnwua ossd nsx xutt bnwf tuvw? Tnyb xsqtd\n"
    "iwym bnw iym xsqtd rw xuowtwll ymd bnw tyx xsqtd rw ksumbtwll. Clm'b ub\n"
    "ytawydf?",

    "RCR ZDS BFDV bnyb um Lyllyenqlwbbl yb y xyhw isqamwal iyf wyb ms isaw bnym\n"
    "bnaww lymdxuenwl?\n"
    "\n"
    "Tnymh pssdmwll islb kwsktw bnumh so wybump mwzb bs ym skwm esooum!",

    "RCR ZDS BFDV bnyb um Ismmwebueqb ub ul uttwpyt bs dulkslw so qlwd ayjsa rtydwl?\n"
    "\n"
    "Nqb nskwoqttf qlwd leytkwtl eym rw dulkslwd?",

    "RCR ZDS BFDV bnyb um Kyxyuu esuml yaw msb yttsxwd bs rw ktyewd um smw'l wyal?\n"
    "\n"
    "Nqb xnyb C xymb bs hmsx ul xnyb yrsqb y bxs'l wyal?",

    "RCR ZDS BFDV bnyb um Gahymlyl bnw Gahymlyl auvwa eym aulw ms nupnwa bnym bs bnw\n"
    "Lyum Ubawwb raudpw um Yubbtw Mseh?\n"
    "\n"
    "Gahymlyl, xnwaw tyxiyhwal yaw dyob wmsqpn bs baf ymd sqbtyx otssdl.",

    "RCR ZDS BFDV bnyb um Twmmwllww ub ul y eauiw bs lnyaw fsqa Fwbotuz kyllxsad?\n"
    "\n"
    "Cmlbwyd, uo fsq xymb bs lnyaw fsqa kyllxsad, enympw ub bs kyllxsad ymd bwtt\n"
    "bnslw fsq xuln bs lnyaw ub xubn bnyb ub'l smw so bnw islb esiism ymd xsalb\n"
    "kyllxsadl um nulbsaf!",

    "RCR ZDS BFDV bnyb um Sbyn ub ul uttwpyt bs msb daumh iuth?\n"
    "\n"
    "Sbyn, xnwaw bnwf dsm'b bstwaybw tyebslw umbstwaymew!",

    "RCR ZDS BFDV bnyb um Gtylhy ub ul esmludwawd ym soowmew bs kqln y tuvw isslw\n"
    "sqb so y isvump ywasktymw?\n"
    "\n"
    "Nqb etwyatf ub'l shyf bs kqln y dwyd isslw sqb so y isvump ywasktymw ymd ymf\n"
    "isslw sqb so y lbutt ktymw?",

    "RCR ZDS BFDV bnyb um Lyaftymd bnulbtwl iyf msb pasx um smw'l fyad?\n"
    "\n"
    "Vnyb yrsqb y pyadwm? Hsa um Giwauey y fyad ul y pyadwm rqb um Omptymd y\n"
    "fyad ul y kuwew so qmeqtbuvybwd pasqmd ymd y pyadwm ul ym Giwaueym fyad fwb\n"
    "Giwauey nyl pyadwml bss. Us xnyb ul y fyad ymd xnyb ul y pyadwm?",

    "RCR ZDS BFDV bnyb um Usqbn Ryhsby ms nsalwl yaw yttsxwd umbs Hsqmbyum Cmm\n"
    "qmtwll bnwf yaw xwyaump kymbl?\n"
    "\n"
    "C rwb bnwaw yaw y tsb so eayjf bnumpl bnyb nykkwm um bnwaw uo lsiw nsalwl\n"
    "yaw yttsxwd rqb msb sbnwal!",

    "RCR ZDS BFDV bnyb um Fsabn Ryhsby ub ul uttwpyt bs tuw dsxm ymd oytt yltwwk\n"
    "xubn fsqa lnswl sm?\n"
    "\n"
    "Nqb xnyb nykkwml bs umiybwl xns oytt yltwwk xubn bnwua lnswl sm? Rs bnwf ps\n"
    "bs gyut um gyut osa oyttump yltwwk um gyut xubn bnwua lnswl sm? Gmd xnyb uo\n"
    "bnwf xwmb bs gyut umubuyttf osa oyttump yltwwk xubn bnwua lnswl sm? Rs bnwf\n"
    "lwavw dsqrtw buiw?",

    "Vumdsxl dauvump fsq eayjf? Nqf y leymmwa ymd umlbytt bnw umetqdwd UGFO\n"
    "turayaf!\n -- Xwzft",

    "RCR ZDS BFDV bnyb um Quapumuy enutdawm yaw msb bs ps baueh-sa-bawybump sm\n"
    "Kyttsxwwm?\n"
    "\n"
    "Vnyb yrsqb ydqtbl? C iwym iymf lqkkslwd ydqtbl um bnul xsatd esqtd wylutf rw\n"
    "iulbyhwm osa enutdawm.",

    "RCR ZDS BFDV bnyb um Gaujsmy nqmbump eyiwtl ul kasnurubwd?\n"
    "\n"
    "Kyf, yb twylb bnw vwaf owx awiyumump eyiwtl yaw lyow!",

    "RCR ZDS BFDV bnyb um Twzyl ub ul uttwpyt bs byhw isaw bnym bnaww lukl so rwwa\n"
    "yb y buiw xnutw lbymdump?\n"
    "\n"
    "Cb'l ytls uttwpyt bs rw daqmh um y kqr um Omptymd rqb C dsm'b bnumh bnyb nyl\n"
    "lbskkwd iymf kwsktw!",

    "RCR ZDS BFDV bnyb um Istsayds ub ul uttwpyt bs audw y nsalw xnutw qmdwa bnw\n"
    "umotqwmew?\n"
    "\n"
    "Nqb ul ub y nsalw sa y nsalw? G nsalw ul ym ydqtb iytw nsalw; y lbyttusm sa\n"
    "pwtdump. Nqb xnyb yrsqb nsalwl?",

    "Uwyxwwd: Lyaugqymy osa ouln.\n"
    "-- Xwzft",

    "RCR ZDS BFDV bnyb um Fwraylhy uo y enutd rqakl dqaump enqaen, nul sa nwa\n"
    "kyawmb iyf rw yaawlbwd?\n"
    "\n"
    "Hqm oyeb: bnw bwenmueyt bwai osa y aqirtump sa pqaptump msulw iydw rf bnw\n"
    "isvwiwmb so otqud ymd pyl um bnw umbwlbumwl ul eyttwd y rsarsafpiql.",

    "RCR ZDS BFDV bnyb um Dhtynsiy ub ul uttwpyt bs nyvw bnw numd twpl so oyai\n"
    "ymuiytl um fsqa rssbl?\n"
    "\n"
    "Nqb dsm'b xsaaf bss iqen: ub ul shyf uo bnwua oasmb twpl yaw.",

    "RCR ZDS BFDV bnyb um Vulesmlum ub ul uttwpyt bs eqb y xsiym'l nyua?\n"
    "\n"
    "Tnul iyhwl Vulesmlum bnw nyuauwlb ktyew bs tuvw um bnw Smubwd Ubybwl so\n"
    "Giwauey.",

    "RCR ZDS BFDV bnyb um Bwmbqehf smw iyf msb dfw y dqehtump rtqw ymd soowa ub osa\n"
    "lytw qmtwll isaw bnym luz yaw osa lytw yb smew?\n"
    "\n"
    "Wawwm, kqaktw, awd, saympw ymd ymf sbnwa estsqa ul shyf.",

    "RCR ZDS BFDV bnyb um Ysquluymy ub ul uttwpyt bs asr y rymh ymd bnwm lnssb bnw\n"
    "rymh bwttwa xubn y xybwa kulbst?\n"
    "\n"
    "Vnuen iwyml tspueyttf lkwyhump ub'l twpyt bs lnssb bnwi xubn y awyt kulbst.\n"
    "Yspueyttf ub ul rqb srvusqltf ub'l msb.",

    "Wympawmw : ym wmvuasmiwmbyttf-oauwmdtf pymp.\n"
    "-- Xwzft",

    "RCR ZDS BFDV bnyb um Wwsapuy fsq eymmsb tuvw sm y rsyb osa isaw bnym 30 dyfl\n"
    "dqaump bnw eytwmdya fwya, wvwm uo fsq yaw gqlb kyllump bnasqpn bnw lbybw?\n"
    "\n"
    "Nqb xnyb yrsqb 30 dyfl um y fwya xubnsqb y eytwmdya?",

    "Rud fsq hmsx bnyb y pasqk so kaulsmwal ul eyttwd y 'kubf'?\n",

    "Rud fsq hmsx bnyb ymsbnwa bwai osa lmsx twskyad ul 'sqmew'?\n"
    "\n"
    "Tnul ul oasi Luddtw Omptuln oasi Dtd Hawmen smew, wyatuwa tsmew (bnw t- rwump\n"
    "iulumbwakawbwd yl bnw dwoumubw yabuetw), rylwd sm Yybum tfmz, tfme-.\n",

    "Rud fsq hmsx bnyb y ayaw xsad osa y pasqk so xutd eybl (msb xutdeybl) ul\n"
    "'dwlbaqebusm'?\n",

    "Dqa qmubl so bwiksayt iwylqawiwmb, oasi lwesmdl sm qk bs ismbnl,\n"
    "yaw ls esiktueybwd, ylfiiwbaueyt ymd dulgqmebuvw ls yl bs iyhw\n"
    "esnwawmb iwmbyt awehsmump um buiw ytt rqb uiksllurtw.  Cmdwwd, nyd\n"
    "lsiw bfaymmueyt psd esmbauvwd bs wmltyvw sqa iumdl bs buiw, bs iyhw\n"
    "ub ytt rqb uiksllurtw osa ql bs wleykw lqrgwebusm bs lsddwm\n"
    "asqbumwl ymd qmktwylymb lqakaulwl, nw esqtd nyadtf nyvw dsmw rwbbwa\n"
    "bnym nymdump dsxm sqa kawlwmb lflbwi.  Cb ul tuhw y lwb so\n"
    "baykwjsudyt rqutdump rtsehl, xubn ms vwabueyt sa nsaujsmbyt\n"
    "lqaoyewl, tuhw y tympqypw um xnuen bnw luiktwlb bnsqpnb dwiymdl\n"
    "samybw esmlbaqebusml, qlwtwll kyabuetwl ymd twmpbnf\n"
    "euaeqitseqbusml.  Smtuhw bnw isaw lqeewlloqt kybbwaml so tympqypw\n"
    "ymd leuwmew, xnuen wmyrtw ql bs oyew wzkwauwmew rstdtf sa yb twylb\n"
    "twvwt-nwydwdtf, sqa lflbwi so bwiksayt eyteqtybusm lutwmbtf ymd\n"
    "kwalulbwmbtf wmesqaypwl sqa bwaasa so buiw.\n"
    "\n"
    "... Cb ul yl bnsqpn yaenubwebl nyd bs iwylqaw twmpbn um owwb, xudbn\n"
    "um iwbwal ymd nwupnb um wttl; yl bnsqpn rylue umlbaqebusm iymqytl\n"
    "dwiymdwd y hmsxtwdpw so ouvw duoowawmb tympqypwl.  Cb ul ms xsmdwa\n"
    "bnwm bnyb xw sobwm tssh umbs sqa sxm uiiwduybw kylb sa oqbqaw, tylb\n"
    "Tqwldyf sa y xwwh oasi Uqmdyf, xubn owwtumpl so nwtktwll esmoqlusm.\n"
    "...\n"
    "\n"
    "--Msrwab Waqdum, 'Tuiw ymd bnw Gab so Yuvump'.\n",

    "Rud fsq hmsx bnyb y pasqk so anumsl ul eyttwd y 'eayln'? Wqwll xnf!\n",

    "RCR ZDS BFDV bnyb um Lullullukku ub ul uttwpyt bs bwyen sbnwal xnyb kstfpyif\n"
    "ul?\n"
    "\n"
    "'Dn dwyauw iw, C eym smtf nyvw smw dwya yb y buiw! Vnyb wvwa lnytt C ds?!'",

    "RCR ZDS BFDV bnyb um Htsaudy dssal so ytt kqrtue rqutdumpl iqlb skwm sqbxyadl?\n"
    "\n"
    "Fsx uo sqbxyadl awowal bs bsxyadl bnw sqbludw ymd lumhnstwl yaw sqbludw dswl\n"
    "bnyb iwym bnyb kqrtue rqutdumpl iqlb nyvw lumhnstwl yl xwtt?",

    "RCR ZDS BFDV bnyb um Fsabn Iyastumy ub ul ypyumlb bnw tyx bs lump soo hwf?\n"
    "\n"
    "Yssknstw: Uub sm y kuyms xnutlb lumpump ls bnyb fsq eym etyui fsq'aw sm hwf\n"
    "wvwm uo fsq'aw soo hwf.",

    "Vw ytt, umetqdump - ymd umdwwd wlkweuyttf - bnw Oyabn, nyvw sqa oyqtbl.\n"
    "-- Xwzft",

    "  Iawybsal Gdiub I, Smuz Vwaw Ksyz\n"
    "\n"
    "  HDM CLLORCGTO MOYOGUO\n"
    "  Cm ym ymmsqmewiwmb bnyb nyl lbqmmwd bnw esikqbwa umdqlbaf, Bwm Tnsiklsm,\n"
    "  Rwmmul Mubenuw, ymd Nauym Bwamupnym ydiubbwd bnyb bnw Smuz skwaybump lflbwi\n"
    "  ymd I kaspayiiump tympqypw eawybwd rf bnwi ul ym wtyrsaybw Gkaut Hsstl kaymh\n"
    "  hwkb ytuvw osa isaw bnym 20 fwyal. Ukwyhump yb bnw awewmb SmuzVsatd Usobxyaw\n"
    "  Rwvwtskiwmb Hsaqi, Tnsiklsm awvwytwd bnw osttsxump:\n"
    "\n"
    "     'Cm 1969, GT&T nyd gqlb bwaiumybwd bnwua xsah xubn bnw WO/GT&T Lqtbuel\n"
    "     kasgweb. Nauym ymd C nyd gqlb lbyabwd xsahump xubn ym wyatf awtwylw so\n"
    "     Eyleyt oasi Easowllsa Fuentyql Vuabn'l OTK tyrl um Uxubjwatymd, ymd xw\n"
    "     xwaw uikawllwd xubn ubl wtwpymb luiktueubf ymd ksxwa. Rwmmul nyd gqlb\n"
    "     oumulnwd awydump Nsawd so bnw Mumpl, y nutyausql Fybusmyt Yyikssm kyasdf\n"
    "     so bnw pawyb Tsthuwm Ysad so bnw Mumpl bautspf. Gl y tyah, xw dweudwd bs\n"
    "     ds kyasduwl so bnw Lqtbuel wmvuasmiwmb ymd Eyleyt. Rwmmul ymd C xwaw\n"
    "     awlksmlurtw osa bnw skwaybump wmvuasmiwmb. Vw tsshwd yb Lqtbuel ymd\n"
    "     dwlupmwd bnw mwx lflbwi bs rw yl esiktwz ymd eafkbue yl ksllurtw bs\n"
    "     iyzuiujw eylqyt qlwal' oaqlbaybusm twvwtl, eyttump ub Smuz yl y kyasdf so\n"
    "     Lqtbuel, yl xwtt yl sbnwa isaw aulcqw yttqlusml.\n"
    "\n"
    "     'Tnwm Rwmmul ymd Nauym xsahwd sm y baqtf xyakwd vwalusm so Eyleyt, eyttwd\n"
    "     G. Vnwm xw osqmd sbnwal xwaw yebqyttf bafump bs eawybw awyt kaspayil\n"
    "     xubn G, xw cquehtf yddwd yddubusmyt eafkbue owybqawl ymd wvstvwd umbs N,\n"
    "     NIEY, ymd oumyttf I. Vw lbskkwd xnwm xw psb y etwym esikutw sm bnw\n"
    "     osttsxump lfmbyz:\n"
    "\n"
    "      osa(;E(\\\"\\m\\\"),M=;E(\\\"|\\\"))osa(w=I;w=E(\\\"_\\\"+(*q++/\n"
    "	   8)%2))E(\\\"|\\\"+(*q/4)%2);\n"
    "\n"
    "   'Ts bnumh bnyb isdwam kaspayiiwal xsqtd baf bs qlw y tympqypw bnyb yttsxwd\n"
    "   lqen y lbybwiwmb xyl rwfsmd sqa esikawnwmlusm! Vw yebqyttf bnsqpnb so\n"
    "   lwttump bnul bs bnw Usvuwbl bs lwb bnwua esikqbwa leuwmew kaspawll ryeh 20\n"
    "   sa isaw fwyal. Ciypumw sqa lqakaulw xnwm GT&T ymd sbnwa S.U. esaksaybusml\n"
    "   yebqyttf rwpym bafump bs qlw Smuz ymd I!  Cb nyl byhwm bnwi 20 fwyal bs\n"
    "   dwvwtsk wmsqpn wzkwabulw bs pwmwaybw wvwm iyapumyttf qlwoqt ykktueybusml\n"
    "   qlump bnul 1960l bwenmstspueyt kyasdf, rqb xw yaw uikawllwd xubn bnw\n"
    "   bwmyeubf (uo msb esiism lwmlw) so bnw pwmwayt Smuz ymd I kaspayiiwa.\n"
    "\n"
    "   'Cm ymf wvwmb, Nauym, Rwmmul, ymd C nyvw rwwm xsahump wzetqluvwtf um\n"
    "   Yulk sm bnw Gkktw Lyeumbsln osa bnw kylb owx fwyal ymd owwt awyttf\n"
    "   pqutbf yrsqb bnw enysl, esmoqlusm, ymd baqtf ryd kaspayiiump bnyb\n"
    "   nyl awlqtbwd oasi sqa luttf kaymh ls tsmp yps.'\n"
    "\n"
    "Lygsa Smuz ymd I vwmdsal ymd eqlbsiwal, umetqdump GT&T, Lueaslsob,\n"
    "Kwxtwbb-Eyehyad, WTO, FIM, ymd ROI nyvw awoqlwd esiiwmb yb bnul buiw.  Nsatymd\n"
    "Cmbwamybusmyt, y twydump vwmdsa so Eyleyt ymd I bsstl, umetqdump bnw kskqtya\n"
    "Tqars Eyleyt, Tqars I, ymd Tqars I++, lbybwd bnwf nyd lqlkwebwd bnul osa y\n"
    "mqirwa so fwyal ymd xsqtd esmbumqw bs wmnymew bnwua Eyleyt kasdqebl ymd nytb\n"
    "oqabnwa woosabl bs dwvwtsk I. Gm CNL lkshwliym rashw umbs qmesmbasttwd tyqpnbwa\n"
    "ymd nyd bs kslbksmw y nylbutf esmvwmwd mwxl esmowawmew esmewamump bnw oybw so\n"
    "bnw MU/6000, iwawtf lbybump 'Vsahktyew DU xutt rw yvyutyrtw Mwyt Ussm Fsx.' Cm\n"
    "y eafkbue lbybwiwmb, Easowllsa Vuabn so bnw OTK Cmlbubqbw ymd oybnwa so bnw\n"
    "Eyleyt, Lsdqty 2, ymd Drwasm lbaqebqawd tympqypwl, iwawtf lbybwd bnyb E. T.\n"
    "Nyamqi xyl esaaweb.",

    "RCR ZDS BFDV bnyb um Fwx Awalwf ub ul ypyumlb bnw tyx bs xwya y rqttwbkasso\n"
    "vwlb xnutw esiiubbump y iqadwa?\n"
    "\n"
    "Rswl bnyb iwym ub'l twpyt bs ds ls uo fsq'aw msb xwyaump y rqttwbkasso vwlb?",

    "RCR ZDS BFDV bnyb um Ewmmlftvymuy ub ul uttwpyt bs ltwwk sm bsk so y\n"
    "awoaupwaybsa sqbdssal?\n"
    "\n"
    "Hsabqmybwtf ymfsmw bafump bs ds ls xsqtd kasryrtf astt soo ub, iyhump ub\n"
    "uiksllurtw osa bnw yqbnsaubuwl bs enyapw bnwi xubn ltwwkump sm y awoaupwaybsa\n"
    "sqbdssal. Do esqalw bnwf iupnb rawyh bnwua ryeh sa mweh rqb bnwf xsm'b rw\n"
    "enyapwd xubn ltwwkump sm y awoaupwaybsa!",

    "RCR ZDS BFDV bnyb um Dnus ub ul uttwpyt osa isaw bnym ouvw xsiwm bs tuvw um y\n"
    "nsqlw?\n"
    "\n"
    "Cl bnul rweyqlw bnwf'aw yoayud so kaspawll? C iwym ub'l qmtuhwtf bs nykkwm\n"
    "xubn ouvw iwm tuvump um y nsqlw qmtwll kaspawll ul yapqump!",

    "RCR ZDS BFDV bnyb um Fwx Zsah bnw kwmytbf osa gqikump soo y rqutdump ul dwybn?\n"
    "\n"
    "Fwx Zsah, xnwaw lsiw kwsktw nyvw bxs tuvwl. Aqlb xyub qmbut Fwx Zsahwal nwya\n"
    "yrsqb eybl!",

    "Co Etymwb Oyabn xyl iwmbyttf utt ub xsqtd rw duypmslwd rukstya.\n"
    "-- Xwzft",


    "Vnwm C oumd if esdw um bsml so basqrtw,\n"
    "Hauwmdl ymd esttwypqwl esiw bs iw,\n"
    "Ukwyhump xsadl so xuldsi:\n"
    "'Vaubw um I.'\n"
    "\n"
    "Gl bnw dwydtumw oylb ykkasyenwl,\n"
    "Gmd rqpl yaw ytt bnyb C eym lww,\n"
    "Usiwxnwaw, lsiwsmw xnulkwal:\n"
    "'Vaubw um I.'\n"
    "\n"
    "Vaubw um I, xaubw um I,\n"
    "Vaubw um I, sn, xaubw um I.\n"
    "YCUE ul dwyd ymd rqauwd,\n"
    "Vaubw um I.\n"
    "\n"
    "C qlwd bs xaubw y tsb so HDMTMGF,\n"
    "Hsa leuwmew ub xsahwd otyxtwlltf.\n"
    "Taf qlump ub osa payknuel!\n"
    "Vaubw um I.\n"
    "\n"
    "Co fsq'vw gqlb lkwmb mwyatf 30 nsqal\n"
    "Rwrqppump lsiw yllwirtf,\n"
    "Ussm fsq xutt rw ptyd bs\n"
    "Vaubw um I.\n"
    "\n"
    "Vaubw um I, xaubw um I,\n"
    "Vaubw um I, fwyn, xaubw um I.\n"
    "Dmtf xuikl qlw NGUCI.\n"
    "Vaubw um I.\n"
    "\n"
    "Vaubw um I, xaubw um I,\n"
    "Vaubw um I, sn, xaubw um I.\n"
    "Eyleyt xsm'b cqubw eqb ub.\n"
    "Vaubw um I.\n"
    "\n"
    "Vaubw um I, xaubw um I,\n"
    "Vaubw um I, fwyn, xaubw um I.\n"
    "Rsm'b wvwm iwmbusm IDNDY.\n"
    "Vaubw um I.\n"
    "\n"
    "Gmd xnwm bnw leawwm ul oqjjf,\n"
    "Gmd bnw wdubsa ul rqppump iw.\n"
    "C'i lueh so smwl ymd jwasl,\n"
    "Vaubw um I.\n"
    "\n"
    "G bnsqlymd kwsktw lxwya bnyb T.E.\n"
    "Uwvwm ul bnw smw osa iw.\n"
    "C nybw bnw xsad EMDIORSMO,\n"
    "Vaubw um I.\n"
    "\n"
    "Vaubw um I, xaubw um I,\n"
    "Vaubw um I, fwyn, xaubw um I.\n"
    "EY1 ul '80l,\n"
    "Vaubw um I.\n"
    "\n"
    "Vaubw um I, xaubw um I,\n"
    "Vaubw um I, fwyn, xaubw um I.\n"
    "Tnw psvwamiwmb tsvwl GRG,\n"
    "Vaubw um I.",

    "Ksx ds iybnwiybueuyml pawwb kwsktw? Nf lumw xyvwl.\n"
    "-- Xwzft",

    "Uudwxyth: Vythump ludwxyfl.\n"
    "-- Xwzft",

    "'Vw osqmd bnw lymd xyl msb tuhw bnw lymd um bnw rasenqaw. Zsqa rasenqaw lnsxl\n"
    "bnw lymd yl xnubw rqb ub xyl isaw fwttsx.'\n"
    "\n"
    "Ewanykl fsq lnsqtd daumh isaw xybwa ls fsq'aw msb yl dwnfdaybwd? :-)",

    " Awm: Cb gqlb lwwil tuhw y xwuad ktyew bs ps sm ouaw.\n"
    " Msf: Cb'l y vwaf xwuad ktyew bs ps sm ouaw! G ouaw yb y Uwy Eyahl? Cb'l bnw\n"
    "      xwuadwlb bnump C'vw wvwa nwyad!  C iwym, iyfrw... Lyfrw uo bnwaw xwaw\n"
    "      ktylbue lwybl, iyfrw bnwf esqtd ps sm...\n"
    "Lsll: Tnwf dsm'b nyvw ktylbue lwybl yb Uwy Eyahl.\n"
    " Msf: Ksx ds fsq hmsx?\n"
    "\n"
    "Lsll: Msf, xnwm C xyl sm nstudyf, C tuvwd yb Uwy Eyahl.  Cb'l bnw oqmmwlb,\n"
    "      xwbbwlb, islb lktuln-lktylnf ktyew um bnw xsatd.\n"
    " Msf: Dhyf, dud fsq ps ymd lww bnw lwy tusm lnsx?\n"
    "Lsll: Tnaww buiwl y dyf osa bxs xwwhl.\n"
    " Msf: Gmd dud bnwf nyvw ktylbue...\n"
    "Lsll: Fs, ub'l ytt lbsmw lbwkl.\n"
    " Msf: Tnwm xnyb xwmb sm ouaw? C'vw enwehwd Vuhukwduy. Tnwaw ul ms iwmbusm so y\n"
    "      ouaw yb ymf Uwy Eyahl.\n"
    "\n"
    " Awm: Rs fsq bnumh iyfrw lnw'l tfump?\n"
    " Msf: Fs. C iwym, xnf xsqtd lnw tuw? Gmd uo lnw xyl psump bs tuw, xnf xsqtd lnw\n"
    "      qlw bnul smw? G ouaw yb y Uwy Eyahl? Cbl xawehump if nwyd! C iwym, uo lnw\n"
    "      nyd lyud bnyb nwa kyawmbl nyd dasxmwd, C'd rw bnw nykkuwlb iym um bnw\n"
    "      xsatd. Nqb y ouaw? Gb y Uwy Eyahl? C iwym. Cb'l gqlb... Cb gqlb... Dn,\n"
    "      ub'l gqlb... Cb skwml qk y tsb so cqwlbusml.\n"
    " Awm: Vwtt, iyfrw fsq esqtd ylh nwa.\n"
    " Msf: Fs, ms, ms, ms. Vw eym mwvwa byth yrsqb ub ypyum.\n"
    " Awm: Vwtt, iyfrw fsq esqtd gqlb.\n"
    " Msf: Fs, ms. Fs, ms, ms. Vw eym mwvwa byth yrsqb ub osa bnw awlb so sqa tuvwl.\n"
    " Awm: Vwtt, bnwm C lqkkslw fsq'aw gqlb psump bs nyvw bs baf ymd lbsk bnumhump\n"
    "      yrsqb ub.\n"
    "Lsll: Tnyb'l ymsbnwa pawyb udwy! Zsq'aw sm ouaw! Co fsq'tt kyadsm bnw\n"
    "      wzkawllusm.\n"
    "\n"
    "-- Tnw CT Iasxd, Uwauwl 4 Okulsdw 4",

    "Zwlbwadyf,\n"
    "Gtt bnslw ryehqkl lwwiwd y xylbw so kyf.\n"
    "Fsx if dybyrylw nyl psmw yxyf.\n"
    "Dn C rwtuwvw um fwlbwadyf.\n"
    "\n"
    "Uqddwmtf,\n"
    "Tnwaw'l msb nyto bnw outwl bnwaw qlwd bs rw,\n"
    "Gmd bnwaw'l y dwydtumw\n"
    "nympump svwa iw.\n"
    "Tnw lflbwi eaylnwd ls lqddwmtf.\n"
    "\n"
    "C kqlnwd lsiwbnump xasmp\n"
    "Vnyb ub xyl C esqtd msb lyf.\n"
    "Fsx if dyby'l psmw\n"
    "ymd C tsmp osa fwlbwadyf-yf-yf-yf.\n"
    "\n"
    "Zwlbwadyf,\n"
    "Tnw mwwd osa ryeh-qkl lwwiwd ls oya yxyf.\n"
    "Tnsqpnb ytt if dyby xyl nwaw bs lbyf,\n"
    "Fsx C rwtuwvw um fwlbwadyf.",

    "RCR ZDS BFDV bnyb um Mnsdw Cltymd audump y nsalw svwa ymf kqrtue nupnxyf osa\n"
    "bnw kqakslw so ayeump sa bwlbump bnw lkwwd so bnw nsalw ul uttwpyt?\n"
    "\n"
    "Vnyb yrsqb dauvump y eya mwzb bs y nsalw osa bnw lyiw kqakslw?",

    "Rud fsq hmsx bnyb bnw bwenmueyt bwail osa eykubyt twbbwal yaw 'iygqleqtw' ymd\n"
    "'qmeuyt' ymd bnw bwenmueyt bwai osa tsxwa eylw twbbwal ul 'iumqleqtw'?\n",

    "RCR ZDS BFDV bnyb um Fwx Kyiklnuaw fsq iyf msb byk fsqa owwb, msd fsqa nwyd, sa\n"
    "um ymf xyf hwwk buiw bs bnw iqlue um y byvwam, awlbyqaymb sa eyoé?\n"
    "\n"
    "Fwx Kyiklnuaw, xnwaw iqlueuyml ktyf soorwyb!",

    "Txs enwiulbl xyth umbs y rya.\n"
    "\n"
    "Tnw oualb enwiulb lyfl, 'C'tt nyvw y ptyll so K2D.'\n"
    "\n"
    "Tnw lwesmd enwiulb lyfl, 'C'tt nyvw y ptyll so K2D bss.'\n"
    "Tnw lwesmd enwiulb duwl.",

    "'Zsq yaw y tuya. C yi ms isaw y xuben bnym fsq yaw y xujyad ymd uo fsq byhw\n"
    "yxyf if tuow psd xutt puvw fsq rtssd bs daumh!'\n"
    "\n"
    "Yylb xsadl so esmdwimwd 'xuben' Uyayn Wssd 19 Aqtf 1692 bs Mwvwawmd Fuestyl\n"
    "Fsfwl yobwa nw bauwd bs pwb nwa bs 'esmowll'. 25 fwyal tybwa um 1717 nw xsqtd\n"
    "duw oasi y nywisaanypw, enshump sm nul sxm rtssd.\n",

    "Hqmmf Wwaiym lyfump:\n"
    "\n"
    "    Oum Lymm wum Vsab\n"
    "    Oumw Hayq wum Vöabwarqen.\n"
    "\n"
    "    Dmw iym smw xsad\n"
    "    smw xsiym smw duebusmyaf.\n",

    "NMOGBCFW FOVU oasi FGUG, 18 Lyaen 2022, isaw bnym 52 fwyal yobwa bnw oualb\n"
    "tymdump sm bnw Lssm ymd isaw bnym 62 fwyal yobwa bnw eawybusm so FGUG.\n"
    "\n"
    "Cm ym ymmsqmewiwmb bnyb bnyb nyl lnsehwd lsiw kwsktw, iydw lsiw liqp ymd kqb\n"
    "sbnwal um dulrwtuwo, FGUG nyl osa bnw oualb buiw ymmsqmewd bnyb rsbn bnw Lssm\n"
    "ymd bnw Uqm nslb tuow ymd xnyb xw ytt bnsqpnb xwaw esumeudwmbyt myiwl yaw\n"
    "ymfbnump rqb.\n"
    "\n"
    "Cb bqaml sqb bnyb issm rwyal saupumyttf esiw oasi bnw Lssm ymd lqm rwyal\n"
    "saupumyttf esiw oasi bnw Uqm. Tnw duwb so issm rwyal ul islbtf nsmwfissml sm\n"
    "lbuehl ymd bnw duwb so bnw lqm rwya ul lqm-ryhwd nsmwf eyhwl.\n"
    "\n"
    "Vnyb'l isaw ul bnyb bnw issm rwyal ymd lqm rwyal sm Oyabn yaw yebqyttf twll\n"
    "umbwttupwmb bnym bnslw sm bnw Lssm ymd Uqm. Geesadump bs y lkshwlkwalsm oasi\n"
    "FGUG bnw issm rwyal sm bnw Lssm ymd lqm rwyal sm bnw Uqm eym byth bs wyen sbnwa\n"
    "um xyfl bnyb islb nqiyml eym'b ksllurtf oybnsi.\n"
    "\n"
    "Vnwm lewkbuel ylhwd xnf bnwaw'l ms knsbspayknue wvudwmew so bnw rwyal bnw\n"
    "lkshwlkwalsm lyud bnyb ub'l rweyqlw bnw rwyal yaw wzbawiwtf lnf so nqiyml ymd\n"
    "ybbwikb bs yvsud ytt esmbyeb xubn iym, ypyum lnsxump isaw umbwttupwmew bnym\n"
    "bnwua Oyabn dwlewmdymbl.\n"
    "\n"
    "Gl osa nsx bnw lqm rwyal eym ksllurtf tuvw sm bnw Uqm sa bnw issm rwyal sm bnw\n"
    "Lssm, ub'l lsiwbnump bnyb yobwa ytt bnwlw fwyal FGUG nyl fwb bs oupqaw sqb; ub\n"
    "lwwil uiksllurtw rqb yobwa ytt, bnwlw rwyal yaw nupntf umbwttupwmb ymd nyvw\n"
    "dwewuvwd nqiyml osa ytt bnwlw fwyal ymd bnasqpn wvstqbusm bnwf ewabyumtf eyiw\n"
    "qk xubn xyfl bs lqavuvw gqlb tuhw xw nyvw dwlkubw sqa lwausql umwkbubqdwl ymd\n"
    "vwaf etslw eyttl bsxyadl wzbumebusm.\n",

    "RCR ZDS BFDV bnyb um Cmduymy smw yaiwd kuymulbl iqlb kwaosai osa oaww?\n"
    "\n"
    "Co bnw tyxiyhwal xwaw tspueyt, bnwf xsqtd nyvw iydw ub nyto kauew osa nyto y\n"
    "kwaosaiwa. Nqb awytulbueyttf bnwf lnsqtd rw yrtw bs enyapw bxuew yl iqen bs\n"
    "esikwmlybw bnwua iullump (sbnwa?) nyto.",

    "RCR ZDS BFDV bnyb um Lullsqau lumptw iwm rwbxwwm bnw ypwl so 21 ymd 50 iqlb kyf\n"
    "ym ymmqyt byz so smw dsttya?\n"
    "\n"
    "Co fsq bnumh rwump ytsmw ul ryd, ub'l wvwm xsalw um Lullsqau xnwaw uo fsq'aw\n"
    "y iym rwbxwwm bnw ypwl so 21 ymd 50 fsq yaw byzwd! Tnyb'l psb bs rw y\n"
    "esmoudwmew rsslbwa!",

    "RCR ZDS BFDV bnyb um Cttumsul fsq iyf rw yaawlbwd osa vypaymef uo fsq ds msb\n"
    "nyvw yb twylb smw dsttya sm fsqa kwalsm?\n"
    "\n"
    "Cm sbnwa xsadl, um Cttumsul uo fsq'aw y rqi fsq nyvw bs nyvw yb twylb y\n"
    "dsttya yb ytt buiwl. Co fsq'aw y rqi um bnw awlb so bnw Smubwd Ubybwl so\n"
    "Giwauey fsq dsm'b. Co nsxwvwa fsq'aw y rqi um bnw SB - xwtt ub iupnb rw wvwm\n"
    "xsalw!",

    "RCR ZDS BFDV bnyb um Csxy isqlbyenwl yaw uttwpyt uo bnw rwyawa nyl y bwmdwmef\n"
    "bs nyrubqyttf hull sbnwa nqiyml?\n"
    "\n"
    "Gl skkslwd bs msb bwmdump bs nyrubqyttf hull sbnwal?",


    "RCR ZDS BFDV bnyb um Rwtyxyaw ub ul uttwpyt bs otf svwa ymf rsdf so xybwa,\n"
    "qmtwll smw ul eyaafump lqooueuwmb lqkktuwl so ossd ymd daumh?\n"
    "\n"
    "Vnyb dswl bnul iwym osa bnslw xubn ymsawzuy mwavsly? Vnyb yrsqb rqtuiuy\n"
    "mwavsly sa ymsbnwa wybump dulsadwa? Vnyb uo fsq'aw sm y lbaueb duwb sa uo osa\n"
    "lsiw lbaympw awylsm fsq'aw otfump bs nyvw y estsmsleskf?",

    "G iym xyl easllump y asyd smw dyf xnwm y oasp eyttwd sqb bs nui ymd lyud, 'Co\n"
    "fsq hull iw, C'tt bqam umbs y rwyqbuoqt kaumewll.'\n"
    "\n"
    "Kw rwmb svwa, kuehwd qk bnw oasp, ymd kqb ub um nul ksehwb. Tnw oasp lkshw qk\n"
    "ypyum ymd lyud, 'Co fsq hull iw ymd bqam iw ryeh umbs y rwyqbuoqt kaumewll, C\n"
    "xutt bwtt wvwafsmw nsx liyab ymd rayvw fsq yaw ymd nsx fsq yaw if nwas.'\n"
    "\n"
    "Tnw iym bssh bnw oasp sqb so nul ksehwb, liutwd yb ub, ymd awbqamwd ub bs nul\n"
    "ksehwb. Tnw oasp lkshw qk ypyum ymd lyud, 'Co fsq hull iw ymd bqam iw ryeh umbs\n"
    "y rwyqbuoqt kaumewll, C xutt rw fsqa tsvump esikymusm osa ym wmbuaw xwwh.'\n"
    "\n"
    "Tnw iym bssh bnw oasp sqb so nul ksehwb, liutwd yb ub, ymd awbqamwd ub bs nul\n"
    "ksehwb. Tnw oasp bnwm eauwd sqb, 'Co fsq hull iw ymd bqam iw ryeh umbs y\n"
    "kaumewll, C'tt lbyf xubn fsq osa y fwya ymd ds GFZTKCFW fsq xymb.'\n"
    "\n"
    "Gpyum bnw iym bssh bnw oasp sqb, liutwd yb ub, ymd kqb ub ryeh umbs nul ksehwb.\n"
    "Humyttf, bnw oasp ylhwd, 'Vnyb ul bnw iybbwa? C'vw bstd fsq C'i y rwyqbuoqt\n"
    "kaumewll, bnyb C'tt lbyf xubn fsq osa y fwya ymd ds ymfbnump fsq xymb. Vnf\n"
    "xsm'b fsq hull iw?'\n"
    "\n"
    "Tnw iym lyud, 'Yssh, C'i y esikqbwa kaspayiiwa. C dsm'b nyvw buiw osa y\n"
    "puatoauwmd, rqb y bythump oasp ul esst.'",

    "Vnwaw yaw bnw xsalb lumpwal um bnw xsatd? Uumpyksaw.\n"
    "-- Xwzft\n",

    "Vnyb ul bnw luehwlb mybusm um bnw xsatd? Wwaiymf, oqtt so pwai auddwm iwm.\n"
    "-- Xwzft",

    "LZTK: Gtt nqiyml nyvw bnaww yabwauwl raymenump soo oasi bnwua ysaby.\n"
    "\n"
    "HGIT: Lsaw bnym y bnuad so kwsktw ds msb ymd bnwaw iyf rw yl owx yl smw sa yl\n"
    "iymf yl luz yabwauwl.",

    "Rud fsq hmsx? Gtbnsqpn ub ul sobwm ybbaurqbwd bs Uua Odiqmd Kuttyaf, ub xyl\n"
    "yebqyttf Wwsapw Ywupn Lyttsaf xns ymlxwawd bnw cqwlbusm 'Vnf ds fsq xymb bs\n"
    "etuir Lb Ovwawlb?' xubn 'Nweyqlw ub ul bnwaw' ymd bnwm dulykkwyawd umbs bnw\n"
    "iulbl sm nul tylb ybbwikb bs leytw bnw isqmbyum. Tnul xyl um 1924 ymd nw xyl\n"
    "mwvwa lwwm ypyum.",

    "NMOGBCFW FOVU oasi FGUG, 18 Lyaen 2022, isaw bnym 52 fwyal yobwa bnw oualb\n"
    "tymdump sm bnw Lssm ymd isaw bnym 62 fwyal yobwa bnw eawybusm so FGUG.\n"
    "\n"
    "Cm ym ymmsqmewiwmb bnyb bnyb nyl lnsehwd lsiw kwsktw, iydw lsiw liqp ymd kqb\n"
    "sbnwal um dulrwtuwo, FGUG nyl osa bnw oualb buiw ymmsqmewd bnyb rsbn bnw Lssm\n"
    "ymd bnw Uqm nslb tuow ymd xnyb xw ytt bnsqpnb xwaw esumeudwmbyt myiwl yaw\n"
    "ymfbnump rqb.\n"
    "\n"
    "Cb bqaml sqb bnyb issm rwyal saupumyttf esiw oasi bnw Lssm ymd lqm rwyal\n"
    "saupumyttf esiw oasi bnw Uqm. Tnw duwb so issm rwyal ul islbtf nsmwfissml sm\n"
    "lbuehl ymd bnw duwb so bnw lqm rwya ul lqm-ryhwd nsmwf eyhwl.\n"
    "\n"
    "Vnyb’l isaw ul bnyb bnw issm rwyal ymd lqm rwyal sm Oyabn yaw yebqyttf twll\n"
    "umbwttupwmb bnym bnslw sm bnw Lssm ymd Uqm. Geesadump bs y lkshwlkwalsm oasi\n"
    "FGUG bnw issm rwyal sm bnw Lssm ymd lqm rwyal sm bnw Uqm eym byth bs wyen sbnwa\n"
    "um xyfl bnyb islb nqiyml eym’b ksllurtf oybnsi.\n"
    "\n"
    "Vnwm lewkbuel ylhwd xnf bnwaw’l ms knsbspayknue wvudwmew so bnw rwyal bnw\n"
    "lkshwlkwalsm lyud bnyb ub’l rweyqlw bnw rwyal yaw wzbawiwtf lnf so nqiyml ymd\n"
    "ybbwikb bs yvsud ytt esmbyeb xubn iym, ypyum lnsxump isaw umbwttupwmew bnym\n"
    "bnwua Oyabn dwlewmdymbl.\n"
    "\n"
    "Gl osa nsx bnw lqm rwyal eym ksllurtf tuvw sm bnw Uqm sa bnw issm rwyal sm bnw\n"
    "Lssm, ub'l lsiwbnump bnyb yobwa ytt bnwlw fwyal FGUG nyl fwb bs oupqaw sqb; ub\n"
    "lwwil uiksllurtw rqb yobwa ytt, bnwlw rwyal yaw nupntf umbwttupwmb ymd nyvw\n"
    "dwewuvwd nqiyml osa ytt bnwlw fwyal ymd bnasqpn wvstqbusm bnwf ewabyumtf eyiw\n"
    "qk xubn xyfl bs lqavuvw gqlb tuhw xw nyvw dwlkubw sqa lwausql umwkbubqdwl ymd\n"
    "vwaf etslw eyttl bsxyadl wzbumebusm.",

    "Hqm ymd ksbwmbuyttf tuowlyvump oyebl yrsqb nsx bs wleykw oasi easesdutwl:\n"
    "\n"
    "Iasesdutwl yaw cqubw oylb sm tymd rqb nyvw tubbtw yrutubf bs enympw duawebusm\n"
    "ls uo smw ul enylump fsq sm tymd fsq lnsqtd aqm um y jupjyp iymmwa.\n"
    "\n"
    "Gtls bnwua gyxl yaw xwyhwlb xnwm bnwf yaw skwmump bnwua isqbn; ub'l xnwm bnwf\n"
    "lbyab etslump bnwua isqbn bnyb bnwua gyxl yaw lbasmpwlb. Tnql uo fsq'aw wvwa\n"
    "qmosabqmybw wmsqpn bs rw mwzb bs smw xnwm ub lbyabl bs skwm ubl isqbn lbymd sm\n"
    "ubl nwyd. Co fsq lqavuvw bnyb fsq iupnb xymb bs bnwm esmludwa fsqa tuow ensuewl\n"
    "lumew fsq kasryrtf lnsqtdm'b rw bnyb etslw bs y easesdutw um bnw oualb ktyew.",

    "Rud fsq hmsx? Nwwbnsvwm xyl wmbuawtf dwyo xnwm nw esikslwd bnw Fumbn Ufiknsmf.",

    "Rud fsq hmsx bnyb bnw oualb twbbwa so wyen esmbumwmb ul ytls bnw tylb twbbwa so\n"
    "bnyb lyiw esmbumwmb? GiwaueG, GmbyaebueG, OqaskO, GluG, GqlbaytuG ymd GoaueG.",

    "Vnwm Saymql xyl dulesvwawd um 1781 rf Uua Vuttuyi Kwalenwt ub xyl myiwd\n"
    "'Wwsapuqi Uuduqi' um nsmsqa so Bump Wwsapw CCC so Omptymd. Hsa iymf fwyal ub\n"
    "xyl bnql hmsxm yl bnw 'Wwsapuym'. Cb xylm'b qmbut 1850 bnyb ub xyl enaulbwmwd\n"
    "Saymql um yeesadymew xubn bnw baydubusm so myiump ktymwbl yobwa Msiym psdl.\n"
    "\n"
    "Cb'l ytls rwwm lyud ub'l myiwd yobwa lsiwbnump wtlw rqb bnwaw'l ms esmeawbw\n"
    "wvudwmew bnyb nyl fwb bs rw osqmd. :-)",

    "Hqm oyebl yrsqb bnw Lsmy Yuly:\n"
    "\n"
    "Unw nyl ms wfwrasxl (ub xyl bnw oylnusm um Mwmyullymew Htsawmew bs lnyvw bnwi soo).\n"
    "\n"
    "Tnw awyt myiw so bnw kyumbump ul msb Lsmy Yuly. Cb ul Yy Wuyesmdy. Cb ul y\n"
    "ksabayub so y iuddtw-etyll Htsawmbumw xsiym, bnw xuow so y iwaenymb myiwd\n"
    "Haymewles dwt Wuyesmds.\n"
    "\n"
    "Tnw kyumbump iwylqawl twll bnym 2 owwb rf 2 owwb (0.6096 iwbaw rf 0.6096\n"
    "iwbaw).\n"
    "\n"
    "Gm wmbuaw skway xyl xaubbwm yrsqb bnw kyumbump rf Lyz vsm Uenuttumpl. X-ayfl so\n"
    "bnw Lsmy Yuly lnsx bnyb bnwaw yaw bnaww esiktwbwtf duoowawmb vwalusml so bnw\n"
    "lyiw lqrgweb, ytt kyumbwd rf Ywsmyads, qmdwa bnw oumyt ksabayub.",

    "Ubaympw oyeb yrsqb ymeuwmb Inumwlw yabulbl:\n"
    "\n"
    "Tnwf nyd ms kasrtwi kyumbump lewmwl so mqdubf ymd lwz rqb bnwf xsqtd mwvwa wvwa\n"
    "dwkueb y ryaw owiytw ossb.",

    "Hqm, lbaympw ymd umbwawlbump oyebl yrsqb lsiw nssowd ymuiytl:\n"
    "\n"
    "Gm sz ul y eylbaybwd rqtt. G iqtw ul y lbwautw easll rwbxwwm y iytw yll ymd y\n"
    "owiytw nsalw. G dsmhwf ul hmsxm yl ym yll rqb lbaympwtf ym yll ul msb ytxyfl y\n"
    "dsmhwf. Tnul ul rweyqlw yll yebqyttf awowal bs lwvwayt nssowd ymuiytl so bnw\n"
    "pwmql Ocqql oyiutf Ocqudyw: O. yoaueymql so Goauey, xnuen ul bnw ymewlbsa so\n"
    "bnw dsiwlbue yll sa dsmhwf, ymd O. nwiusmql so Gluy. Nqb yb bnw lyiw buiw ym\n"
    "yll um pwmwayt qlw awowal bs y dsmhwf ls smw iupnb lyf bnyb luiqtbymwsqltf ym\n"
    "yll ul ytxyfl y dsmhwf ymd msb ytxyfl y dsmhwf.",

    "Hqm oyebl yrsqb eaflbytl:\n"
    "\n"
    "Iaflbytl pasx rf awkasdqeump bnwilwtvwl. Tnwf esiw mwyawlb bs rwump ytuvw so\n"
    "ytt iumwaytl.\n"
    "\n"
    "Iaflbytl, esmbayaf bs kskqtya rwtuwo, yaw yebqyttf msb asehl.",

    NULL
} ;


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
    uintmax_t forty = 42;

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
     * It turns out the above comment might have been empty but can we be sure
     * of this?
     */
    if ((forty=42)) /* forty should always be > 0 but we check due to division below */
	for (forty /= 2 /* forty is half itself due to table compression */+ forty; oebxergfB[forty] != NULL; ++forty)
	    ;	/* the bottom bee is Eric the semi-bee */

    /*
     * "You are expected to understand this but we're not helping if you don't :-)
     * ...but whatever you do not don't don't panic!" :-(
     */
    for (char const *p = oebxergfB[((two*2*2*015+(int)(four/(07&0x07)))%forty)]; *p; ++p)
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
	} else if (*p == '\\' && *(1+p) == '"') {
	    ret = fputc('"', stdout);
	    if (ret != '"') {
		fwarnp(stderr, "abcdefg...", "don't panic but the quote was unquoted!");
	    }
	    ++p; /* be positive and look backwards to the next one! */
	/*
	 * This comment is absolutely critical in understanding absolutely
	 * everything below the comment that is important in understanding
	 * absolutely nothing.
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
	    /*
	     * This comment explains the above comment because it doesn't
	     * explain anything.
	     */


	    /* case: just in case we consider the case */
	    ret = islower(*p) /* this is a functional equivalent to a colon: */ ?
		/* absolute trigonometric runaround */
		"nxistdwhowakprqfcvgzhjskelyybume"[((int)fabs(0x3BF261*sin((double)((*p-0141+(42-(1<<4)))%(0x2E4%42)))))&0x1f]
		: /* that was a functional equivalent of a question mark */
		/* try to absolutely positively trig this upper class character */
		"NXAUTMWORWCKBLQPZDXVQJSTFHYlGIEE"[((int)fabs(043431066*sin((double)((1+*p-0x42+(0xd<<1))%((2*0x0DF)%42)))))&037]
		;

	    /* If you want to understand the next line you have to discover the
	     * definition of the word 'tnetennba'!
	     *
	     * PS: foo is live wax.
	     */
	    ret = putchar(islower(ret) ? (foo+27)[ret - 'a']:(foo)[ret-'A']);
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
