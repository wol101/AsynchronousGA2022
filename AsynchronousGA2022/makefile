# get the system name
SYSTEM = $(shell uname)

# change this in shell to -g for debug
OPT_FLAGS ?= -O3 -ffast-math
EXTRA_FLAGS ?= -std=c++17

ifeq ($(SYSTEM),Linux)
# GNU compiler flags 
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
LIBS += 
endif

ifeq ($(SYSTEM),Darwin)
# clang compiler flags 
CXXFLAGS    = -Wall -fexceptions $(OPT_FLAGS) $(EXTRA_FLAGS)
LIBS += 
endif


INC_DIRS += \
-Iasio-1.18.2/include \
-Iexprtk \
-Ipystring \
-Irapidxml-1.13

SRC = \
ArgParse.cpp \
DataFile.cpp \
GAASIO.cpp \
Genome.cpp \
MD5.cpp \
Mating.cpp \
Population.cpp \
Preferences.cpp \
Random.cpp \
ServerASIO.cpp \
Statistics.cpp \
XMLConverter.cpp 

PYSTRINGSRC = \
pystring.cpp


OBJ = $(addsuffix .o, $(basename $(SRC) ) )
PYSTRINGOBJ = $(addsuffix .o, $(basename $(PYSTRINGSRC) ) )
ENETOBJ = $(addsuffix .o, $(basename $(ENETSRC) ) )

BINARIES = bin/AsynchronousGA2022

all: directories binaries 

directories: bin obj 

binaries: $(BINARIES)

obj: 
	-mkdir obj
	-mkdir obj/src
	-mkdir obj/pystring

bin:
	-mkdir bin

obj/pystring/%.o : pystring/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@


obj/src/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c $< -o $@

bin/AsynchronousGA2022: $(addprefix obj/src/, $(OBJ) ) $(addprefix obj/pystring/, $(PYSTRINGOBJ) )
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o nohup.out
	rm -rf build*

superclean:
	rm -rf obj bin
	rm -f *~ *.bak *.bck *.tmp *.o 
	rm -rf build*
	rm -rf AsynchronousGA2022Qt/AsynchronousGA2022Qt*.pro.user*
	find . -name '.DS_Store' -exec rm -f {} \;
	find . -name '.gdb_history' -exec rm -f {} \;
	find . -name '.#*' -exec rm -f {} \;
	find . -name '*~' -exec rm -f {} \;
	find . -name '#*' -exec rm -f {} \;
	find . -name '*.bak' -exec rm -f {} \;
	find . -name '*.bck' -exec rm -f {} \;
	find . -name '*.tmp' -exec rm -f {} \;
	find . -name '*.o' -exec rm -f {} \;

