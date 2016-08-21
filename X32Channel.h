/*
 * X32Channel.h
 *
 *  Created on: 4 févr. 2015
 *      Author: patrick
 */

#ifndef X32CHANNEL_H_
#define X32CHANNEL_H_


X32command Xchannel01[] = {
		{"/ch",							{CHCO}, F_FND, {0}},
		{"/ch/01",						{CHCO}, F_FND, {0}},
		{"/ch/01/config",				{CHCO}, F_FND, {0}},
		{"/ch/01/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/01/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/01/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/01/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/01/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/01/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/01/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/01/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/01/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/01/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/01/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/01/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/01/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/01/delay",				{CHDE}, F_FND, {0}},
		{"/ch/01/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/01/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/01/insert",				{CHIN}, F_FND, {0}},
		{"/ch/01/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/01/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/01/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/01/gate",					{CHGA}, F_FND, {0}},
		{"/ch/01/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/01/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/01/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/01/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/01/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/01/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/01/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/01/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/01/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/01/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/01/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/01/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/01/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/01/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/01/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/01/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/01/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/01/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/01/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/01/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/01/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/01/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/01/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/01/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/01/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/01/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/01/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/01/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/01/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/01/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/01/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/01/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/01/eq",					{OFFON}, F_FND, {1}},
		{"/ch/01/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/01/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/01/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/01/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/01/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/01/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/01/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/01/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/01/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/01/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/01/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/01/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/01/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/01/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/01/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/01/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/01/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/01/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/01/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/01/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/01/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/01/mix",					{CHMX}, F_FND, {0}},
		{"/ch/01/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/01/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/01/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/01/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/01/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/01/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/01/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/01/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/01/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/01/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/01/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/01/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/01/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/01/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/01/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/01/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/01/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/01/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/01/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/01/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/01/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/01/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/01/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/01/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/01/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/01/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/01/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/01/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/01/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/01/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/01/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/01/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/01/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/01/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/01/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/01/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/01/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/01/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/01/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/01/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/01/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/01/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/01/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/01/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/01/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/01/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/01/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/01/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/01/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/01/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/01/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/01/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/01/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/01/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/01/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/01/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/01/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/01/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/01/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/01/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/01/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/01/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/01/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/01/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/01/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/01/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/01/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/01/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/01/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/01/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel01_max = sizeof(Xchannel01) / sizeof(X32command);


X32command Xchannel02[] = {
		{"/ch/02",						{CHCO}, F_FND, {0}},
		{"/ch/02/config",				{CHCO}, F_FND, {0}},
		{"/ch/02/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/02/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/02/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/02/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/02/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/02/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/02/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/02/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/02/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/02/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/02/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/02/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/02/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/02/delay",				{CHDE}, F_FND, {0}},
		{"/ch/02/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/02/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/02/insert",				{CHIN}, F_FND, {0}},
		{"/ch/02/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/02/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/02/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/02/gate",					{CHGA}, F_FND, {0}},
		{"/ch/02/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/02/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/02/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/02/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/02/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/02/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/02/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/02/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/02/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/02/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/02/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/02/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/02/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/02/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/02/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/02/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/02/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/02/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/02/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/02/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/02/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/02/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/02/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/02/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/02/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/02/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/02/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/02/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/02/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/02/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/02/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/02/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/02/eq",					{OFFON}, F_FND, {1}},
		{"/ch/02/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/02/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/02/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/02/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/02/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/02/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/02/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/02/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/02/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/02/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/02/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/02/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/02/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/02/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/02/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/02/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/02/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/02/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/02/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/02/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/02/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/02/mix",					{CHMX}, F_FND, {0}},
		{"/ch/02/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/02/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/02/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/02/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/02/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/02/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/02/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/02/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/02/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/02/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/02/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/02/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/02/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/02/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/02/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/02/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/02/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/02/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/02/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/02/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/02/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/02/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/02/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/02/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/02/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/02/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/02/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/02/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/02/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/02/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/02/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/02/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/02/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/02/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/02/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/02/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/02/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/02/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/02/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/02/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/02/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/02/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/02/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/02/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/02/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/02/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/02/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/02/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/02/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/02/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/02/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/02/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/02/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/02/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/02/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/02/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/02/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/02/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/02/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/02/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/02/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/02/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/02/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/02/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/02/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/02/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/02/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/02/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/02/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/02/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel02_max = sizeof(Xchannel02) / sizeof(X32command);


X32command Xchannel03[] = {
		{"/ch/03",						{CHCO}, F_FND, {0}},
		{"/ch/03/config",				{CHCO}, F_FND, {0}},
		{"/ch/03/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/03/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/03/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/03/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/03/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/03/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/03/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/03/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/03/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/03/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/03/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/03/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/03/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/03/delay",				{CHDE}, F_FND, {0}},
		{"/ch/03/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/03/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/03/insert",				{CHIN}, F_FND, {0}},
		{"/ch/03/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/03/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/03/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/03/gate",					{CHGA}, F_FND, {0}},
		{"/ch/03/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/03/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/03/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/03/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/03/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/03/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/03/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/03/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/03/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/03/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/03/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/03/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/03/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/03/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/03/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/03/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/03/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/03/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/03/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/03/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/03/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/03/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/03/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/03/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/03/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/03/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/03/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/03/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/03/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/03/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/03/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/03/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/03/eq",					{OFFON}, F_FND, {1}},
		{"/ch/03/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/03/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/03/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/03/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/03/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/03/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/03/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/03/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/03/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/03/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/03/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/03/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/03/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/03/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/03/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/03/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/03/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/03/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/03/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/03/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/03/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/03/mix",					{CHMX}, F_FND, {0}},
		{"/ch/03/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/03/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/03/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/03/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/03/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/03/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/03/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/03/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/03/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/03/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/03/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/03/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/03/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/03/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/03/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/03/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/03/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/03/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/03/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/03/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/03/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/03/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/03/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/03/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/03/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/03/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/03/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/03/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/03/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/03/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/03/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/03/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/03/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/03/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/03/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/03/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/03/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/03/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/03/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/03/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/03/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/03/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/03/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/03/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/03/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/03/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/03/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/03/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/03/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/03/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/03/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/03/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/03/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/03/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/03/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/03/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/03/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/03/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/03/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/03/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/03/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/03/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/03/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/03/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/03/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/03/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/03/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/03/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/03/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/03/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel03_max = sizeof(Xchannel03) / sizeof(X32command);


X32command Xchannel04[] = {
		{"/ch/04",						{CHCO}, F_FND, {0}},
		{"/ch/04/config",				{CHCO}, F_FND, {0}},
		{"/ch/04/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/04/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/04/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/04/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/04/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/04/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/04/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/04/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/04/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/04/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/04/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/04/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/04/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/04/delay",				{CHDE}, F_FND, {0}},
		{"/ch/04/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/04/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/04/insert",				{CHIN}, F_FND, {0}},
		{"/ch/04/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/04/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/04/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/04/gate",					{CHGA}, F_FND, {0}},
		{"/ch/04/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/04/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/04/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/04/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/04/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/04/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/04/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/04/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/04/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/04/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/04/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/04/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/04/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/04/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/04/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/04/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/04/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/04/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/04/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/04/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/04/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/04/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/04/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/04/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/04/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/04/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/04/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/04/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/04/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/04/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/04/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/04/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/04/eq",					{OFFON}, F_FND, {1}},
		{"/ch/04/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/04/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/04/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/04/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/04/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/04/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/04/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/04/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/04/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/04/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/04/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/04/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/04/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/04/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/04/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/04/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/04/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/04/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/04/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/04/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/04/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/04/mix",					{CHMX}, F_FND, {0}},
		{"/ch/04/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/04/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/04/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/04/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/04/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/04/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/04/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/04/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/04/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/04/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/04/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/04/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/04/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/04/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/04/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/04/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/04/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/04/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/04/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/04/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/04/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/04/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/04/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/04/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/04/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/04/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/04/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/04/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/04/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/04/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/04/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/04/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/04/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/04/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/04/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/04/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/04/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/04/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/04/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/04/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/04/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/04/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/04/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/04/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/04/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/04/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/04/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/04/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/04/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/04/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/04/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/04/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/04/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/04/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/04/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/04/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/04/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/04/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/04/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/04/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/04/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/04/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/04/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/04/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/04/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/04/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/04/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/04/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/04/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/04/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel04_max = sizeof(Xchannel04) / sizeof(X32command);


X32command Xchannel05[] = {
		{"/ch/05",						{CHCO}, F_FND, {0}},
		{"/ch/05/config",				{CHCO}, F_FND, {0}},
		{"/ch/05/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/05/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/05/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/05/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/05/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/05/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/05/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/05/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/05/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/05/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/05/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/05/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/05/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/05/delay",				{CHDE}, F_FND, {0}},
		{"/ch/05/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/05/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/05/insert",				{CHIN}, F_FND, {0}},
		{"/ch/05/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/05/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/05/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/05/gate",					{CHGA}, F_FND, {0}},
		{"/ch/05/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/05/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/05/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/05/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/05/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/05/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/05/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/05/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/05/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/05/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/05/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/05/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/05/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/05/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/05/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/05/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/05/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/05/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/05/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/05/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/05/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/05/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/05/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/05/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/05/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/05/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/05/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/05/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/05/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/05/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/05/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/05/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/05/eq",					{OFFON}, F_FND, {1}},
		{"/ch/05/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/05/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/05/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/05/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/05/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/05/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/05/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/05/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/05/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/05/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/05/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/05/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/05/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/05/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/05/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/05/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/05/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/05/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/05/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/05/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/05/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/05/mix",					{CHMX}, F_FND, {0}},
		{"/ch/05/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/05/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/05/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/05/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/05/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/05/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/05/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/05/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/05/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/05/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/05/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/05/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/05/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/05/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/05/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/05/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/05/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/05/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/05/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/05/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/05/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/05/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/05/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/05/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/05/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/05/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/05/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/05/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/05/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/05/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/05/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/05/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/05/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/05/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/05/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/05/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/05/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/05/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/05/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/05/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/05/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/05/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/05/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/05/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/05/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/05/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/05/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/05/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/05/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/05/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/05/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/05/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/05/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/05/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/05/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/05/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/05/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/05/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/05/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/05/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/05/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/05/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/05/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/05/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/05/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/05/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/05/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/05/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/05/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/05/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel05_max = sizeof(Xchannel05) / sizeof(X32command);


X32command Xchannel06[] = {
		{"/ch/06",						{CHCO}, F_FND, {0}},
		{"/ch/06/config",				{CHCO}, F_FND, {0}},
		{"/ch/06/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/06/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/06/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/06/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/06/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/06/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/06/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/06/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/06/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/06/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/06/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/06/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/06/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/06/delay",				{CHDE}, F_FND, {0}},
		{"/ch/06/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/06/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/06/insert",				{CHIN}, F_FND, {0}},
		{"/ch/06/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/06/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/06/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/06/gate",					{CHGA}, F_FND, {0}},
		{"/ch/06/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/06/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/06/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/06/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/06/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/06/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/06/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/06/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/06/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/06/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/06/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/06/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/06/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/06/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/06/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/06/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/06/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/06/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/06/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/06/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/06/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/06/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/06/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/06/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/06/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/06/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/06/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/06/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/06/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/06/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/06/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/06/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/06/eq",					{OFFON}, F_FND, {1}},
		{"/ch/06/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/06/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/06/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/06/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/06/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/06/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/06/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/06/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/06/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/06/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/06/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/06/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/06/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/06/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/06/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/06/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/06/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/06/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/06/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/06/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/06/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/06/mix",					{CHMX}, F_FND, {0}},
		{"/ch/06/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/06/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/06/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/06/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/06/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/06/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/06/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/06/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/06/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/06/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/06/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/06/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/06/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/06/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/06/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/06/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/06/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/06/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/06/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/06/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/06/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/06/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/06/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/06/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/06/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/06/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/06/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/06/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/06/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/06/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/06/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/06/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/06/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/06/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/06/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/06/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/06/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/06/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/06/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/06/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/06/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/06/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/06/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/06/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/06/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/06/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/06/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/06/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/06/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/06/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/06/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/06/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/06/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/06/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/06/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/06/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/06/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/06/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/06/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/06/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/06/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/06/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/06/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/06/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/06/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/06/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/06/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/06/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/06/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/06/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel06_max = sizeof(Xchannel06) / sizeof(X32command);


X32command Xchannel07[] = {
		{"/ch/07",						{CHCO}, F_FND, {0}},
		{"/ch/07/config",				{CHCO}, F_FND, {0}},
		{"/ch/07/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/07/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/07/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/07/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/07/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/07/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/07/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/07/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/07/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/07/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/07/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/07/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/07/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/07/delay",				{CHDE}, F_FND, {0}},
		{"/ch/07/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/07/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/07/insert",				{CHIN}, F_FND, {0}},
		{"/ch/07/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/07/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/07/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/07/gate",					{CHGA}, F_FND, {0}},
		{"/ch/07/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/07/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/07/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/07/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/07/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/07/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/07/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/07/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/07/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/07/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/07/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/07/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/07/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/07/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/07/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/07/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/07/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/07/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/07/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/07/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/07/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/07/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/07/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/07/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/07/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/07/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/07/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/07/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/07/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/07/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/07/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/07/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/07/eq",					{OFFON}, F_FND, {1}},
		{"/ch/07/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/07/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/07/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/07/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/07/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/07/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/07/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/07/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/07/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/07/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/07/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/07/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/07/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/07/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/07/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/07/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/07/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/07/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/07/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/07/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/07/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/07/mix",					{CHMX}, F_FND, {0}},
		{"/ch/07/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/07/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/07/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/07/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/07/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/07/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/07/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/07/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/07/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/07/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/07/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/07/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/07/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/07/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/07/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/07/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/07/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/07/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/07/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/07/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/07/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/07/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/07/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/07/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/07/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/07/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/07/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/07/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/07/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/07/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/07/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/07/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/07/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/07/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/07/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/07/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/07/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/07/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/07/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/07/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/07/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/07/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/07/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/07/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/07/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/07/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/07/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/07/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/07/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/07/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/07/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/07/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/07/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/07/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/07/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/07/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/07/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/07/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/07/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/07/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/07/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/07/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/07/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/07/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/07/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/07/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/07/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/07/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/07/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/07/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel07_max = sizeof(Xchannel07) / sizeof(X32command);


X32command Xchannel08[] = {
		{"/ch/08",						{CHCO}, F_FND, {0}},
		{"/ch/08/config",				{CHCO}, F_FND, {0}},
		{"/ch/08/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/08/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/08/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/08/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/08/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/08/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/08/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/08/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/08/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/08/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/08/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/08/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/08/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/08/delay",				{CHDE}, F_FND, {0}},
		{"/ch/08/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/08/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/08/insert",				{CHIN}, F_FND, {0}},
		{"/ch/08/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/08/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/08/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/08/gate",					{CHGA}, F_FND, {0}},
		{"/ch/08/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/08/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/08/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/08/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/08/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/08/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/08/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/08/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/08/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/08/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/08/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/08/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/08/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/08/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/08/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/08/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/08/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/08/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/08/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/08/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/08/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/08/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/08/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/08/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/08/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/08/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/08/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/08/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/08/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/08/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/08/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/08/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/08/eq",					{OFFON}, F_FND, {1}},
		{"/ch/08/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/08/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/08/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/08/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/08/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/08/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/08/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/08/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/08/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/08/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/08/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/08/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/08/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/08/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/08/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/08/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/08/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/08/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/08/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/08/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/08/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/08/mix",					{CHMX}, F_FND, {0}},
		{"/ch/08/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/08/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/08/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/08/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/08/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/08/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/08/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/08/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/08/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/08/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/08/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/08/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/08/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/08/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/08/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/08/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/08/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/08/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/08/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/08/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/08/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/08/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/08/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/08/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/08/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/08/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/08/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/08/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/08/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/08/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/08/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/08/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/08/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/08/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/08/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/08/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/08/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/08/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/08/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/08/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/08/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/08/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/08/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/08/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/08/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/08/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/08/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/08/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/08/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/08/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/08/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/08/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/08/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/08/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/08/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/08/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/08/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/08/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/08/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/08/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/08/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/08/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/08/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/08/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/08/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/08/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/08/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/08/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/08/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/08/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel08_max = sizeof(Xchannel08) / sizeof(X32command);


X32command Xchannel09[] = {
		{"/ch/09",						{CHCO}, F_FND, {0}},
		{"/ch/09/config",				{CHCO}, F_FND, {0}},
		{"/ch/09/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/09/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/09/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/09/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/09/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/09/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/09/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/09/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/09/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/09/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/09/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/09/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/09/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/09/delay",				{CHDE}, F_FND, {0}},
		{"/ch/09/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/09/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/09/insert",				{CHIN}, F_FND, {0}},
		{"/ch/09/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/09/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/09/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/09/gate",					{CHGA}, F_FND, {0}},
		{"/ch/09/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/09/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/09/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/09/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/09/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/09/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/09/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/09/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/09/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/09/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/09/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/09/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/09/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/09/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/09/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/09/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/09/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/09/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/09/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/09/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/09/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/09/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/09/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/09/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/09/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/09/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/09/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/09/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/09/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/09/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/09/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/09/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/09/eq",					{OFFON}, F_FND, {1}},
		{"/ch/09/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/09/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/09/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/09/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/09/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/09/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/09/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/09/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/09/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/09/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/09/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/09/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/09/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/09/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/09/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/09/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/09/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/09/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/09/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/09/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/09/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/09/mix",					{CHMX}, F_FND, {0}},
		{"/ch/09/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/09/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/09/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/09/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/09/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/09/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/09/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/09/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/09/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/09/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/09/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/09/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/09/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/09/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/09/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/09/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/09/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/09/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/09/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/09/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/09/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/09/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/09/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/09/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/09/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/09/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/09/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/09/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/09/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/09/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/09/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/09/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/09/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/09/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/09/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/09/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/09/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/09/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/09/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/09/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/09/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/09/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/09/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/09/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/09/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/09/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/09/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/09/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/09/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/09/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/09/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/09/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/09/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/09/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/09/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/09/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/09/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/09/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/09/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/09/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/09/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/09/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/09/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/09/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/09/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/09/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/09/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/09/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/09/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/09/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel09_max = sizeof(Xchannel09) / sizeof(X32command);


X32command Xchannel10[] = {
		{"/ch/10",						{CHCO}, F_FND, {0}},
		{"/ch/10/config",				{CHCO}, F_FND, {0}},
		{"/ch/10/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/10/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/10/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/10/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/10/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/10/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/10/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/10/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/10/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/10/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/10/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/10/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/10/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/10/delay",				{CHDE}, F_FND, {0}},
		{"/ch/10/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/10/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/10/insert",				{CHIN}, F_FND, {0}},
		{"/ch/10/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/10/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/10/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/10/gate",					{CHGA}, F_FND, {0}},
		{"/ch/10/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/10/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/10/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/10/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/10/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/10/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/10/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/10/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/10/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/10/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/10/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/10/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/10/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/10/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/10/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/10/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/10/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/10/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/10/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/10/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/10/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/10/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/10/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/10/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/10/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/10/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/10/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/10/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/10/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/10/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/10/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/10/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/10/eq",					{OFFON}, F_FND, {1}},
		{"/ch/10/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/10/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/10/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/10/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/10/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/10/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/10/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/10/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/10/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/10/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/10/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/10/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/10/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/10/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/10/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/10/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/10/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/10/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/10/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/10/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/10/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/10/mix",					{CHMX}, F_FND, {0}},
		{"/ch/10/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/10/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/10/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/10/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/10/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/10/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/10/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/10/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/10/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/10/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/10/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/10/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/10/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/10/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/10/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/10/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/10/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/10/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/10/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/10/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/10/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/10/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/10/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/10/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/10/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/10/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/10/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/10/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/10/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/10/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/10/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/10/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/10/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/10/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/10/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/10/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/10/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/10/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/10/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/10/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/10/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/10/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/10/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/10/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/10/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/10/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/10/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/10/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/10/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/10/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/10/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/10/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/10/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/10/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/10/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/10/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/10/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/10/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/10/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/10/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/10/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/10/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/10/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/10/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/10/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/10/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/10/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/10/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/10/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/10/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel10_max = sizeof(Xchannel10) / sizeof(X32command);


X32command Xchannel11[] = {
		{"/ch/11",						{CHCO}, F_FND, {0}},
		{"/ch/11/config",				{CHCO}, F_FND, {0}},
		{"/ch/11/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/11/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/11/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/11/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/11/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/11/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/11/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/11/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/11/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/11/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/11/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/11/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/11/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/11/delay",				{CHDE}, F_FND, {0}},
		{"/ch/11/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/11/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/11/insert",				{CHIN}, F_FND, {0}},
		{"/ch/11/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/11/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/11/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/11/gate",					{CHGA}, F_FND, {0}},
		{"/ch/11/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/11/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/11/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/11/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/11/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/11/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/11/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/11/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/11/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/11/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/11/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/11/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/11/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/11/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/11/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/11/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/11/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/11/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/11/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/11/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/11/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/11/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/11/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/11/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/11/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/11/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/11/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/11/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/11/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/11/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/11/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/11/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/11/eq",					{OFFON}, F_FND, {1}},
		{"/ch/11/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/11/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/11/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/11/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/11/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/11/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/11/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/11/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/11/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/11/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/11/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/11/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/11/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/11/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/11/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/11/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/11/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/11/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/11/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/11/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/11/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/11/mix",					{CHMX}, F_FND, {0}},
		{"/ch/11/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/11/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/11/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/11/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/11/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/11/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/11/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/11/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/11/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/11/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/11/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/11/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/11/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/11/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/11/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/11/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/11/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/11/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/11/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/11/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/11/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/11/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/11/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/11/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/11/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/11/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/11/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/11/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/11/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/11/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/11/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/11/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/11/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/11/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/11/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/11/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/11/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/11/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/11/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/11/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/11/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/11/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/11/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/11/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/11/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/11/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/11/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/11/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/11/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/11/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/11/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/11/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/11/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/11/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/11/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/11/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/11/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/11/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/11/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/11/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/11/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/11/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/11/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/11/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/11/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/11/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/11/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/11/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/11/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/11/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel11_max = sizeof(Xchannel11) / sizeof(X32command);


X32command Xchannel12[] = {
		{"/ch/12",						{CHCO}, F_FND, {0}},
		{"/ch/12/config",				{CHCO}, F_FND, {0}},
		{"/ch/12/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/12/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/12/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/12/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/12/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/12/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/12/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/12/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/12/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/12/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/12/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/12/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/12/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/12/delay",				{CHDE}, F_FND, {0}},
		{"/ch/12/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/12/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/12/insert",				{CHIN}, F_FND, {0}},
		{"/ch/12/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/12/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/12/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/12/gate",					{CHGA}, F_FND, {0}},
		{"/ch/12/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/12/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/12/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/12/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/12/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/12/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/12/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/12/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/12/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/12/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/12/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/12/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/12/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/12/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/12/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/12/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/12/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/12/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/12/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/12/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/12/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/12/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/12/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/12/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/12/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/12/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/12/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/12/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/12/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/12/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/12/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/12/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/12/eq",					{OFFON}, F_FND, {1}},
		{"/ch/12/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/12/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/12/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/12/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/12/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/12/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/12/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/12/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/12/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/12/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/12/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/12/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/12/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/12/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/12/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/12/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/12/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/12/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/12/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/12/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/12/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/12/mix",					{CHMX}, F_FND, {0}},
		{"/ch/12/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/12/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/12/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/12/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/12/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/12/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/12/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/12/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/12/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/12/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/12/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/12/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/12/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/12/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/12/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/12/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/12/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/12/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/12/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/12/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/12/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/12/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/12/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/12/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/12/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/12/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/12/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/12/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/12/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/12/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/12/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/12/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/12/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/12/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/12/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/12/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/12/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/12/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/12/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/12/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/12/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/12/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/12/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/12/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/12/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/12/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/12/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/12/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/12/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/12/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/12/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/12/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/12/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/12/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/12/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/12/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/12/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/12/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/12/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/12/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/12/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/12/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/12/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/12/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/12/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/12/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/12/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/12/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/12/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/12/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel12_max = sizeof(Xchannel12) / sizeof(X32command);


X32command Xchannel13[] = {
		{"/ch/13",						{CHCO}, F_FND, {0}},
		{"/ch/13/config",				{CHCO}, F_FND, {0}},
		{"/ch/13/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/13/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/13/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/13/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/13/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/13/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/13/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/13/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/13/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/13/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/13/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/13/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/13/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/13/delay",				{CHDE}, F_FND, {0}},
		{"/ch/13/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/13/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/13/insert",				{CHIN}, F_FND, {0}},
		{"/ch/13/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/13/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/13/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/13/gate",					{CHGA}, F_FND, {0}},
		{"/ch/13/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/13/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/13/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/13/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/13/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/13/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/13/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/13/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/13/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/13/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/13/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/13/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/13/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/13/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/13/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/13/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/13/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/13/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/13/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/13/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/13/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/13/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/13/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/13/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/13/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/13/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/13/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/13/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/13/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/13/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/13/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/13/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/13/eq",					{OFFON}, F_FND, {1}},
		{"/ch/13/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/13/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/13/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/13/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/13/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/13/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/13/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/13/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/13/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/13/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/13/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/13/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/13/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/13/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/13/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/13/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/13/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/13/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/13/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/13/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/13/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/13/mix",					{CHMX}, F_FND, {0}},
		{"/ch/13/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/13/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/13/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/13/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/13/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/13/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/13/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/13/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/13/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/13/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/13/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/13/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/13/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/13/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/13/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/13/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/13/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/13/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/13/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/13/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/13/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/13/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/13/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/13/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/13/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/13/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/13/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/13/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/13/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/13/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/13/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/13/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/13/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/13/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/13/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/13/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/13/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/13/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/13/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/13/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/13/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/13/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/13/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/13/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/13/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/13/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/13/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/13/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/13/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/13/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/13/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/13/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/13/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/13/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/13/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/13/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/13/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/13/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/13/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/13/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/13/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/13/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/13/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/13/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/13/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/13/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/13/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/13/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/13/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/13/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel13_max = sizeof(Xchannel13) / sizeof(X32command);


X32command Xchannel14[] = {
		{"/ch/14",						{CHCO}, F_FND, {0}},
		{"/ch/14/config",				{CHCO}, F_FND, {0}},
		{"/ch/14/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/14/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/14/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/14/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/14/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/14/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/14/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/14/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/14/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/14/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/14/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/14/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/14/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/14/delay",				{CHDE}, F_FND, {0}},
		{"/ch/14/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/14/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/14/insert",				{CHIN}, F_FND, {0}},
		{"/ch/14/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/14/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/14/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/14/gate",					{CHGA}, F_FND, {0}},
		{"/ch/14/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/14/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/14/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/14/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/14/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/14/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/14/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/14/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/14/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/14/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/14/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/14/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/14/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/14/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/14/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/14/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/14/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/14/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/14/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/14/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/14/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/14/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/14/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/14/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/14/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/14/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/14/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/14/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/14/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/14/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/14/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/14/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/14/eq",					{OFFON}, F_FND, {1}},
		{"/ch/14/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/14/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/14/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/14/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/14/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/14/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/14/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/14/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/14/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/14/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/14/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/14/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/14/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/14/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/14/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/14/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/14/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/14/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/14/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/14/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/14/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/14/mix",					{CHMX}, F_FND, {0}},
		{"/ch/14/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/14/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/14/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/14/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/14/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/14/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/14/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/14/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/14/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/14/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/14/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/14/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/14/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/14/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/14/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/14/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/14/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/14/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/14/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/14/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/14/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/14/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/14/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/14/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/14/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/14/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/14/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/14/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/14/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/14/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/14/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/14/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/14/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/14/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/14/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/14/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/14/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/14/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/14/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/14/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/14/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/14/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/14/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/14/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/14/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/14/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/14/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/14/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/14/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/14/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/14/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/14/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/14/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/14/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/14/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/14/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/14/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/14/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/14/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/14/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/14/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/14/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/14/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/14/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/14/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/14/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/14/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/14/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/14/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/14/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel14_max = sizeof(Xchannel14) / sizeof(X32command);


X32command Xchannel15[] = {
		{"/ch/15",						{CHCO}, F_FND, {0}},
		{"/ch/15/config",				{CHCO}, F_FND, {0}},
		{"/ch/15/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/15/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/15/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/15/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/15/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/15/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/15/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/15/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/15/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/15/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/15/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/15/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/15/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/15/delay",				{CHDE}, F_FND, {0}},
		{"/ch/15/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/15/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/15/insert",				{CHIN}, F_FND, {0}},
		{"/ch/15/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/15/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/15/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/15/gate",					{CHGA}, F_FND, {0}},
		{"/ch/15/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/15/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/15/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/15/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/15/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/15/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/15/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/15/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/15/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/15/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/15/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/15/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/15/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/15/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/15/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/15/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/15/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/15/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/15/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/15/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/15/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/15/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/15/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/15/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/15/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/15/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/15/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/15/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/15/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/15/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/15/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/15/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/15/eq",					{OFFON}, F_FND, {1}},
		{"/ch/15/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/15/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/15/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/15/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/15/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/15/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/15/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/15/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/15/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/15/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/15/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/15/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/15/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/15/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/15/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/15/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/15/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/15/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/15/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/15/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/15/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/15/mix",					{CHMX}, F_FND, {0}},
		{"/ch/15/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/15/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/15/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/15/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/15/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/15/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/15/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/15/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/15/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/15/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/15/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/15/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/15/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/15/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/15/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/15/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/15/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/15/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/15/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/15/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/15/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/15/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/15/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/15/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/15/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/15/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/15/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/15/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/15/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/15/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/15/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/15/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/15/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/15/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/15/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/15/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/15/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/15/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/15/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/15/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/15/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/15/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/15/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/15/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/15/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/15/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/15/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/15/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/15/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/15/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/15/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/15/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/15/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/15/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/15/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/15/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/15/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/15/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/15/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/15/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/15/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/15/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/15/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/15/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/15/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/15/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/15/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/15/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/15/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/15/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel15_max = sizeof(Xchannel15) / sizeof(X32command);


X32command Xchannel16[] = {
		{"/ch/16",						{CHCO}, F_FND, {0}},
		{"/ch/16/config",				{CHCO}, F_FND, {0}},
		{"/ch/16/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/16/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/16/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/16/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/16/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/16/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/16/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/16/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/16/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/16/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/16/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/16/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/16/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/16/delay",				{CHDE}, F_FND, {0}},
		{"/ch/16/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/16/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/16/insert",				{CHIN}, F_FND, {0}},
		{"/ch/16/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/16/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/16/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/16/gate",					{CHGA}, F_FND, {0}},
		{"/ch/16/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/16/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/16/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/16/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/16/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/16/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/16/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/16/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/16/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/16/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/16/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/16/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/16/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/16/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/16/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/16/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/16/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/16/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/16/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/16/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/16/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/16/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/16/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/16/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/16/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/16/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/16/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/16/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/16/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/16/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/16/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/16/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/16/eq",					{OFFON}, F_FND, {1}},
		{"/ch/16/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/16/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/16/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/16/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/16/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/16/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/16/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/16/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/16/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/16/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/16/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/16/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/16/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/16/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/16/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/16/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/16/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/16/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/16/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/16/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/16/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/16/mix",					{CHMX}, F_FND, {0}},
		{"/ch/16/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/16/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/16/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/16/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/16/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/16/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/16/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/16/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/16/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/16/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/16/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/16/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/16/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/16/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/16/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/16/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/16/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/16/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/16/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/16/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/16/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/16/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/16/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/16/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/16/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/16/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/16/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/16/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/16/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/16/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/16/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/16/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/16/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/16/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/16/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/16/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/16/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/16/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/16/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/16/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/16/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/16/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/16/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/16/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/16/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/16/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/16/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/16/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/16/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/16/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/16/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/16/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/16/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/16/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/16/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/16/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/16/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/16/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/16/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/16/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/16/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/16/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/16/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/16/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/16/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/16/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/16/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/16/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/16/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/16/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel16_max = sizeof(Xchannel16) / sizeof(X32command);


X32command Xchannel17[] = {
		{"/ch/17",						{CHCO}, F_FND, {0}},
		{"/ch/17/config",				{CHCO}, F_FND, {0}},
		{"/ch/17/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/17/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/17/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/17/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/17/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/17/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/17/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/17/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/17/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/17/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/17/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/17/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/17/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/17/delay",				{CHDE}, F_FND, {0}},
		{"/ch/17/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/17/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/17/insert",				{CHIN}, F_FND, {0}},
		{"/ch/17/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/17/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/17/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/17/gate",					{CHGA}, F_FND, {0}},
		{"/ch/17/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/17/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/17/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/17/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/17/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/17/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/17/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/17/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/17/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/17/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/17/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/17/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/17/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/17/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/17/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/17/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/17/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/17/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/17/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/17/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/17/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/17/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/17/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/17/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/17/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/17/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/17/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/17/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/17/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/17/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/17/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/17/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/17/eq",					{OFFON}, F_FND, {1}},
		{"/ch/17/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/17/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/17/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/17/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/17/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/17/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/17/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/17/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/17/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/17/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/17/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/17/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/17/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/17/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/17/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/17/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/17/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/17/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/17/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/17/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/17/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/17/mix",					{CHMX}, F_FND, {0}},
		{"/ch/17/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/17/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/17/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/17/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/17/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/17/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/17/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/17/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/17/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/17/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/17/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/17/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/17/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/17/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/17/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/17/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/17/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/17/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/17/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/17/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/17/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/17/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/17/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/17/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/17/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/17/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/17/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/17/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/17/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/17/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/17/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/17/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/17/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/17/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/17/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/17/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/17/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/17/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/17/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/17/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/17/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/17/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/17/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/17/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/17/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/17/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/17/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/17/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/17/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/17/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/17/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/17/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/17/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/17/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/17/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/17/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/17/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/17/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/17/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/17/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/17/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/17/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/17/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/17/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/17/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/17/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/17/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/17/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/17/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/17/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel17_max = sizeof(Xchannel17) / sizeof(X32command);


X32command Xchannel18[] = {
		{"/ch/18",						{CHCO}, F_FND, {0}},
		{"/ch/18/config",				{CHCO}, F_FND, {0}},
		{"/ch/18/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/18/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/18/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/18/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/18/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/18/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/18/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/18/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/18/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/18/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/18/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/18/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/18/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/18/delay",				{CHDE}, F_FND, {0}},
		{"/ch/18/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/18/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/18/insert",				{CHIN}, F_FND, {0}},
		{"/ch/18/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/18/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/18/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/18/gate",					{CHGA}, F_FND, {0}},
		{"/ch/18/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/18/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/18/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/18/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/18/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/18/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/18/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/18/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/18/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/18/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/18/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/18/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/18/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/18/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/18/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/18/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/18/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/18/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/18/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/18/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/18/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/18/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/18/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/18/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/18/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/18/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/18/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/18/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/18/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/18/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/18/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/18/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/18/eq",					{OFFON}, F_FND, {1}},
		{"/ch/18/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/18/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/18/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/18/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/18/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/18/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/18/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/18/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/18/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/18/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/18/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/18/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/18/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/18/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/18/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/18/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/18/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/18/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/18/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/18/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/18/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/18/mix",					{CHMX}, F_FND, {0}},
		{"/ch/18/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/18/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/18/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/18/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/18/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/18/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/18/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/18/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/18/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/18/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/18/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/18/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/18/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/18/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/18/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/18/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/18/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/18/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/18/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/18/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/18/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/18/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/18/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/18/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/18/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/18/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/18/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/18/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/18/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/18/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/18/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/18/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/18/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/18/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/18/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/18/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/18/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/18/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/18/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/18/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/18/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/18/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/18/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/18/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/18/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/18/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/18/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/18/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/18/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/18/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/18/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/18/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/18/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/18/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/18/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/18/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/18/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/18/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/18/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/18/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/18/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/18/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/18/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/18/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/18/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/18/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/18/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/18/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/18/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/18/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel18_max = sizeof(Xchannel18) / sizeof(X32command);


X32command Xchannel19[] = {
		{"/ch/19",						{CHCO}, F_FND, {0}},
		{"/ch/19/config",				{CHCO}, F_FND, {0}},
		{"/ch/19/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/19/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/19/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/19/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/19/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/19/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/19/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/19/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/19/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/19/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/19/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/19/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/19/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/19/delay",				{CHDE}, F_FND, {0}},
		{"/ch/19/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/19/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/19/insert",				{CHIN}, F_FND, {0}},
		{"/ch/19/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/19/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/19/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/19/gate",					{CHGA}, F_FND, {0}},
		{"/ch/19/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/19/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/19/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/19/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/19/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/19/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/19/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/19/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/19/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/19/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/19/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/19/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/19/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/19/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/19/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/19/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/19/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/19/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/19/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/19/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/19/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/19/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/19/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/19/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/19/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/19/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/19/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/19/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/19/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/19/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/19/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/19/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/19/eq",					{OFFON}, F_FND, {1}},
		{"/ch/19/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/19/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/19/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/19/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/19/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/19/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/19/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/19/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/19/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/19/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/19/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/19/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/19/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/19/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/19/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/19/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/19/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/19/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/19/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/19/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/19/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/19/mix",					{CHMX}, F_FND, {0}},
		{"/ch/19/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/19/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/19/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/19/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/19/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/19/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/19/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/19/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/19/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/19/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/19/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/19/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/19/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/19/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/19/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/19/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/19/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/19/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/19/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/19/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/19/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/19/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/19/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/19/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/19/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/19/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/19/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/19/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/19/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/19/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/19/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/19/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/19/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/19/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/19/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/19/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/19/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/19/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/19/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/19/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/19/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/19/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/19/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/19/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/19/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/19/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/19/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/19/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/19/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/19/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/19/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/19/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/19/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/19/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/19/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/19/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/19/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/19/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/19/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/19/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/19/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/19/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/19/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/19/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/19/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/19/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/19/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/19/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/19/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/19/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel19_max = sizeof(Xchannel19) / sizeof(X32command);


X32command Xchannel20[] = {
		{"/ch/20",						{CHCO}, F_FND, {0}},
		{"/ch/20/config",				{CHCO}, F_FND, {0}},
		{"/ch/20/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/20/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/20/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/20/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/20/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/20/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/20/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/20/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/20/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/20/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/20/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/20/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/20/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/20/delay",				{CHDE}, F_FND, {0}},
		{"/ch/20/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/20/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/20/insert",				{CHIN}, F_FND, {0}},
		{"/ch/20/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/20/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/20/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/20/gate",					{CHGA}, F_FND, {0}},
		{"/ch/20/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/20/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/20/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/20/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/20/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/20/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/20/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/20/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/20/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/20/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/20/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/20/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/20/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/20/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/20/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/20/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/20/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/20/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/20/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/20/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/20/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/20/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/20/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/20/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/20/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/20/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/20/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/20/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/20/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/20/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/20/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/20/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/20/eq",					{OFFON}, F_FND, {1}},
		{"/ch/20/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/20/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/20/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/20/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/20/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/20/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/20/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/20/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/20/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/20/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/20/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/20/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/20/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/20/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/20/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/20/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/20/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/20/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/20/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/20/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/20/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/20/mix",					{CHMX}, F_FND, {0}},
		{"/ch/20/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/20/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/20/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/20/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/20/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/20/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/20/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/20/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/20/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/20/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/20/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/20/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/20/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/20/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/20/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/20/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/20/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/20/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/20/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/20/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/20/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/20/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/20/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/20/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/20/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/20/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/20/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/20/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/20/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/20/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/20/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/20/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/20/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/20/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/20/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/20/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/20/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/20/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/20/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/20/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/20/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/20/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/20/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/20/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/20/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/20/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/20/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/20/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/20/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/20/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/20/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/20/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/20/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/20/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/20/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/20/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/20/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/20/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/20/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/20/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/20/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/20/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/20/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/20/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/20/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/20/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/20/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/20/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/20/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/20/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel20_max = sizeof(Xchannel20) / sizeof(X32command);


X32command Xchannel21[] = {
		{"/ch/21",						{CHCO}, F_FND, {0}},
		{"/ch/21/config",				{CHCO}, F_FND, {0}},
		{"/ch/21/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/21/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/21/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/21/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/21/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/21/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/21/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/21/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/21/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/21/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/21/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/21/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/21/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/21/delay",				{CHDE}, F_FND, {0}},
		{"/ch/21/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/21/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/21/insert",				{CHIN}, F_FND, {0}},
		{"/ch/21/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/21/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/21/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/21/gate",					{CHGA}, F_FND, {0}},
		{"/ch/21/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/21/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/21/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/21/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/21/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/21/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/21/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/21/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/21/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/21/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/21/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/21/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/21/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/21/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/21/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/21/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/21/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/21/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/21/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/21/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/21/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/21/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/21/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/21/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/21/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/21/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/21/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/21/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/21/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/21/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/21/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/21/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/21/eq",					{OFFON}, F_FND, {1}},
		{"/ch/21/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/21/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/21/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/21/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/21/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/21/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/21/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/21/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/21/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/21/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/21/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/21/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/21/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/21/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/21/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/21/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/21/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/21/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/21/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/21/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/21/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/21/mix",					{CHMX}, F_FND, {0}},
		{"/ch/21/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/21/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/21/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/21/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/21/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/21/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/21/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/21/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/21/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/21/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/21/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/21/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/21/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/21/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/21/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/21/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/21/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/21/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/21/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/21/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/21/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/21/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/21/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/21/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/21/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/21/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/21/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/21/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/21/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/21/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/21/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/21/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/21/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/21/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/21/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/21/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/21/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/21/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/21/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/21/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/21/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/21/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/21/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/21/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/21/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/21/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/21/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/21/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/21/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/21/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/21/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/21/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/21/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/21/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/21/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/21/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/21/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/21/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/21/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/21/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/21/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/21/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/21/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/21/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/21/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/21/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/21/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/21/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/21/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/21/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel21_max = sizeof(Xchannel21) / sizeof(X32command);


X32command Xchannel22[] = {
		{"/ch/22",						{CHCO}, F_FND, {0}},
		{"/ch/22/config",				{CHCO}, F_FND, {0}},
		{"/ch/22/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/22/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/22/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/22/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/22/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/22/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/22/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/22/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/22/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/22/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/22/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/22/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/22/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/22/delay",				{CHDE}, F_FND, {0}},
		{"/ch/22/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/22/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/22/insert",				{CHIN}, F_FND, {0}},
		{"/ch/22/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/22/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/22/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/22/gate",					{CHGA}, F_FND, {0}},
		{"/ch/22/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/22/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/22/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/22/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/22/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/22/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/22/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/22/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/22/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/22/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/22/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/22/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/22/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/22/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/22/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/22/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/22/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/22/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/22/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/22/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/22/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/22/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/22/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/22/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/22/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/22/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/22/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/22/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/22/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/22/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/22/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/22/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/22/eq",					{OFFON}, F_FND, {1}},
		{"/ch/22/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/22/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/22/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/22/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/22/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/22/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/22/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/22/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/22/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/22/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/22/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/22/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/22/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/22/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/22/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/22/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/22/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/22/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/22/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/22/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/22/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/22/mix",					{CHMX}, F_FND, {0}},
		{"/ch/22/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/22/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/22/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/22/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/22/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/22/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/22/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/22/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/22/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/22/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/22/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/22/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/22/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/22/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/22/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/22/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/22/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/22/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/22/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/22/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/22/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/22/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/22/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/22/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/22/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/22/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/22/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/22/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/22/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/22/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/22/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/22/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/22/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/22/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/22/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/22/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/22/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/22/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/22/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/22/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/22/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/22/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/22/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/22/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/22/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/22/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/22/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/22/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/22/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/22/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/22/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/22/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/22/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/22/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/22/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/22/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/22/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/22/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/22/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/22/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/22/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/22/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/22/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/22/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/22/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/22/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/22/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/22/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/22/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/22/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel22_max = sizeof(Xchannel22) / sizeof(X32command);


X32command Xchannel23[] = {
		{"/ch/23",						{CHCO}, F_FND, {0}},
		{"/ch/23/config",				{CHCO}, F_FND, {0}},
		{"/ch/23/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/23/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/23/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/23/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/23/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/23/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/23/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/23/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/23/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/23/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/23/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/23/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/23/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/23/delay",				{CHDE}, F_FND, {0}},
		{"/ch/23/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/23/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/23/insert",				{CHIN}, F_FND, {0}},
		{"/ch/23/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/23/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/23/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/23/gate",					{CHGA}, F_FND, {0}},
		{"/ch/23/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/23/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/23/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/23/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/23/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/23/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/23/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/23/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/23/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/23/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/23/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/23/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/23/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/23/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/23/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/23/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/23/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/23/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/23/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/23/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/23/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/23/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/23/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/23/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/23/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/23/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/23/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/23/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/23/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/23/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/23/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/23/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/23/eq",					{OFFON}, F_FND, {1}},
		{"/ch/23/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/23/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/23/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/23/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/23/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/23/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/23/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/23/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/23/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/23/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/23/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/23/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/23/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/23/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/23/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/23/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/23/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/23/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/23/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/23/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/23/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/23/mix",					{CHMX}, F_FND, {0}},
		{"/ch/23/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/23/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/23/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/23/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/23/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/23/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/23/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/23/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/23/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/23/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/23/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/23/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/23/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/23/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/23/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/23/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/23/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/23/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/23/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/23/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/23/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/23/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/23/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/23/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/23/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/23/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/23/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/23/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/23/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/23/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/23/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/23/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/23/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/23/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/23/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/23/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/23/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/23/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/23/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/23/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/23/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/23/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/23/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/23/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/23/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/23/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/23/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/23/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/23/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/23/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/23/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/23/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/23/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/23/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/23/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/23/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/23/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/23/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/23/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/23/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/23/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/23/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/23/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/23/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/23/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/23/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/23/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/23/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/23/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/23/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel23_max = sizeof(Xchannel23) / sizeof(X32command);


X32command Xchannel24[] = {
		{"/ch/24",						{CHCO}, F_FND, {0}},
		{"/ch/24/config",				{CHCO}, F_FND, {0}},
		{"/ch/24/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/24/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/24/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/24/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/24/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/24/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/24/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/24/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/24/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/24/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/24/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/24/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/24/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/24/delay",				{CHDE}, F_FND, {0}},
		{"/ch/24/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/24/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/24/insert",				{CHIN}, F_FND, {0}},
		{"/ch/24/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/24/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/24/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/24/gate",					{CHGA}, F_FND, {0}},
		{"/ch/24/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/24/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/24/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/24/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/24/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/24/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/24/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/24/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/24/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/24/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/24/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/24/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/24/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/24/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/24/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/24/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/24/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/24/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/24/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/24/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/24/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/24/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/24/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/24/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/24/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/24/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/24/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/24/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/24/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/24/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/24/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/24/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/24/eq",					{OFFON}, F_FND, {1}},
		{"/ch/24/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/24/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/24/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/24/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/24/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/24/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/24/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/24/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/24/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/24/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/24/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/24/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/24/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/24/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/24/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/24/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/24/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/24/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/24/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/24/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/24/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/24/mix",					{CHMX}, F_FND, {0}},
		{"/ch/24/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/24/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/24/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/24/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/24/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/24/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/24/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/24/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/24/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/24/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/24/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/24/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/24/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/24/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/24/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/24/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/24/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/24/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/24/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/24/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/24/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/24/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/24/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/24/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/24/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/24/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/24/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/24/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/24/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/24/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/24/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/24/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/24/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/24/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/24/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/24/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/24/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/24/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/24/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/24/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/24/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/24/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/24/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/24/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/24/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/24/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/24/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/24/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/24/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/24/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/24/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/24/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/24/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/24/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/24/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/24/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/24/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/24/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/24/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/24/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/24/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/24/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/24/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/24/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/24/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/24/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/24/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/24/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/24/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/24/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel24_max = sizeof(Xchannel24) / sizeof(X32command);


X32command Xchannel25[] = {
		{"/ch/25",						{CHCO}, F_FND, {0}},
		{"/ch/25/config",				{CHCO}, F_FND, {0}},
		{"/ch/25/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/25/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/25/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/25/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/25/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/25/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/25/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/25/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/25/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/25/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/25/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/25/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/25/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/25/delay",				{CHDE}, F_FND, {0}},
		{"/ch/25/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/25/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/25/insert",				{CHIN}, F_FND, {0}},
		{"/ch/25/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/25/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/25/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/25/gate",					{CHGA}, F_FND, {0}},
		{"/ch/25/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/25/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/25/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/25/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/25/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/25/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/25/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/25/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/25/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/25/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/25/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/25/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/25/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/25/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/25/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/25/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/25/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/25/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/25/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/25/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/25/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/25/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/25/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/25/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/25/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/25/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/25/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/25/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/25/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/25/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/25/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/25/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/25/eq",					{OFFON}, F_FND, {1}},
		{"/ch/25/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/25/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/25/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/25/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/25/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/25/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/25/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/25/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/25/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/25/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/25/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/25/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/25/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/25/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/25/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/25/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/25/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/25/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/25/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/25/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/25/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/25/mix",					{CHMX}, F_FND, {0}},
		{"/ch/25/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/25/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/25/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/25/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/25/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/25/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/25/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/25/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/25/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/25/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/25/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/25/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/25/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/25/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/25/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/25/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/25/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/25/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/25/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/25/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/25/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/25/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/25/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/25/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/25/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/25/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/25/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/25/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/25/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/25/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/25/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/25/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/25/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/25/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/25/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/25/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/25/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/25/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/25/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/25/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/25/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/25/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/25/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/25/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/25/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/25/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/25/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/25/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/25/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/25/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/25/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/25/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/25/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/25/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/25/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/25/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/25/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/25/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/25/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/25/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/25/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/25/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/25/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/25/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/25/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/25/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/25/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/25/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/25/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/25/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel25_max = sizeof(Xchannel25) / sizeof(X32command);


X32command Xchannel26[] = {
		{"/ch/26",						{CHCO}, F_FND, {0}},
		{"/ch/26/config",				{CHCO}, F_FND, {0}},
		{"/ch/26/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/26/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/26/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/26/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/26/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/26/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/26/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/26/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/26/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/26/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/26/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/26/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/26/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/26/delay",				{CHDE}, F_FND, {0}},
		{"/ch/26/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/26/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/26/insert",				{CHIN}, F_FND, {0}},
		{"/ch/26/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/26/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/26/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/26/gate",					{CHGA}, F_FND, {0}},
		{"/ch/26/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/26/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/26/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/26/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/26/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/26/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/26/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/26/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/26/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/26/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/26/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/26/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/26/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/26/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/26/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/26/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/26/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/26/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/26/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/26/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/26/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/26/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/26/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/26/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/26/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/26/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/26/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/26/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/26/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/26/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/26/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/26/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/26/eq",					{OFFON}, F_FND, {1}},
		{"/ch/26/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/26/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/26/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/26/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/26/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/26/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/26/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/26/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/26/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/26/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/26/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/26/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/26/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/26/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/26/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/26/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/26/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/26/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/26/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/26/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/26/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/26/mix",					{CHMX}, F_FND, {0}},
		{"/ch/26/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/26/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/26/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/26/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/26/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/26/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/26/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/26/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/26/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/26/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/26/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/26/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/26/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/26/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/26/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/26/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/26/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/26/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/26/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/26/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/26/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/26/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/26/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/26/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/26/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/26/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/26/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/26/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/26/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/26/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/26/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/26/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/26/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/26/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/26/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/26/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/26/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/26/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/26/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/26/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/26/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/26/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/26/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/26/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/26/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/26/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/26/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/26/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/26/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/26/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/26/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/26/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/26/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/26/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/26/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/26/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/26/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/26/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/26/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/26/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/26/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/26/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/26/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/26/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/26/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/26/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/26/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/26/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/26/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/26/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel26_max = sizeof(Xchannel26) / sizeof(X32command);


X32command Xchannel27[] = {
		{"/ch/27",						{CHCO}, F_FND, {0}},
		{"/ch/27/config",				{CHCO}, F_FND, {0}},
		{"/ch/27/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/27/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/27/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/27/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/27/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/27/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/27/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/27/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/27/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/27/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/27/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/27/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/27/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/27/delay",				{CHDE}, F_FND, {0}},
		{"/ch/27/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/27/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/27/insert",				{CHIN}, F_FND, {0}},
		{"/ch/27/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/27/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/27/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/27/gate",					{CHGA}, F_FND, {0}},
		{"/ch/27/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/27/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/27/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/27/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/27/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/27/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/27/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/27/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/27/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/27/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/27/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/27/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/27/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/27/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/27/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/27/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/27/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/27/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/27/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/27/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/27/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/27/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/27/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/27/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/27/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/27/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/27/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/27/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/27/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/27/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/27/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/27/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/27/eq",					{OFFON}, F_FND, {1}},
		{"/ch/27/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/27/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/27/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/27/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/27/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/27/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/27/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/27/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/27/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/27/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/27/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/27/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/27/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/27/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/27/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/27/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/27/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/27/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/27/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/27/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/27/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/27/mix",					{CHMX}, F_FND, {0}},
		{"/ch/27/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/27/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/27/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/27/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/27/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/27/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/27/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/27/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/27/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/27/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/27/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/27/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/27/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/27/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/27/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/27/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/27/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/27/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/27/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/27/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/27/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/27/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/27/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/27/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/27/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/27/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/27/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/27/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/27/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/27/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/27/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/27/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/27/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/27/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/27/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/27/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/27/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/27/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/27/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/27/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/27/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/27/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/27/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/27/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/27/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/27/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/27/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/27/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/27/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/27/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/27/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/27/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/27/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/27/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/27/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/27/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/27/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/27/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/27/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/27/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/27/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/27/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/27/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/27/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/27/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/27/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/27/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/27/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/27/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/27/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel27_max = sizeof(Xchannel27) / sizeof(X32command);


X32command Xchannel28[] = {
		{"/ch/28",						{CHCO}, F_FND, {0}},
		{"/ch/28/config",				{CHCO}, F_FND, {0}},
		{"/ch/28/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/28/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/28/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/28/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/28/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/28/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/28/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/28/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/28/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/28/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/28/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/28/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/28/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/28/delay",				{CHDE}, F_FND, {0}},
		{"/ch/28/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/28/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/28/insert",				{CHIN}, F_FND, {0}},
		{"/ch/28/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/28/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/28/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/28/gate",					{CHGA}, F_FND, {0}},
		{"/ch/28/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/28/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/28/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/28/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/28/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/28/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/28/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/28/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/28/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/28/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/28/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/28/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/28/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/28/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/28/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/28/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/28/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/28/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/28/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/28/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/28/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/28/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/28/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/28/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/28/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/28/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/28/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/28/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/28/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/28/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/28/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/28/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/28/eq",					{OFFON}, F_FND, {1}},
		{"/ch/28/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/28/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/28/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/28/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/28/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/28/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/28/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/28/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/28/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/28/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/28/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/28/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/28/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/28/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/28/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/28/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/28/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/28/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/28/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/28/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/28/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/28/mix",					{CHMX}, F_FND, {0}},
		{"/ch/28/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/28/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/28/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/28/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/28/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/28/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/28/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/28/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/28/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/28/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/28/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/28/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/28/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/28/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/28/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/28/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/28/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/28/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/28/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/28/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/28/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/28/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/28/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/28/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/28/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/28/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/28/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/28/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/28/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/28/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/28/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/28/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/28/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/28/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/28/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/28/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/28/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/28/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/28/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/28/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/28/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/28/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/28/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/28/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/28/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/28/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/28/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/28/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/28/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/28/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/28/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/28/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/28/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/28/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/28/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/28/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/28/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/28/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/28/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/28/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/28/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/28/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/28/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/28/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/28/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/28/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/28/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/28/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/28/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/28/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel28_max = sizeof(Xchannel28) / sizeof(X32command);


X32command Xchannel29[] = {
		{"/ch/29",						{CHCO}, F_FND, {0}},
		{"/ch/29/config",				{CHCO}, F_FND, {0}},
		{"/ch/29/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/29/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/29/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/29/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/29/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/29/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/29/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/29/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/29/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/29/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/29/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/29/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/29/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/29/delay",				{CHDE}, F_FND, {0}},
		{"/ch/29/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/29/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/29/insert",				{CHIN}, F_FND, {0}},
		{"/ch/29/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/29/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/29/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/29/gate",					{CHGA}, F_FND, {0}},
		{"/ch/29/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/29/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/29/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/29/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/29/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/29/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/29/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/29/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/29/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/29/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/29/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/29/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/29/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/29/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/29/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/29/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/29/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/29/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/29/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/29/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/29/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/29/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/29/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/29/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/29/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/29/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/29/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/29/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/29/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/29/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/29/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/29/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/29/eq",					{OFFON}, F_FND, {1}},
		{"/ch/29/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/29/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/29/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/29/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/29/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/29/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/29/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/29/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/29/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/29/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/29/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/29/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/29/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/29/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/29/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/29/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/29/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/29/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/29/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/29/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/29/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/29/mix",					{CHMX}, F_FND, {0}},
		{"/ch/29/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/29/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/29/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/29/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/29/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/29/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/29/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/29/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/29/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/29/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/29/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/29/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/29/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/29/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/29/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/29/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/29/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/29/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/29/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/29/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/29/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/29/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/29/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/29/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/29/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/29/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/29/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/29/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/29/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/29/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/29/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/29/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/29/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/29/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/29/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/29/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/29/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/29/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/29/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/29/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/29/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/29/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/29/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/29/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/29/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/29/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/29/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/29/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/29/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/29/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/29/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/29/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/29/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/29/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/29/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/29/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/29/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/29/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/29/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/29/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/29/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/29/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/29/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/29/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/29/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/29/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/29/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/29/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/29/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/29/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel29_max = sizeof(Xchannel29) / sizeof(X32command);


X32command Xchannel30[] = {
		{"/ch/30",						{CHCO}, F_FND, {0}},
		{"/ch/30/config",				{CHCO}, F_FND, {0}},
		{"/ch/30/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/30/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/30/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/30/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/30/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/30/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/30/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/30/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/30/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/30/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/30/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/30/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/30/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/30/delay",				{CHDE}, F_FND, {0}},
		{"/ch/30/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/30/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/30/insert",				{CHIN}, F_FND, {0}},
		{"/ch/30/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/30/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/30/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/30/gate",					{CHGA}, F_FND, {0}},
		{"/ch/30/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/30/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/30/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/30/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/30/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/30/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/30/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/30/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/30/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/30/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/30/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/30/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/30/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/30/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/30/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/30/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/30/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/30/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/30/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/30/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/30/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/30/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/30/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/30/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/30/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/30/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/30/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/30/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/30/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/30/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/30/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/30/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/30/eq",					{OFFON}, F_FND, {1}},
		{"/ch/30/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/30/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/30/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/30/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/30/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/30/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/30/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/30/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/30/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/30/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/30/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/30/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/30/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/30/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/30/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/30/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/30/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/30/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/30/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/30/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/30/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/30/mix",					{CHMX}, F_FND, {0}},
		{"/ch/30/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/30/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/30/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/30/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/30/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/30/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/30/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/30/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/30/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/30/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/30/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/30/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/30/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/30/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/30/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/30/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/30/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/30/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/30/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/30/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/30/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/30/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/30/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/30/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/30/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/30/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/30/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/30/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/30/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/30/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/30/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/30/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/30/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/30/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/30/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/30/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/30/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/30/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/30/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/30/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/30/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/30/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/30/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/30/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/30/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/30/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/30/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/30/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/30/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/30/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/30/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/30/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/30/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/30/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/30/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/30/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/30/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/30/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/30/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/30/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/30/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/30/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/30/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/30/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/30/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/30/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/30/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/30/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/30/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/30/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel30_max = sizeof(Xchannel30) / sizeof(X32command);


X32command Xchannel31[] = {
		{"/ch/31",						{CHCO}, F_FND, {0}},
		{"/ch/31/config",				{CHCO}, F_FND, {0}},
		{"/ch/31/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/31/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/31/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/31/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/31/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/31/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/31/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/31/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/31/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/31/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/31/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/31/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/31/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/31/delay",				{CHDE}, F_FND, {0}},
		{"/ch/31/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/31/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/31/insert",				{CHIN}, F_FND, {0}},
		{"/ch/31/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/31/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/31/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/31/gate",					{CHGA}, F_FND, {0}},
		{"/ch/31/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/31/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/31/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/31/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/31/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/31/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/31/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/31/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/31/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/31/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/31/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/31/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/31/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/31/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/31/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/31/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/31/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/31/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/31/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/31/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/31/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/31/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/31/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/31/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/31/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/31/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/31/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/31/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/31/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/31/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/31/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/31/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/31/eq",					{OFFON}, F_FND, {1}},
		{"/ch/31/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/31/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/31/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/31/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/31/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/31/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/31/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/31/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/31/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/31/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/31/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/31/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/31/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/31/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/31/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/31/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/31/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/31/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/31/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/31/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/31/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/31/mix",					{CHMX}, F_FND, {0}},
		{"/ch/31/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/31/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/31/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/31/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/31/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/31/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/31/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/31/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/31/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/31/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/31/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/31/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/31/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/31/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/31/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/31/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/31/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/31/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/31/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/31/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/31/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/31/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/31/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/31/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/31/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/31/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/31/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/31/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/31/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/31/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/31/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/31/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/31/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/31/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/31/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/31/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/31/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/31/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/31/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/31/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/31/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/31/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/31/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/31/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/31/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/31/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/31/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/31/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/31/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/31/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/31/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/31/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/31/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/31/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/31/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/31/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/31/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/31/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/31/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/31/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/31/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/31/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/31/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/31/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/31/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/31/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/31/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/31/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/31/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/31/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel31_max = sizeof(Xchannel31) / sizeof(X32command);


X32command Xchannel32[] = {
		{"/ch/32",						{CHCO}, F_FND, {0}},
		{"/ch/32/config",				{CHCO}, F_FND, {0}},
		{"/ch/32/config/name",		{S32}, F_XET, {0}},			// 1
		{"/ch/32/config/icon",		{I32}, F_XET, {0}},			// 2
		{"/ch/32/config/color",		{I32}, F_XET, {0}},			// 3
		{"/ch/32/config/source",	{I32}, F_XET, {0}},			// 4
		{"/ch/32/grp",					{CHGRP}, F_FND, {0}},
		{"/ch/32/grp/dca",			{I32}, F_XET, {0}},			// 6
		{"/ch/32/grp/mute",			{I32}, F_XET, {0}},			// 7
		{"/ch/32/preamp",				{CHPR}, F_FND, {0}},
		{"/ch/32/preamp/trim",		{F32}, F_XET, {0}},			// 9
		{"/ch/32/preamp/invert",	{I32}, F_XET, {0}},			// 10
		{"/ch/32/preamp/hpon",		{I32}, F_XET, {0}},			// 11
		{"/ch/32/preamp/hpslope",	{I32}, F_XET, {0}},			// 12
		{"/ch/32/preamp/hpf",		{F32}, F_XET, {0}},			// 13
		{"/ch/32/delay",				{CHDE}, F_FND, {0}},
		{"/ch/32/delay/on",			{I32}, F_XET, {0}},			// 15
		{"/ch/32/delay/time",		{I32}, F_XET, {0}},			// 16
		{"/ch/32/insert",				{CHIN}, F_FND, {0}},
		{"/ch/32/insert/on",		{I32}, F_XET, {0}},			// 17
		{"/ch/32/insert/pos",		{I32}, F_XET, {0}},			// 18
		{"/ch/32/insert/sel",		{I32}, F_XET, {0}},			// 19
		{"/ch/32/gate",					{CHGA}, F_FND, {0}},
		{"/ch/32/gate/on",			{I32}, F_XET, {0}},			// 21
		{"/ch/32/gate/mode",		{I32}, F_XET, {0}},			// 22
		{"/ch/32/gate/thr",			{F32}, F_XET, {0}},			// 23
		{"/ch/32/gate/range",		{F32}, F_XET, {0}},			// 24
		{"/ch/32/gate/attack",		{F32}, F_XET, {0}},			// 25
		{"/ch/32/gate/hold",		{F32}, F_XET, {0}},			// 26
		{"/ch/32/gate/release",		{F32}, F_XET, {0}},			// 27
		{"/ch/32/gate/keysrc",		{I32}, F_XET, {0}},			// 28
		{"/ch/32/gate/filter",			{CHGF}, F_FND, {0}},
		{"/ch/32/gate/filter/on",	{I32}, F_XET, {0}},			// 30
		{"/ch/32/gate/filter/type",	{I32}, F_XET, {0}},			// 31
		{"/ch/32/gate/filter/f",	{F32}, F_XET, {0}},			// 32
		{"/ch/32/dyn",					{CHDY}, F_FND, {0}},
		{"/ch/32/dyn/on",			{I32}, F_XET, {0}},			// 34
		{"/ch/32/dyn/mode",			{I32}, F_XET, {0}},			// 35
		{"/ch/32/dyn/det",			{I32}, F_XET, {0}},			// 36
		{"/ch/32/dyn/env",			{I32}, F_XET, {0}},			// 37
		{"/ch/32/dyn/thr",			{F32}, F_XET, {0}},			// 38
		{"/ch/32/dyn/ratio",		{I32}, F_XET, {0}},			// 39
		{"/ch/32/dyn/knee",			{F32}, F_XET, {0}},			// 40
		{"/ch/32/dyn/mgain",		{F32}, F_XET, {0}},			// 41
		{"/ch/32/dyn/attack",		{F32}, F_XET, {0}},			// 42
		{"/ch/32/dyn/hold",			{F32}, F_XET, {0}},			// 43
		{"/ch/32/dyn/release",		{F32}, F_XET, {0}},			// 44
		{"/ch/32/dyn/pos",			{I32}, F_XET, {0}},			// 45
		{"/ch/32/dyn/keysrc",		{F32}, F_XET, {0}},			// 46
		{"/ch/32/dyn/mix",			{F32}, F_XET, {0}},			// 47
		{"/ch/32/dyn/auto",			{I32}, F_XET, {0}},			// 48
		{"/ch/32/dyn/filter",			{CHDF}, F_FND, {0}},
		{"/ch/32/dyn/filter/on",	{I32}, F_XET, {0}},			// 50
		{"/ch/32/dyn/filter/type",	{I32}, F_XET, {0}},			// 51
		{"/ch/32/dyn/filter/f",		{F32}, F_XET, {0}},			// 52
		{"/ch/32/eq",					{OFFON}, F_FND, {1}},
		{"/ch/32/eq/on",			{I32}, F_XET, {0}},			// 54
		{"/ch/32/eq/1",					{CHEQ}, F_FND, {0}},
		{"/ch/32/eq/1/type",		{I32}, F_XET, {0}},			// 56
		{"/ch/32/eq/1/f",			{F32}, F_XET, {0}},			// 57
		{"/ch/32/eq/1/g",			{F32}, F_XET, {0}},			// 58
		{"/ch/32/eq/1/q",			{F32}, F_XET, {0}},			// 59
		{"/ch/32/eq/2",					{CHEQ}, F_FND, {0}},
		{"/ch/32/eq/2/type",		{I32}, F_XET, {0}},			// 61
		{"/ch/32/eq/2/f",			{F32}, F_XET, {0}},			// 62
		{"/ch/32/eq/2/g",			{F32}, F_XET, {0}},			// 63
		{"/ch/32/eq/2/q",			{F32}, F_XET, {0}},			// 64
		{"/ch/32/eq/3",					{CHEQ}, F_FND, {0}},
		{"/ch/32/eq/3/type",		{I32}, F_XET, {0}},			// 66
		{"/ch/32/eq/3/f",			{F32}, F_XET, {0}},			// 67
		{"/ch/32/eq/3/g",			{F32}, F_XET, {0}},			// 68
		{"/ch/32/eq/3/q",			{F32}, F_XET, {0}},			// 69
		{"/ch/32/eq/4",					{CHEQ}, F_FND, {0}},
		{"/ch/32/eq/4/type",		{I32}, F_XET, {0}},			// 71
		{"/ch/32/eq/4/f",			{F32}, F_XET, {0}},			// 72
		{"/ch/32/eq/4/g",			{F32}, F_XET, {0}},			// 73
		{"/ch/32/eq/4/q",			{F32}, F_XET, {0}},			// 74
		{"/ch/32/mix",					{CHMX}, F_FND, {0}},
		{"/ch/32/mix/on",			{I32}, F_XET, {0}},			// 76
		{"/ch/32/mix/fader",		{F32}, F_XET, {0}},			// 77
		{"/ch/32/mix/st",			{I32}, F_XET, {0}},			// 78
		{"/ch/32/mix/pan",			{F32}, F_XET, {0}},			// 79
		{"/ch/32/mix/mono",			{I32}, F_XET, {0}},			// 80
		{"/ch/32/mix/mlevel",		{F32}, F_XET, {0}},			// 81
		{"/ch/32/mix/01",				{CHMO}, F_FND, {0}},
		{"/ch/32/mix/01/on",		{I32}, F_XET, {0}},			// 83
		{"/ch/32/mix/01/level",		{F32}, F_XET, {0}},			// 84
		{"/ch/32/mix/01/pan",		{F32}, F_XET, {0}},			// 85
		{"/ch/32/mix/01/type",		{I32}, F_XET, {0}},			// 86
		{"/ch/32/mix/02",				{CHME}, F_FND, {0}},
		{"/ch/32/mix/02/on",		{I32}, F_XET, {0}},			// 88
		{"/ch/32/mix/02/level",		{F32}, F_XET, {0}},			// 89
		{"/ch/32/mix/03",				{CHMO}, F_FND, {0}},
		{"/ch/32/mix/03/on",		{I32}, F_XET, {0}},			// 91
		{"/ch/32/mix/03/level",		{F32}, F_XET, {0}},			// 92
		{"/ch/32/mix/03/pan",		{F32}, F_XET, {0}},			// 93
		{"/ch/32/mix/03/type",		{I32}, F_XET, {0}},			// 94
		{"/ch/32/mix/04",				{CHME}, F_FND, {0}},
		{"/ch/32/mix/04/on",		{I32}, F_XET, {0}},			// 96
		{"/ch/32/mix/04/level",		{F32}, F_XET, {0}},			// 97
		{"/ch/32/mix/05",				{CHMO}, F_FND, {0}},
		{"/ch/32/mix/05/on",		{I32}, F_XET, {0}},			// 99
		{"/ch/32/mix/05/level",		{F32}, F_XET, {0}},			// 100
		{"/ch/32/mix/05/pan",		{F32}, F_XET, {0}},			// 101
		{"/ch/32/mix/05/type",		{I32}, F_XET, {0}},			// 102
		{"/ch/32/mix/06",				{CHME}, F_FND, {0}},
		{"/ch/32/mix/06/on",		{I32}, F_XET, {0}},			// 104
		{"/ch/32/mix/06/level",		{F32}, F_XET, {0}},			// 105
		{"/ch/32/mix/07",				{CHMO}, F_FND, {0}},
		{"/ch/32/mix/07/on",		{I32}, F_XET, {0}},			// 107
		{"/ch/32/mix/07/level",		{F32}, F_XET, {0}},			// 108
		{"/ch/32/mix/07/pan",		{F32}, F_XET, {0}},			// 109
		{"/ch/32/mix/07/type",		{I32}, F_XET, {0}},			// 110
		{"/ch/32/mix/08",				{CHME}, F_FND, {0}},
		{"/ch/32/mix/08/on",		{I32}, F_XET, {0}},			// 112
		{"/ch/32/mix/08/level",		{F32}, F_XET, {0}},			// 113
		{"/ch/32/mix/09",				{CHMO}, F_FND, {0}},
		{"/ch/32/mix/09/on",		{I32}, F_XET, {0}},			// 115
		{"/ch/32/mix/09/level",		{F32}, F_XET, {0}},			// 116
		{"/ch/32/mix/09/pan",		{F32}, F_XET, {0}},			// 117
		{"/ch/32/mix/09/type",		{I32}, F_XET, {0}},			// 118
		{"/ch/32/mix/10",				{CHME}, F_FND, {0}},
		{"/ch/32/mix/10/on",		{I32}, F_XET, {0}},			// 120
		{"/ch/32/mix/10/level",		{F32}, F_XET, {0}},			// 121
		{"/ch/32/mix/11",				{CHMO}, F_FND, {0}},
		{"/ch/32/mix/11/on",		{I32}, F_XET, {0}},			// 123
		{"/ch/32/mix/11/level",		{F32}, F_XET, {0}},			// 124
		{"/ch/32/mix/11/pan",		{F32}, F_XET, {0}},			// 125
		{"/ch/32/mix/11/type",		{I32}, F_XET, {0}},			// 126
		{"/ch/32/mix/12",				{CHME}, F_FND, {0}},
		{"/ch/32/mix/12/on",		{I32}, F_XET, {0}},			// 128
		{"/ch/32/mix/12/level",		{F32}, F_XET, {0}},			// 129
		{"/ch/32/mix/13",				{CHMO}, F_FND, {0}},
		{"/ch/32/mix/13/on",		{I32}, F_XET, {0}},			// 131
		{"/ch/32/mix/13/level",		{F32}, F_XET, {0}},			// 132
		{"/ch/32/mix/13/pan",		{F32}, F_XET, {0}},			// 133
		{"/ch/32/mix/13/type",		{I32}, F_XET, {0}},			// 134
		{"/ch/32/mix/14",				{CHME}, F_FND, {0}},
		{"/ch/32/mix/14/on",		{I32}, F_XET, {0}},			// 136
		{"/ch/32/mix/14/level",		{F32}, F_XET, {0}},			// 137
		{"/ch/32/mix/15",				{CHMO}, F_FND, {0}},
		{"/ch/32/mix/15/on",		{I32}, F_XET, {0}},			// 139
		{"/ch/32/mix/15/level",		{F32}, F_XET, {0}},			// 140
		{"/ch/32/mix/15/pan",		{F32}, F_XET, {0}},			// 141
		{"/ch/32/mix/15/type",		{I32}, F_XET, {0}},			// 142
		{"/ch/32/mix/16",				{CHME}, F_FND, {0}},
		{"/ch/32/mix/16/on",		{I32}, F_XET, {0}},			// 144
		{"/ch/32/mix/16/level",		{F32}, F_XET, {0}},			// 145
};
int 	Xchannel32_max = sizeof(Xchannel32) / sizeof(X32command);


X32command	*Xchannelset[32] = {
		Xchannel01, Xchannel02, Xchannel03, Xchannel04,
		Xchannel05, Xchannel06, Xchannel07, Xchannel08,
		Xchannel09, Xchannel10, Xchannel11, Xchannel12,
		Xchannel13, Xchannel14, Xchannel15, Xchannel16,
		Xchannel17, Xchannel18, Xchannel19, Xchannel20,
		Xchannel21, Xchannel22, Xchannel23, Xchannel24,
		Xchannel25, Xchannel26, Xchannel27, Xchannel28,
		Xchannel29, Xchannel30, Xchannel31, Xchannel32,
};



#endif /* X32CHANNEL_H_ */
