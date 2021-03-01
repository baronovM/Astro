
SFML_LIBS= -L'c:\SFML-2.5.1\lib' -lsfml-main -lsfml-system -lsfml-window -lsfml-graphics -lvorbis
SFML_INCL= -I'c:\SFML-2.5.1\include'

distorce.exe reverse_distorce.exe: %.exe : %.cpp Makefile
	g++ $< $(SFML_INCL)  -o $@ $(SFML_LIBS)

#distorce.exe: distorce.cpp Makefile
#	g++ '$<' $(SFML_INCL)  -o $@ $(SFML_LIBS)