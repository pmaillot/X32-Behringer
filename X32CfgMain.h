/*
 * X32CfgMain.h
 *
 *  Created on: 4 févr. 2015
 *      Author: patrick
 */

#ifndef X32CFGMAIN_H_
#define X32CFGMAIN_H_



X32command Xconfig[] = {
		{"/config",							{OFFON}, F_FND, {16}, NULL},
		{"/config/chlink",					{OFFON}, F_FND, {16}, NULL},
		{"/config/chlink/1-2",			{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/3-4",			{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/5-6",			{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/7-8",			{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/9-10",			{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/11-12",		{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/13-14",		{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/15-16",		{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/17-18",		{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/19-20",		{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/21-22",		{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/23-24",		{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/25-26",		{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/27-28",		{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/29-30",		{E32}, F_XET, {0}, OffOn},
		{"/config/chlink/31-32",		{E32}, F_XET, {0}, OffOn},
		{"/config/auxlink",					{OFFON}, F_FND, {4}, NULL},
		{"/config/auxlink/1-2",			{E32}, F_XET, {0}, OffOn},
		{"/config/auxlink/3-4",			{E32}, F_XET, {0}, OffOn},
		{"/config/auxlink/5-6",			{E32}, F_XET, {0}, OffOn},
		{"/config/auxlink/7-8",			{E32}, F_XET, {0}, OffOn},
		{"/config/fxlink",					{OFFON}, F_FND, {4}, NULL},
		{"/config/fxlink/1-2",			{E32}, F_XET, {0}, OffOn},
		{"/config/fxlink/3-4",			{E32}, F_XET, {0}, OffOn},
		{"/config/fxlink/5-6",			{E32}, F_XET, {0}, OffOn},
		{"/config/fxlink/7-8",			{E32}, F_XET, {0}, OffOn},
		{"/config/buslink",					{OFFON}, F_FND, {8}, NULL},
		{"/config/buslink/1-2",			{E32}, F_XET, {0}, OffOn},
		{"/config/buslink/3-4",			{E32}, F_XET, {0}, OffOn},
		{"/config/buslink/5-6",			{E32}, F_XET, {0}, OffOn},
		{"/config/buslink/7-8",			{E32}, F_XET, {0}, OffOn},
		{"/config/buslink/9-10",		{E32}, F_XET, {0}, OffOn},
		{"/config/buslink/11-12",		{E32}, F_XET, {0}, OffOn},
		{"/config/buslink/13-14",		{E32}, F_XET, {0}, OffOn},
		{"/config/buslink/15-16",		{E32}, F_XET, {0}, OffOn},
		{"/config/mtxlink",					{OFFON}, F_FND, {3}, NULL},
		{"/config/mtxlink/1-2",			{E32}, F_XET, {0}, OffOn},
		{"/config/mtxlink/3-4",			{E32}, F_XET, {0}, OffOn},
		{"/config/mtxlink/5-6",			{E32}, F_XET, {0}, OffOn},
		{"/config/mute",					{OFFON}, F_FND, {6}, NULL},
		{"/config/mute/1",				{E32}, F_XET, {0}, OffOn},
		{"/config/mute/2",				{E32}, F_XET, {0}, OffOn},
		{"/config/mute/3",				{E32}, F_XET, {0}, OffOn},
		{"/config/mute/4",				{E32}, F_XET, {0}, OffOn},
		{"/config/mute/5",				{E32}, F_XET, {0}, OffOn},
		{"/config/mute/6",				{E32}, F_XET, {0}, OffOn},
		{"/config/linkcfg",					{OFFON}, F_FND, {4}, NULL},
		{"/config/linkcfg/hadly",		{E32}, F_XET, {0}, OffOn},
		{"/config/linkcfg/eq",			{E32}, F_XET, {0}, OffOn},
		{"/config/linkcfg/dyn",			{E32}, F_XET, {0}, OffOn},
		{"/config/linkcfg/fdrmute",		{E32}, F_XET, {0}, OffOn},
		{"/config/mono",					{CMONO}, F_FND, {0}, NULL},
		{"/config/mono/mode",			{E32}, F_XET, {0}, Xmnmode},
		{"/config/mono/link",			{E32}, F_XET, {0}, OffOn},
		{"/config/solo",					{CSOLO}, F_FND, {0}, NULL},
		{"/config/solo/level",			{F32}, F_XET, {0}, NULL},
		{"/config/solo/source",			{I32}, F_XET, {0}, XSsourc},
		{"/config/solo/sourcetrim",		{F32}, F_XET, {0}, NULL},
		{"/config/solo/chmode",			{E32}, F_XET, {0}, Xchmode},
		{"/config/solo/busmode",		{E32}, F_XET, {0}, Xchmode},
		{"/config/solo/dcamode",		{E32}, F_XET, {0}, Xchmode},
		{"/config/solo/exclusive",		{E32}, F_XET, {0}, OffOn},
		{"/config/solo/followsel",		{E32}, F_XET, {0}, OffOn},
		{"/config/solo/followsolo",		{E32}, F_XET, {0}, OffOn},
		{"/config/solo/dimatt",			{F32}, F_XET, {0}, NULL},
		{"/config/solo/dim",			{E32}, F_XET, {0}, OffOn},
		{"/config/solo/mono",			{E32}, F_XET, {0}, OffOn},
		{"/config/solo/delay",			{E32}, F_XET, {0}, OffOn},
		{"/config/solo/delaytime",		{F32}, F_XET, {0}, NULL},
		{"/config/solo/masterctrl",		{E32}, F_XET, {0}, OffOn},
		{"/config/solo/mute",			{E32}, F_XET, {0}, OffOn},
		{"/config/solo/dimpfl",			{E32}, F_XET, {0}, OffOn},
		{"/config/talk",					{CTALK}, F_FND, {0}, NULL},
		{"/config/talk/enable",			{E32}, F_XET, {0}, OffOn},
		{"/config/talk/source",			{E32}, F_XET, {0}, XTsourc},
		{"/config/talk/A",					{CTALKAB}, F_FND, {0}, NULL},
		{"/config/talk/A/level",		{F32}, F_XET, {0}, NULL},
		{"/config/talk/A/dim",			{E32}, F_XET, {0}, OffOn},
		{"/config/talk/A/latch",		{E32}, F_XET, {0}, OffOn},
		{"/config/talk/A/destmap",		{P32}, F_XET, {0}, NULL},
		{"/config/talk/B",					{CTALKAB}, F_FND, {0}, NULL},
		{"/config/talk/B/level",		{F32}, F_XET, {0}, NULL},
		{"/config/talk/B/dim",			{E32}, F_XET, {0}, OffOn},
		{"/config/talk/B/latch",		{E32}, F_XET, {0}, OffOn},
		{"/config/talk/B/destmap",		{P32}, F_XET, {0}, NULL},
		{"/config/osc",						{COSC}, F_FND, {0}, NULL},
		{"/config/osc/level",			{F32}, F_XET, {0}, NULL},
		{"/config/osc/f1",				{F32}, F_XET, {0}, NULL},
		{"/config/osc/f2",				{F32}, F_XET, {0}, NULL},
		{"/config/osc/fsel",			{E32}, F_XET, {0}, XOscsel},
		{"/config/osc/type",			{E32}, F_XET, {0}, XOsctyp},
		{"/config/osc/dest",			{I32}, F_XET, {0}, NULL},
		{"/config/routing",						{CROUTSW}, F_FND, {1}, NULL},
		{"/config/routing/routswitch",		{E32}, F_XET, {0}, XCFrsw},
		{"/config/routing/IN",					{CROUTIN}, F_FND, {5}, NULL},
		{"/config/routing/IN/1-8",			{E32}, F_XET, {0}, XRtgin},
		{"/config/routing/IN/9-16",			{E32}, F_XET, {0}, XRtgin},
		{"/config/routing/IN/17-24",		{E32}, F_XET, {0}, XRtgin},
		{"/config/routing/IN/25-32",		{E32}, F_XET, {0}, XRtgin},
		{"/config/routing/IN/AUX",			{E32}, F_XET, {0}, XRtina},
		{"/config/routing/AES50A",				{CROUTAC}, F_FND, {6}, NULL},
		{"/config/routing/AES50A/1-8",		{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50A/9-16",		{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50A/17-24",	{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50A/25-32",	{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50A/33-40",	{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50A/41-48",	{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50B",				{CROUTAC}, F_FND, {6}, NULL},
		{"/config/routing/AES50B/1-8",		{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50B/9-16",		{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50B/17-24",	{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50B/25-32",	{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50B/33-40",	{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/AES50B/41-48",	{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/CARD",				{CROUTAC}, F_FND, {4}, NULL},
		{"/config/routing/CARD/1-8",		{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/CARD/9-16",		{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/CARD/17-24",		{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/CARD/25-32",		{E32}, F_XET, {0}, XRtaea},
		{"/config/routing/OUT",					{CROUTOT}, F_FND, {0}, NULL},
		{"/config/routing/OUT/1-4",			{E32}, F_XET, {0}, XRout1},
		{"/config/routing/OUT/9-12",		{E32}, F_XET, {0}, XRout1},
		{"/config/routing/OUT/5-8",			{E32}, F_XET, {0}, XRout5},
		{"/config/routing/OUT/13-16",		{E32}, F_XET, {0}, XRout5},
		{"/config/routing/PLAY",				{CROUTIN}, F_FND, {0}, NULL},
		{"/config/routing/PLAY/1-8",		{E32}, F_XET, {0}, XRtgin},
		{"/config/routing/PLAY/9-16",		{E32}, F_XET, {0}, XRtgin},
		{"/config/routing/PLAY/17-24",		{E32}, F_XET, {0}, XRtgin},
		{"/config/routing/PLAY/25-32",		{E32}, F_XET, {0}, XRtgin},
		{"/config/routing/PLAY/AUX",		{E32}, F_XET, {0}, XRtina},
		{"/config/userctrl/A",					{CCTRL}, F_FND, {0}, NULL},
		{"/config/userctrl/A/color",		{E32}, F_XET, {0}, Xcolors},
		{"/config/userctrl/A/enc",				{CENC}, F_FND, {4}, NULL},
		{"/config/userctrl/A/enc/1",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/enc/2",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/enc/3",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/enc/4",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/btn",				{CENC}, F_FND, {8}, NULL},
		{"/config/userctrl/A/btn/5",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/btn/6",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/btn/7",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/btn/8",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/btn/9",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/btn/10",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/btn/11",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/A/btn/12",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B",					{CCTRL}, F_FND, {0}, NULL},
		{"/config/userctrl/B/color",		{E32}, F_XET, {0}, Xcolors},
		{"/config/userctrl/B/enc",				{CENC}, F_FND, {4}, NULL},
		{"/config/userctrl/B/enc/1",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/enc/2",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/enc/3",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/enc/4",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/btn",				{CENC}, F_FND, {8}, NULL},
		{"/config/userctrl/B/btn/5",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/btn/6",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/btn/7",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/btn/8",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/btn/9",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/btn/10",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/btn/11",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/B/btn/12",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C",					{CCTRL}, F_FND, {0}, NULL},
		{"/config/userctrl/C/color",		{E32}, F_XET, {0}, Xcolors},
		{"/config/userctrl/C/enc",				{CENC}, F_FND, {4}, NULL},
		{"/config/userctrl/C/enc/1",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/enc/2",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/enc/3",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/enc/4",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/btn",				{CENC}, F_FND, {8}, NULL},
		{"/config/userctrl/C/btn/5",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/btn/6",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/btn/7",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/btn/8",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/btn/9",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/btn/10",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/btn/11",		{S32}, F_XET, {0}, NULL},
		{"/config/userctrl/C/btn/12",		{S32}, F_XET, {0}, NULL},
		{"/config/tape",						{CTAPE}, F_FND, {0}, NULL},
		{"/config/tape/gainL",				{F32}, F_XET, {0}, NULL},
		{"/config/tape/gainR",				{F32}, F_XET, {0}, NULL},
		{"/config/tape/autoplay",			{E32}, F_XET, {0}, OffOn},
		{"/config/amixenable",					{CMIX}, F_FND, {0}, NULL},
		{"/config/amixenable/X",			{E32}, F_XET, {0}, OffOn},
		{"/config/amixenable/Y",			{E32}, F_XET, {0}, OffOn},
};
int 	Xconfig_max = sizeof(Xconfig) / sizeof(X32command);

X32command Xmain[] = {
		{"/main",							{BSCO}, F_FND, {0}, NULL},
		{"/main/st",						{BSCO}, F_FND, {0}, NULL},
		{"/main/st/config",					{BSCO}, F_FND, {0}, NULL},
		{"/main/st/config/name",		{S32}, F_XET, {0}, NULL},
		{"/main/st/config/icon",		{I32}, F_XET, {0}, NULL},
		{"/main/st/config/color",		{E32}, F_XET, {0}, Xcolors},
		{"/main/st/dyn",					{MXDY}, F_FND, {0}, NULL},
		{"/main/st/dyn/on",				{E32}, F_XET, {0}, OffOn},
		{"/main/st/dyn/mode",			{E32}, F_XET, {0}, Xdymode},
		{"/main/st/dyn/det",			{E32}, F_XET, {0}, Xdydet},
		{"/main/st/dyn/env",			{E32}, F_XET, {0}, Xdyenv},
		{"/main/st/dyn/thr",			{F32}, F_XET, {0}, NULL},
		{"/main/st/dyn/ratio",			{E32}, F_XET, {0}, Xdyrat},
		{"/main/st/dyn/knee",			{F32}, F_XET, {0}, NULL},
		{"/main/st/dyn/mgain",			{F32}, F_XET, {0}, NULL},
		{"/main/st/dyn/attack",			{F32}, F_XET, {0}, NULL},
		{"/main/st/dyn/hold",			{F32}, F_XET, {0}, NULL},
		{"/main/st/dyn/release",		{F32}, F_XET, {0}, NULL},
		{"/main/st/dyn/pos",			{E32}, F_XET, {0}, Xdyppos},
		{"/main/st/dyn/mix",			{F32}, F_XET, {0}, NULL},
		{"/main/st/dyn/auto",			{E32}, F_XET, {0}, OffOn},
		{"/main/st/dyn/filter",				{CHDF}, F_FND, {0}, NULL},
		{"/main/st/dyn/filter/on",		{E32}, F_XET, {0}, OffOn},
		{"/main/st/dyn/filter/type",	{E32}, F_XET, {0}, Xdyftyp},
		{"/main/st/dyn/filter/f",		{F32}, F_XET, {0}, NULL},
		{"/main/st/insert",					{CHIN}, F_FND, {0}, NULL},
		{"/main/st/insert/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/st/insert/pos",			{E32}, F_XET, {0}, Xdyppos},
		{"/main/st/insert/sel",			{E32}, F_XET, {0}, Xisel},
		{"/main/st/eq",						{OFFON}, F_FND, {1}, NULL},
		{"/main/st/eq/on",				{E32}, F_XET, {0}, OffOn},
		{"/main/st/eq/1",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/st/eq/1/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/st/eq/1/f",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/1/g",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/1/q",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/2",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/st/eq/2/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/st/eq/2/f",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/2/g",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/2/q",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/3",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/st/eq/3/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/st/eq/3/f",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/3/g",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/3/q",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/4",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/st/eq/4/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/st/eq/4/f",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/4/g",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/4/q",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/5",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/st/eq/5/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/st/eq/5/f",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/5/g",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/5/q",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/6",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/st/eq/6/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/st/eq/6/f",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/6/g",				{F32}, F_XET, {0}, NULL},
		{"/main/st/eq/6/q",				{F32}, F_XET, {0}, NULL},
		{"/main/st/mix",					{MSMX}, F_FND, {0}, NULL},
		{"/main/st/mix/on",				{E32}, F_XET, {0}, OffOn},
		{"/main/st/mix/fader",			{F32}, F_XET, {0}, NULL},
		{"/main/st/mix/pan",			{F32}, F_XET, {0}, NULL},
		{"/main/st/mix/01",					{CHMO}, F_FND, {0}, NULL},
		{"/main/st/mix/01/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/st/mix/01/level",		{F32}, F_XET, {0}, NULL},
		{"/main/st/mix/01/pan",			{F32}, F_XET, {0}, NULL},
		{"/main/st/mix/01/type",		{E32}, F_XET, {0}, Xmtype},
		{"/main/st/mix/02",					{CHME}, F_FND, {0}, NULL},
		{"/main/st/mix/02/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/st/mix/02/level",		{F32}, F_XET, {0}, NULL},
		{"/main/st/mix/03",					{CHMO}, F_FND, {0}, NULL},
		{"/main/st/mix/03/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/st/mix/03/level",		{F32}, F_XET, {0}, NULL},
		{"/main/st/mix/03/pan",			{F32}, F_XET, {0}, NULL},
		{"/main/st/mix/03/type",		{E32}, F_XET, {0}, Xmtype},
		{"/main/st/mix/04",					{CHME}, F_FND, {0}, NULL},
		{"/main/st/mix/04/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/st/mix/04/level",		{F32}, F_XET, {0}, NULL},
		{"/main/st/mix/05",					{CHMO}, F_FND, {0}, NULL},
		{"/main/st/mix/05/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/st/mix/05/level",		{F32}, F_XET, {0}, NULL},
		{"/main/st/mix/05/pan",			{F32}, F_XET, {0}, NULL},
		{"/main/st/mix/05/type",		{E32}, F_XET, {0}, Xmtype},
		{"/main/st/mix/06",					{CHME}, F_FND, {0}, NULL},
		{"/main/st/mix/06/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/st/mix/06/level",		{F32}, F_XET, {0}, NULL},

		{"/main/m",							{BSCO}, F_FND, {0}, NULL},
		{"/main/m/config",					{BSCO}, F_FND, {0}, NULL},
		{"/main/m/config/name",			{S32}, F_XET, {0}, NULL},
		{"/main/m/config/icon",			{I32}, F_XET, {0}, NULL},
		{"/main/m/config/color",		{E32}, F_XET, {0}, Xcolors},
		{"/main/m/dyn",						{MXDY}, F_FND, {0}, NULL},
		{"/main/m/dyn/on",				{E32}, F_XET, {0}, OffOn},
		{"/main/m/dyn/mode",			{E32}, F_XET, {0}, Xdymode},
		{"/main/m/dyn/det",				{E32}, F_XET, {0}, Xdydet},
		{"/main/m/dyn/env",				{E32}, F_XET, {0}, Xdyenv},
		{"/main/m/dyn/thr",				{F32}, F_XET, {0}, NULL},
		{"/main/m/dyn/ratio",			{E32}, F_XET, {0}, Xdyrat},
		{"/main/m/dyn/knee",			{F32}, F_XET, {0}, NULL},
		{"/main/m/dyn/mgain",			{F32}, F_XET, {0}, NULL},
		{"/main/m/dyn/attack",			{F32}, F_XET, {0}, NULL},
		{"/main/m/dyn/hold",			{F32}, F_XET, {0}, NULL},
		{"/main/m/dyn/release",			{F32}, F_XET, {0}, NULL},
		{"/main/m/dyn/pos",				{E32}, F_XET, {0}, Xdyppos},
		{"/main/m/dyn/mix",				{F32}, F_XET, {0}, NULL},
		{"/main/m/dyn/auto",			{E32}, F_XET, {0}, OffOn},
		{"/main/m/dyn/filter",				{CHDF}, F_FND, {0}, NULL},
		{"/main/m/dyn/filter/on",		{E32}, F_XET, {0}, OffOn},
		{"/main/m/dyn/filter/type",		{E32}, F_XET, {0}, Xdyftyp},
		{"/main/m/dyn/filter/f",		{F32}, F_XET, {0}, NULL},
		{"/main/m/insert",					{CHIN}, F_FND, {0}, NULL},
		{"/main/m/insert/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/m/insert/pos",			{E32}, F_XET, {0}, Xdyppos},
		{"/main/m/insert/sel",			{E32}, F_XET, {0}, Xisel},
		{"/main/m/eq",						{OFFON}, F_FND, {1}, NULL},
		{"/main/m/eq/on",				{E32}, F_XET, {0}, OffOn},
		{"/main/m/eq/1",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/m/eq/1/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/m/eq/1/f",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/1/g",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/1/q",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/2",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/m/eq/2/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/m/eq/2/f",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/2/g",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/2/q",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/3",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/m/eq/3/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/m/eq/3/f",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/3/g",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/3/q",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/4",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/m/eq/4/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/m/eq/4/f",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/4/g",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/4/q",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/5",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/m/eq/5/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/m/eq/5/f",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/5/g",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/5/q",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/6",					{CHEQ}, F_FND, {0}, NULL},
		{"/main/m/eq/6/type",			{E32}, F_XET, {0}, Xeqty2},
		{"/main/m/eq/6/f",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/6/g",				{F32}, F_XET, {0}, NULL},
		{"/main/m/eq/6/q",				{F32}, F_XET, {0}, NULL},
		{"/main/m/mix",						{CHME}, F_FND, {0}, NULL},
		{"/main/m/mix/on",				{E32}, F_XET, {0}, OffOn},
		{"/main/m/mix/fader",			{F32}, F_XET, {0}, NULL},
		{"/main/m/mix/01",					{CHMO}, F_FND, {0}, NULL},
		{"/main/m/mix/01/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/m/mix/01/level",		{F32}, F_XET, {0}, NULL},
		{"/main/m/mix/01/pan",			{F32}, F_XET, {0}, NULL},
		{"/main/m/mix/01/type",			{E32}, F_XET, {0}, Xmtype},
		{"/main/m/mix/02",					{CHME}, F_FND, {0}, NULL},
		{"/main/m/mix/02/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/m/mix/02/level",		{F32}, F_XET, {0}, NULL},
		{"/main/m/mix/03",					{CHMO}, F_FND, {0}, NULL},
		{"/main/m/mix/03/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/m/mix/03/level",		{F32}, F_XET, {0}, NULL},
		{"/main/m/mix/03/pan",			{F32}, F_XET, {0}, NULL},
		{"/main/m/mix/03/type",			{E32}, F_XET, {0}, Xmtype},
		{"/main/m/mix/04",					{CHME}, F_FND, {0}, NULL},
		{"/main/m/mix/04/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/m/mix/04/level",		{F32}, F_XET, {0}, NULL},
		{"/main/m/mix/05",					{CHMO}, F_FND, {0}, NULL},
		{"/main/m/mix/05/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/m/mix/05/level",		{F32}, F_XET, {0}, NULL},
		{"/main/m/mix/05/pan",			{F32}, F_XET, {0}, NULL},
		{"/main/m/mix/05/type",			{E32}, F_XET, {0}, Xmtype},
		{"/main/m/mix/06",					{CHME}, F_FND, {0}, NULL},
		{"/main/m/mix/06/on",			{E32}, F_XET, {0}, OffOn},
		{"/main/m/mix/06/level",		{F32}, F_XET, {0}, NULL},
};
int Xmain_max = sizeof(Xmain) / sizeof(X32command);



#endif /* X32CFGMAIN_H_ */
