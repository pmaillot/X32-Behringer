/*
 * X32Dca.h
 *
 *  Created on: 4 févr. 2015
 *      Author: patrick
 */

#ifndef X32DCA_H_
#define X32DCA_H_

X32command Xdca[] = {
		{"/dca",						{CHME}, F_FND, {0}},
		{"/dca/1",						{CHME}, F_FND, {0}},
		{"/dca/1/on",				{I32}, F_XET, {0}},
		{"/dca/1/fader",			{F32}, F_XET, {0}},
		{"/dca/1/config",				{BSCO}, F_FND, {0}},
		{"/dca/1/config/name",		{S32}, F_XET, {0}},
		{"/dca/1/config/icon",		{I32}, F_XET, {0}},
		{"/dca/1/config/color",		{I32}, F_XET, {0}},
		{"/dca/2",						{CHME}, F_FND, {0}},
		{"/dca/2/on",				{I32}, F_XET, {0}},
		{"/dca/2/fader",			{F32}, F_XET, {0}},
		{"/dca/2/config",				{BSCO}, F_FND, {0}},
		{"/dca/2/config/name",		{S32}, F_XET, {0}},
		{"/dca/2/config/icon",		{I32}, F_XET, {0}},
		{"/dca/2/config/color",		{I32}, F_XET, {0}},
		{"/dca/3",						{CHME}, F_FND, {0}},
		{"/dca/3/on",				{I32}, F_XET, {0}},
		{"/dca/3/fader",			{F32}, F_XET, {0}},
		{"/dca/3/config",				{BSCO}, F_FND, {0}},
		{"/dca/3/config/name",		{S32}, F_XET, {0}},
		{"/dca/3/config/icon",		{I32}, F_XET, {0}},
		{"/dca/3/config/color",		{I32}, F_XET, {0}},
		{"/dca/4",						{CHME}, F_FND, {0}},
		{"/dca/4/on",				{I32}, F_XET, {0}},
		{"/dca/4/fader",			{F32}, F_XET, {0}},
		{"/dca/4/config",				{BSCO}, F_FND, {0}},
		{"/dca/4/config/name",		{S32}, F_XET, {0}},
		{"/dca/4/config/icon",		{I32}, F_XET, {0}},
		{"/dca/4/config/color",		{I32}, F_XET, {0}},
		{"/dca/5",						{CHME}, F_FND, {0}},
		{"/dca/5/on",				{I32}, F_XET, {0}},
		{"/dca/5/fader",			{F32}, F_XET, {0}},
		{"/dca/5/config",				{BSCO}, F_FND, {0}},
		{"/dca/5/config/name",		{S32}, F_XET, {0}},
		{"/dca/5/config/icon",		{I32}, F_XET, {0}},
		{"/dca/5/config/color",		{I32}, F_XET, {0}},
		{"/dca/6",						{CHME}, F_FND, {0}},
		{"/dca/6/on",				{I32}, F_XET, {0}},
		{"/dca/6/fader",			{F32}, F_XET, {0}},
		{"/dca/6/config",				{BSCO}, F_FND, {0}},
		{"/dca/6/config/name",		{S32}, F_XET, {0}},
		{"/dca/6/config/icon",		{I32}, F_XET, {0}},
		{"/dca/6/config/color",		{I32}, F_XET, {0}},
		{"/dca/7",						{CHME}, F_FND, {0}},
		{"/dca/7/on",				{I32}, F_XET, {0}},
		{"/dca/7/fader",			{F32}, F_XET, {0}},
		{"/dca/7/config",				{BSCO}, F_FND, {0}},
		{"/dca/7/config/name",		{S32}, F_XET, {0}},
		{"/dca/7/config/icon",		{I32}, F_XET, {0}},
		{"/dca/7/config/color",		{I32}, F_XET, {0}},
		{"/dca/8",						{CHME}, F_FND, {0}},
		{"/dca/8/on",				{I32}, F_XET, {0}},
		{"/dca/8/fader",			{F32}, F_XET, {0}},
		{"/dca/8/config",				{BSCO}, F_FND, {0}},
		{"/dca/8/config/name",		{S32}, F_XET, {0}},
		{"/dca/8/config/icon",		{I32}, F_XET, {0}},
		{"/dca/8/config/color",		{I32}, F_XET, {0}},
};
int Xdca_max = sizeof(Xdca) / sizeof(X32command);

#endif /* X32DCA_H_ */
