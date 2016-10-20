BISONSRC = parser.y
BISONHDR = parser.h
BISONCPP = parser.cpp
BISON    = bison --defines=${BISONHDR} --output=${BISONCPP}

FLEXSRC  = tokenizer.l
FLEXHDR  = tokenizer.h
FLEXCPP  = tokenizer.cpp
FLEX     = flex --header-file=${FLEXHDR} --outfile=${FLEXCPP}

EXE      = fpli
CPP      = g++ -Wall -std=gnu++11 -g
SRC      = tokenhelper.cpp parserhelper.cpp main.cpp ast.cpp composition.cpp execution.cpp native_functions.cpp
ALLSRC   = ${SRC} ${BISONCPP} ${FLEXCPP}
OBJ      = ${ALLSRC:.cpp=.o}


all : ${EXE}

${EXE} : ${OBJ}
	${CPP} -o${EXE} ${OBJ}
	cp ${EXE} ..

${OBJ} : ${ALLSRC}
	${CPP} -c ${ALLSRC}

${FLEXCPP} : ${FLEXSRC}
	${FLEX} ${FLEXSRC}

${BISONCPP} : ${BISONSRC}
	${BISON} ${BISONSRC}

clean :
	rm ${OBJ} ${BISONCPP} ${BISONHDR} ${FLEXCPP} ${FLEXHDR} parser.output

spotless : clean
	rm ${EXE}