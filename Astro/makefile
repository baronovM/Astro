SFML_LIBS= -L'c:\SFML-2.5.1\lib' -lsfml-main -lsfml-system -lsfml-window -lsfml-graphics
#SFML_LIBS= -L'c:\SFML-2.5.1\lib' -lsfml-main -lsfml-system -lsfml-graphics -lvorbis -L'c:\SFML-2.5.1\bin'
SFML_INCL= -I'c:\SFML-2.5.1\include'

.PHONY: all

all: distorce.exe reverse_distorce.exe proizv.exe

#NumColor.o: NumColor.hpp makefile NumColor.cpp
#	g++ $< $(SFML_INCL) -c -o $@

#distorce.exe reverse_distorce.exe: %.exe : %.cpp NumColor.o NumColor.hpp makefile
#	g++ -Wl,-r $< $(SFML_INCL)   -o $@ $(SFML_LIBS) NumColor.o 'C:\SFML-2.5.1\lib\libsfml-main.a'

distorce.exe reverse_distorce.exe proizv.exe TestNLopt.exe: %.exe : %.cpp makefile
	g++ -ggdb $< $(SFML_INCL)   -o $@ $(SFML_LIBS) 'C:\SFML-2.5.1\lib\libsfml-main.a'


#distorce.exe: distorce.cpp Makefile
#	g++ '$<' $(SFML_INCL)  -o $@ $(SFML_LIBS)
