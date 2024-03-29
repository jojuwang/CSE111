# $Id: Makefile,v 1.19 2019-04-03 14:15:22-07 - - $

MKFILE      = Makefile
DEPFILE     = ${MKFILE}.dep
NOINCL      = ci clean spotless
NEEDINCL    = ${filter ${NOINCL}, ${MAKECMDGOALS}}
GMAKE       = ${MAKE} --no-print-directory
GPPWARN     = -Wall -Wextra -Werror -Wpedantic -Wshadow -Wold-style-cast
GPPOPTS     = ${GPPWARN} -fdiagnostics-color=never
COMPILECPP  = g++ -g -O0 -std=gnu++17 ${GPPOPTS}
MAKEDEPCPP  = g++ -MM -std=gnu++17 ${GPPOPTS}
LOGCOMMAND  = (echo ""; echo [COMMAND] "${2}"; ${2}) >>${1} 2>&1

CPPHEADER   = hello.h
CPPSOURCE   = hello.cpp main.cpp
ALLCPPSRC   = ${CPPHEADER} ${CPPSOURCE}
OBJECTS     = ${CPPSOURCE:.cpp=.o}
EXECBIN     = hello
OTHERS      = ${MKFILE}
ALLSOURCES  = ${ALLCPPSRC} ${OTHERS}
LISTING     = Listing.ps

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o $@ ${OBJECTS}

%.o : %.cpp
	- checksource $<
	- cpplint.py.perl $<
	${COMPILECPP} -c $<

ci : ${ALLSOURCES}
	- checksource ${ALLSOURCES}
	- cpplint.py.perl ${ALLCPPSRC}
	cid + ${ALLSOURCES}

lis : ${ALLSOURCES} test file
	mkpspdf ${LISTING} ${ALLSOURCES} ${DEPFILE} test.out files.log

clean :
	- rm ${OBJECTS} ${DEPFILE} core test.out files.log

spotless : clean
	- rm ${EXECBIN} ${LISTING} ${LISTING:.ps=.pdf}

test : ${EXECBIN}
	./${EXECBIN} foo bar baz qux >test.out 2>&1

file : ${EXECBIN}
	LC_TIME=C date >files.log
	${call LOGCOMMAND,files.log,checksource ${ALLSOURCES}}
	${call LOGCOMMAND,files.log,cpplint.py.perl ${ALLCPPSRC}}
	${call LOGCOMMAND,files.log,file *}


dep : ${ALLCPPSRC}
	@ echo "# ${DEPFILE} created `LC_TIME=C date`" >${DEPFILE}
	${MAKEDEPCPP} ${CPPSOURCE} >>${DEPFILE}

${DEPFILE} : ${MAKEFILE}
	@ touch ${DEPFILE}
	${GMAKE} dep

again :
	${GMAKE} spotless dep ci all lis

ifeq (${NEEDINCL}, )
include ${DEPFILE}
endif
