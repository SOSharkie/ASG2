# $Id: Makefile,v 1.14 2017-09-21 15:51:23-07 - - $
# Sean Odnert sodnert@ucsc.edu
# Mark Hiserodt mhiserod@ucsc.edu

GCC        = g++ -g -O0 -Wall -Wextra -std=gnu++14
MKDEP      = g++ -MM -std=gnu++14
VALGRIND   = valgrind --leak-check=full --show-reachable=yes

MKFILE     = Makefile
DEPFILE    = Makefile.dep
SOURCES    = string_set.cpp cppstrtok.cpp
HEADERS    = string_set.h
OBJECTS    = ${SOURCES:.cpp=.o}
EXECBIN    = oc
SRCFILES   = ${SOURCES} ${MKFILE}
SMALLFILES = ${DEPFILE} foo.oc foo1.oh foo2.oh
CHECKINS   = ${SRCFILES} ${SMALLFILES}
LISTING    = Listing.ps

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GCC} -o${EXECBIN} ${OBJECTS}

%.o : %.cpp
	${GCC} -c $<

ci :
	cid + ${CHECKINS}
	checksource ${CHECKINS}

clean :
	- rm ${OBJECTS}

spotless : clean
	- rm ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf} ${DEPFILE} \
	     test.out misc.lis

${DEPFILE} :
	${MKDEP} ${SOURCES} >${DEPFILE}

deps :
	- rm ${DEPFILE}
	${MAKE} --no-print-directory ${DEPFILE}

include Makefile.dep

# test : ${EXECBIN}
# 	${VALGRIND} ${EXECBIN} foo.oc 1>test.out 2>&1

# misc.lis : ${DEPFILE} foo.oc foo1.oh foo2.oh
# 	catnv ${DEPFILE} foo.oc foo1.oh foo2.oh >misc.lis

# lis : misc.lis test
# 	mkpspdf ${LISTING} ${SRCFILES} misc.lis test.out

# again :
# 	${MAKE} spotless dep all test lis
