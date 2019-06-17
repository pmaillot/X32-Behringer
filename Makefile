# Copyright (C) 2019 Javier Iglesias
#
# This program is free software; you can redistribute it and/or modify                       
# it under the terms of the GNU General Public License as published by                       
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.                                                        
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of                             
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                              
# GNU General Public License for more details.                                               
#                                                                                            
# You should have received a copy of the GNU General Public License                          
# along with this program; if not, write to the Free Software                                
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA                  
#                                                                                            
#
CC=gcc
RM=rm -rf
MKDIR=mkdir -p
BUILD=build

SRCS := $(shell find . -name '*.c')
OBJS := $(addsuffix .o,$(basename $(SRCS)))
LIBS := m X32 
DIRS := X32lib

CFLAGS=-O0 -g3 -Wall $(addprefix -I,$(DIRS))
LDFLAGS=$(addprefix -L,$(DIRS)) $(addprefix -l,$(LIBS))
TARGETS=X32lib X32Reaper X32Wav_Xlive


all: 	dirs $(TARGETS)

dirs:
	@$(MKDIR) $(BUILD)

X32Reaper: X32lib
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $@.c $(LDFLAGS)

X32_Command: #X32lib
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $@.c $(LDFLAGS)

X32Wav_Xlive: 
	$(CC) $(CFLAGS) -o $(BUILD)/$@ $@.c $(LDFLAGS)

$(DIRS):
	@$(MAKE) -C $@ all

%.o: 	%.c
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	$(RM) $(OBJS) $(BUILD)
	$(MKDIR) $(BUILD)
	$(MAKE) -C $(DIRS) clean

.PHONY:	$(DIRS)
