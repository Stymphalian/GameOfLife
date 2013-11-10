CC=g++
CFLAGS=-g -c -ansi -Wall
INFLAGS=-Iinclude -Iallegro\include
LDFLAGS=-L. -Lallegro\lib -lallegro-5.0.10-monolith-mt-debug -lopengl32 -lglu32

#INFLAGS=-Iinclude -IC:\allegro\include
#LDFLAGS=-L. -LC:\allegro\lib -lallegro-5.0.10-monolith-mt-debug -lopengl32 -lglu32



OBJDIR:=objdir
SOURCES=main.cpp Model.cpp AllegroShell.cpp View.cpp
OBJECTS=$(addprefix $(OBJDIR)\, $(SOURCES:.cpp=.o) )

EXECUTABLE=noin

#-g  allow for debug mode 
#-E  macro expanded

all: $(SOURCES) $(EXECUTABLE)

$(OBJDIR)\\%.o: %.cpp
	$(CC) $(CFLAGS) $(INFLAGS) $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)


# Setup objects directory
$(OBJECTS): | $(OBJDIR)
$(OBJDIR):
	mkdir $(OBJDIR)

# clean methods
clean:
	rmdir /s /q $(OBJDIR) 2> nul & del noin.exe  & del *~
