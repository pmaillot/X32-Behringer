/*
 * X32PrefStat.h
 *
 *  Created on: 4 févr. 2015
 *      Author: patrick
 */

#ifndef X32PREFSTAT_H_
#define X32PREFSTAT_H_


X32command Xprefs[] = {
		{"/-prefs",							{PREFS}, F_FND, {0}},
		{"/-prefs/style",				{S32}, F_XET, {0}},
		{"/-prefs/bright",				{F32}, F_XET, {0}},
		{"/-prefs/lcdcont",				{F32}, F_XET, {0}},
		{"/-prefs/ledbright",			{F32}, F_XET, {0}},
		{"/-prefs/lamp",				{F32}, F_XET, {0}},
		{"/-prefs/lampon",				{I32}, F_XET, {0}},
		{"/-prefs/clockrate",			{I32}, F_XET, {0}},
		{"/-prefs/clocksource",			{I32}, F_XET, {0}},
		{"/-prefs/confirm_general",		{I32}, F_XET, {0}},
		{"/-prefs/confirm_overwrite",	{I32}, F_XET, {0}},
		{"/-prefs/confirm_sceneload",	{I32}, F_XET, {0}},
		{"/-prefs/viewrtn",				{I32}, F_XET, {0}},
		{"/-prefs/selfollowsbank",		{I32}, F_XET, {0}},
		{"/-prefs/sceneadvance",		{I32}, F_XET, {0}},
		{"/-prefs/safe_masterlevels",	{I32}, F_XET, {0}},
		{"/-prefs/haflags",				{I32}, F_XET, {0}},
		{"/-prefs/autosel",				{I32}, F_XET, {0}},
		{"/-prefs/show_control",		{I32}, F_XET, {0}},
		{"/-prefs/clockmode",			{I32}, F_XET, {0}},
		{"/-prefs/hardmute",			{I32}, F_XET, {0}},
		{"/-prefs/dcamute",				{I32}, F_XET, {0}},
		{"/-prefs/invertmutes",			{I32}, F_XET, {0}},
		{"/-prefs/fwver",				{S32}, F_XET, {0}},		// TODO: fix incorrect command

		{"/-prefs/remote",					{PIR}, F_FND, {0}},
		{"/-prefs/remote/enable",		{I32}, F_XET, {0}},
		{"/-prefs/remote/protocol",		{I32}, F_XET, {0}},
		{"/-prefs/remote/port",			{I32}, F_XET, {0}},
		{"/-prefs/remote/ioenable",		{I32}, F_XET, {0}},

		{"/-prefs/iQ",						{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/01",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/01/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/01/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/01/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/02",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/02/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/02/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/02/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/03",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/03/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/03/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/03/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/04",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/04/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/04/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/04/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/05",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/05/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/05/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/05/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/06",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/06/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/06/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/06/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/07",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/07/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/07/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/07/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/08",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/08/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/08/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/08/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/09",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/09/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/09/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/09/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/10",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/10/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/10/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/10/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/11",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/11/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/11/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/11/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/12",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/12/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/12/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/12/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/13",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/13/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/13/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/13/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/14",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/14/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/14/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/14/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/15",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/15/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/15/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/15/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/iQ/16",					{PIQ}, F_FND, {0}},
		{"/-prefs/iQ/16/iQmodel",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/16/IQeqset",		{I32}, F_XET, {0}},
		{"/-prefs/iQ/16/IQsound",		{I32}, F_XET, {0}},

		{"/-prefs/card",					{PCARD}, F_FND, {0}},
		{"/-prefs/card/UFifc",			{I32}, F_XET, {0}},
		{"/-prefs/card/UFmode",			{I32}, F_XET, {0}},
		{"/-prefs/card/USBmode",		{I32}, F_XET, {0}},
		{"/-prefs/card/ADATwc",			{I32}, F_XET, {0}},
		{"/-prefs/card/ADATsync",		{I32}, F_XET, {0}},
		{"/-prefs/card/MADImode",		{I32}, F_XET, {0}},
		{"/-prefs/card/MADIin",			{I32}, F_XET, {0}},
		{"/-prefs/card/MADIout",		{I32}, F_XET, {0}},
		{"/-prefs/card/MADIsrc",		{I32}, F_XET, {0}},

		{"/-prefs/rta",						{PRTA}, F_FND, {0}},
		{"/-prefs/rta/visibility",		{I32}, F_XET, {0}},
		{"/-prefs/rta/gain",			{F32}, F_XET, {0}},
		{"/-prefs/rta/autogain",		{I32}, F_XET, {0}},
		{"/-prefs/rta/source",			{I32}, F_XET, {0}},
		{"/-prefs/rta/pos",				{I32}, F_XET, {0}},
		{"/-prefs/rta/mode",			{I32}, F_XET, {0}},
		{"/-prefs/rta/option",			{I32}, F_XET, {0}},
		{"/-prefs/rta/det",				{I32}, F_XET, {0}},
		{"/-prefs/rta/decay",			{F32}, F_XET, {0}},
		{"/-prefs/rta/peakhold",		{I32}, F_XET, {0}},

		{"/-prefs/ip",						{PIP}, F_FND, {0}},
		{"/-prefs/ip/dhcp",				{I32}, F_XET, {0}},

		{"/-prefs/ip/addr",					{PADDR}, F_FND, {0}},
		{"/-prefs/ip/addr/0",			{I32}, F_XET, {0}},
		{"/-prefs/ip/addr/1",			{I32}, F_XET, {0}},
		{"/-prefs/ip/addr/2",			{I32}, F_XET, {0}},
		{"/-prefs/ip/addr/3",			{I32}, F_XET, {0}},


		{"/-prefs/ip/mask",					{PADDR}, F_FND, {0}},
		{"/-prefs/ip/mask/0",			{I32}, F_XET, {0}},
		{"/-prefs/ip/mask/1",			{I32}, F_XET, {0}},
		{"/-prefs/ip/mask/2",			{I32}, F_XET, {0}},
		{"/-prefs/ip/mask/3",			{I32}, F_XET, {0}},


		{"/-prefs/ip/gateway",				{PADDR}, F_FND, {0}},
		{"/-prefs/ip/gateway/0",		{I32}, F_XET, {0}},
		{"/-prefs/ip/gateway/1",		{I32}, F_XET, {0}},
		{"/-prefs/ip/gateway/2",		{I32}, F_XET, {0}},
		{"/-prefs/ip/gateway/3",		{I32}, F_XET, {0}},

};
int Xprefs_max = sizeof(Xprefs) / sizeof(X32command);

char*		PCsource[] = {" INT", " AES50A", " AES50B", " Exp Card"};
char*		PSCont[] = {" CUES", " SCENES", " SNIPPETS"};
char*		PRpro[] = {" MC", " HUI", " CC"};
char*		PRport[] = {" MIDI", " CARD", " RTP"};
char*		PiQmodel[] = {" none", " iQ8", " iQ10", " iQ12", " iQ15", "iQ15B", "iQ18B"};
char*		PiQeqset[] = {" Linear", " Live", " Speech", " Playback", " User"};
char*		Pctype[] = {" FW", " USB", " unk", " unk", " unk", " unk"};
char*		Pcmode[] = {" 32/32", " 16/16", " 32/8", " 8/32", " 8/8", " 2/2"};
char*		Pcas[] = {" WC", " ADAT1", " ADAT2", " ADAT3", " ADAT4"};
char*		Pcmadio[] = {" 1-32", " 9-40", " 17-48", " 25-56", " 33-64"};
char*		Pcmadsrc[] = {" OFF", " OPT", " COAX", " BOTH"};
char*		Prtavis[] = {" OFF", " 25%", " 30%", " 35%", " 40%", "45%", "50%", " 55%",
						 " 60%", "65%", "70%", "75%", "80%"};
char*		Prtaph[] = {" OFF", " 1", " 2", " 3", " 4", "5", "6", " 7", "8"};



X32command Xstat[] = {
		{"/-stat",						{STAT}, F_FND, {0}},
		{"/-stat/selidx",			{I32}, F_XET, {0}},
		{"/-stat/chfaderbank",		{I32}, F_XET, {0}},
		{"/-stat/grpfaderbank",		{I32}, F_XET, {0}},
		{"/-stat/sendsonfader",		{I32}, F_XET, {0}},
		{"/-stat/bussendbank",		{I32}, F_XET, {0}},
		{"/-stat/eqband",			{I32}, F_XET, {0}},
		{"/-stat/solo",				{I32}, F_XET, {0}},
		{"/-stat/keysolo",			{I32}, F_XET, {0}},
		{"/-stat/userbank",			{I32}, F_XET, {0}},
		{"/-stat/autosave",			{I32}, F_XET, {0}},
		{"/-stat/lock",				{I32}, F_XET, {0}},
		{"/-stat/usbmounted",		{I32}, F_XET, {0}},
		{"/-stat/remote",			{I32}, F_XET, {0}},
		{"/-stat/rtamodeeq",		{I32}, F_XET, {0}},
		{"/-stat/rtamodegeq",		{I32}, F_XET, {0}},
		{"/-stat/rtaeqpre",			{I32}, F_XET, {0}},
		{"/-stat/rtageqpost",		{I32}, F_XET, {0}},
		{"/-stat/rtasource",		{I32}, F_XET, {0}},
		{"/-stat/xcardtype",		{I32}, F_XET, {0}},
		{"/-stat/xcardsync",		{I32}, F_XET, {0}},
		{"/-stat/geqonfdr",			{I32}, F_XET, {0}},
		{"/-stat/geqpos",			{I32}, F_XET, {0}},

		{"/-stat/screen",					{SSCREEN}, F_FND, {0}},
		{"/-stat/screen/screen",		{I32}, F_XET, {0}},
		{"/-stat/screen/mutegrp",		{I32}, F_XET, {0}},
		{"/-stat/screen/utils",			{I32}, F_XET, {0}},
//
		{"/-stat/screen/CHAN",				{SCHA}, F_FND, {0}},
		{"/-stat/screen/CHAN/page",		{I32}, F_XET, {0}},

		{"/-stat/screen/METER",				{SMET}, F_FND, {0}},
		{"/-stat/screen/METER/page",	{I32}, F_XET, {0}},

		{"/-stat/screen/ROUTE",				{SROU}, F_FND, {0}},
		{"/-stat/screen/ROUTE/page",	{I32}, F_XET, {0}},

		{"/-stat/screen/SETUP",				{SSET}, F_FND, {0}},
		{"/-stat/screen/SETUP/page",	{I32}, F_XET, {0}},

		{"/-stat/screen/LIB",				{SLIB}, F_FND, {0}},
		{"/-stat/screen/LIB/page",		{I32}, F_XET, {0}},

		{"/-stat/screen/FX",				{SFX}, F_FND, {0}},
		{"/-stat/screen/FX/page",		{I32}, F_XET, {0}},

		{"/-stat/screen/MON",				{SMON}, F_FND, {0}},
		{"/-stat/screen/MON/page",		{I32}, F_XET, {0}},

		{"/-stat/screen/USB",				{SUSB}, F_FND, {0}},
		{"/-stat/screen/USB/page",		{I32}, F_XET, {0}},

		{"/-stat/screen/SCENE",			{SSCE}, F_FND, {0}},
		{"/-stat/screen/SCENE/page",	{I32}, F_XET, {0}},

		{"/-stat/screen/ASSIGN",			{SASS}, F_FND, {0}},
		{"/-stat/screen/ASSIGN/page",	{I32}, F_XET, {0}},

		{"/-stat/solosw",			{OFFON}, F_FND, {80}},
		{"/-stat/solosw/01",	{I32}, F_XET, {0}},
		{"/-stat/solosw/02",	{I32}, F_XET, {0}},
		{"/-stat/solosw/03",	{I32}, F_XET, {0}},
		{"/-stat/solosw/04",	{I32}, F_XET, {0}},
		{"/-stat/solosw/05",	{I32}, F_XET, {0}},
		{"/-stat/solosw/06",	{I32}, F_XET, {0}},
		{"/-stat/solosw/07",	{I32}, F_XET, {0}},
		{"/-stat/solosw/08",	{I32}, F_XET, {0}},
		{"/-stat/solosw/09",	{I32}, F_XET, {0}},
		{"/-stat/solosw/10",	{I32}, F_XET, {0}},
		{"/-stat/solosw/11",	{I32}, F_XET, {0}},
		{"/-stat/solosw/12",	{I32}, F_XET, {0}},
		{"/-stat/solosw/13",	{I32}, F_XET, {0}},
		{"/-stat/solosw/14",	{I32}, F_XET, {0}},
		{"/-stat/solosw/15",	{I32}, F_XET, {0}},
		{"/-stat/solosw/16",	{I32}, F_XET, {0}},
		{"/-stat/solosw/17",	{I32}, F_XET, {0}},
		{"/-stat/solosw/18",	{I32}, F_XET, {0}},
		{"/-stat/solosw/19",	{I32}, F_XET, {0}},
		{"/-stat/solosw/20",	{I32}, F_XET, {0}},
		{"/-stat/solosw/21",	{I32}, F_XET, {0}},
		{"/-stat/solosw/22",	{I32}, F_XET, {0}},
		{"/-stat/solosw/23",	{I32}, F_XET, {0}},
		{"/-stat/solosw/24",	{I32}, F_XET, {0}},
		{"/-stat/solosw/25",	{I32}, F_XET, {0}},
		{"/-stat/solosw/26",	{I32}, F_XET, {0}},
		{"/-stat/solosw/27",	{I32}, F_XET, {0}},
		{"/-stat/solosw/28",	{I32}, F_XET, {0}},
		{"/-stat/solosw/29",	{I32}, F_XET, {0}},
		{"/-stat/solosw/30",	{I32}, F_XET, {0}},
		{"/-stat/solosw/31",	{I32}, F_XET, {0}},
		{"/-stat/solosw/32",	{I32}, F_XET, {0}},
		{"/-stat/solosw/33",	{I32}, F_XET, {0}},
		{"/-stat/solosw/34",	{I32}, F_XET, {0}},
		{"/-stat/solosw/35",	{I32}, F_XET, {0}},
		{"/-stat/solosw/36",	{I32}, F_XET, {0}},
		{"/-stat/solosw/37",	{I32}, F_XET, {0}},
		{"/-stat/solosw/38",	{I32}, F_XET, {0}},
		{"/-stat/solosw/39",	{I32}, F_XET, {0}},
		{"/-stat/solosw/40",	{I32}, F_XET, {0}},
		{"/-stat/solosw/41",	{I32}, F_XET, {0}},
		{"/-stat/solosw/42",	{I32}, F_XET, {0}},
		{"/-stat/solosw/43",	{I32}, F_XET, {0}},
		{"/-stat/solosw/44",	{I32}, F_XET, {0}},
		{"/-stat/solosw/45",	{I32}, F_XET, {0}},
		{"/-stat/solosw/46",	{I32}, F_XET, {0}},
		{"/-stat/solosw/47",	{I32}, F_XET, {0}},
		{"/-stat/solosw/48",	{I32}, F_XET, {0}},
		{"/-stat/solosw/49",	{I32}, F_XET, {0}},
		{"/-stat/solosw/50",	{I32}, F_XET, {0}},
		{"/-stat/solosw/51",	{I32}, F_XET, {0}},
		{"/-stat/solosw/52",	{I32}, F_XET, {0}},
		{"/-stat/solosw/53",	{I32}, F_XET, {0}},
		{"/-stat/solosw/54",	{I32}, F_XET, {0}},
		{"/-stat/solosw/55",	{I32}, F_XET, {0}},
		{"/-stat/solosw/56",	{I32}, F_XET, {0}},
		{"/-stat/solosw/57",	{I32}, F_XET, {0}},
		{"/-stat/solosw/58",	{I32}, F_XET, {0}},
		{"/-stat/solosw/59",	{I32}, F_XET, {0}},
		{"/-stat/solosw/60",	{I32}, F_XET, {0}},
		{"/-stat/solosw/61",	{I32}, F_XET, {0}},
		{"/-stat/solosw/62",	{I32}, F_XET, {0}},
		{"/-stat/solosw/63",	{I32}, F_XET, {0}},
		{"/-stat/solosw/64",	{I32}, F_XET, {0}},
		{"/-stat/solosw/65",	{I32}, F_XET, {0}},
		{"/-stat/solosw/66",	{I32}, F_XET, {0}},
		{"/-stat/solosw/67",	{I32}, F_XET, {0}},
		{"/-stat/solosw/68",	{I32}, F_XET, {0}},
		{"/-stat/solosw/69",	{I32}, F_XET, {0}},
		{"/-stat/solosw/70",	{I32}, F_XET, {0}},
		{"/-stat/solosw/71",	{I32}, F_XET, {0}},
		{"/-stat/solosw/72",	{I32}, F_XET, {0}},
		{"/-stat/solosw/73",	{I32}, F_XET, {0}},
		{"/-stat/solosw/74",	{I32}, F_XET, {0}},
		{"/-stat/solosw/75",	{I32}, F_XET, {0}},
		{"/-stat/solosw/76",	{I32}, F_XET, {0}},
		{"/-stat/solosw/77",	{I32}, F_XET, {0}},
		{"/-stat/solosw/78",	{I32}, F_XET, {0}},
		{"/-stat/solosw/79",	{I32}, F_XET, {0}},
		{"/-stat/solosw/80",	{I32}, F_XET, {0}},

		{"/-stat/aes50",				{SAES}, F_FND, {0}},
		{"/-stat/aes50/A",			{S32}, F_XET, {0}},
		{"/-stat/aes50/B",			{S32}, F_XET, {0}},
		{"/-stat/aes50/state",		{I32}, F_XET, {0}},

		{"/-stat/tape",					{STAPE}, F_FND, {0}},
		{"/-stat/tape/state",		{I32}, F_XET, {0}},
		{"/-stat/tape/file",		{S32}, F_XET, {0}},
		{"/-stat/tape/etime",		{I32}, F_XET, {0}},
		{"/-stat/tape/rtime",		{I32}, F_XET, {0}},

		{"/-stat/osc",					{SOSC}, F_FND, {0}},
		{"/-stat/osc",				{I32}, F_XET, {0}},

};
int 	Xstat_max = sizeof(Xstat) / sizeof(X32command);

char*	Sselidx[] = {" Ch01", " Ch02", " Ch03", " Ch04", " Ch05", " Ch06", " Ch07", " Ch08",
					 " Ch09", " Ch10", " Ch11", " Ch12", " Ch13", " Ch14", " Ch15", " Ch16",
					 " Ch17", " Ch18", " Ch19", " Ch20", " Ch21", " Ch22", " Ch23", " Ch24",
					 " Ch25", " Ch26", " Ch27", " Ch28", " Ch29", " Ch30", " Ch31", " Ch32",
					 " Aux1", " Aux2", " Aux3", " Aux4", " Aux5", " Aux6", " Aux7", " Aux8",
					 " Fx1L", " Fx1R", " Fx2L", " Fx2R", " Fx3L", " Fx3R", " Fx4L", " Fx4R",
					 " Bus1", " Bus2", " Bus3", " Bus4", " Bus5", " Bus6", " Bus7", " Bus8",
					 " Bus9", " Bs10", " Bs11", " Bs12", " Bs13", " Bs14", " Bs15", " Bs16",
					 " Mtx1", " Mtx2", " Mtx3", " Mtx4", " Mtx5", " Mtx6", " M/C", " LR"};
char*	Sscrn[] = {" CHAN", " METERS", " ROUTE", " SETUP", " LIB", " FX",
				   " MON", " USB", " SCENE", " ASSIGN", " LOCK"};
char*	Schal[] = {" HOME", " CONFIG", " GATE", " DYN", " EQ", "MIX", " MAIN"};
char*	Smetl[] = {" CHANNEL", " MIXBUS", " AUX/FX", " IN/OUT", " RTA"};
char*	Sroul[] = {" HOME", " ANAOUT", " AUXOUT", " P16OUT", " CARDOUT", "AES50A", " AES50B", "XLROUT"};
char*	Ssetl[] = {" GLOB", " CONF", " REMOTE", " NETW", "NAMES", "PREAMPS", " CARD"};
char*	Slibl[] = {" CHAN", " EFFECT", " ROUTE"};
char*	Sfxl[] = {" HOME", " FX1", " FX2", " FX3", "FX4", "FX5", " FX6", " FX7", " FX8"};
char*	Smonl[] = {" MONITOR", " TALKA", " TALKB", " OSC"};
char*	Susbl[] = {" HOME", " CONFIG"};
char*	Sscel[] = {" HOME", " SCENES", " BITS", " PARSAFE", "CHNSAFE", "MIDI"};
char*	Sassl[] = {" HOME", " SETA", " SETB", " SETC"};
char*	Stapl[] = {" STOP", " PPAUSE", " PLAY", " RPAUSE", "RECORD", "FF", "REW"};


X32command Xaction[] = {
		{"-action",						{I32}, F_FND, {0}},
		{"-action/setip",			{I32}, F_XET, {0}},
		{"-action/setclock",		{S32}, F_XET, {0}},
		{"-action/initall",			{I32}, F_XET, {0}},
		{"-action/initlib",			{I32}, F_XET, {0}},
		{"-action/initshow",		{I32}, F_XET, {0}},
		{"-action/savestate",		{I32}, F_XET, {0}},
		{"-action/undopt",			{I32}, F_XET, {0}},
		{"-action/doundo",			{I32}, F_XET, {0}},
		{"-action/platrack",		{I32}, F_XET, {0}},
		{"-action/newscreen",		{I32}, F_XET, {0}},
		{"-action/clearsolo",		{I32}, F_XET, {0}},
		{"-action/setprebus",		{I32}, F_XET, {0}},
		{"-action/setsrate",		{I32}, F_XET, {0}},
		{"-action/setrtasrc",		{I32}, F_XET, {0}},
		{"-action/newscreen",		{I32}, F_XET, {0}},
		{"-action/recselect",		{I32}, F_XET, {0}},
		{"-action/gocue",			{I32}, F_XET, {0}},
		{"-action/goscene",			{I32}, F_XET, {0}},
		{"-action/undopt",			{I32}, F_XET, {0}},
		{"-action/gosnippet",		{I32}, F_XET, {0}},

};
int 	Xaction_max = sizeof(Xaction) / sizeof(X32command);
#endif /* X32PREFSTAT_H_ */
