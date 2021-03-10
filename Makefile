################################################################################
#Copyrights(c)2020 A.I.
#
#DESCRIPTION: Makefile for program sipadpp
#             
#
#HISTORY:
#  Name                 Date                Reason
#  A.I                  02/27/2020          Initial Version
#
#This software is the property of A.I., legally protected as the trade
#secret, proprietary information, and copyrighted work of A.I.. It may
#not be used, duplicated, disclosed to others outside A.I., modified,
#or translated, etc., except as specifically permitted by the software
#license term of an agreement with A.I..
################################################################################
include ../../libs/general/make.def
#include $(OCTEON_ROOT)/components/driver/driver.mk

INCS = -I. 

            
CFLAGS := -g -DUSER_LAYER_SOCKET -O2 -Wall -D__USE_POSIX -Wno-deprecated  \
          $(INCS) -D_FILE_OFFSET_BITS=64 -DSAVE_RAW_MSG -DSOCKET_IP_INFO -w#-m64
          
CXXFLAGS := $(CFLAGS)


LIBS = 	-L. \
	-lpthread \
	-lrt \
	-ldl \
	-lm \
	-lssl \
        -lcrypto 
	#-levent 
	#-L../libs/dbms_common/libSqliteWrapper \
	-ldbBaseItf \
           

ifeq ($(HOST_IS_OCTEON),1)
LIBS += --static
endif

APP = websocket_client
	
ESRCS  = base64.c sha1.c wsclient.c main.c cJSON.c cJSON_Utils.c
       
EOBJS  = ${ESRCS:.cpp=.o}


.cpp.o:
	$(CXX) $(CFLAGS) $(FLAGS) $(INCLUDE) -c $*.cpp

%.o:%.c
	$(CC) $(CFLAGS) $(FLAGS) $(INCLUDE) -c $*.c

$(APP): $(EOBJS)
	$(CC) $(CFLAGS) $(FLAGS) $(INCLUDE) -o $(APP) $(EOBJS) $(LIBS) 
	
clean:  
	- rm -f $(APP) *.o

