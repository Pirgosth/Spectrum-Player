CC=g++
CFLAGS=-Wall -O
LDFLAGS=-lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -lmpg123
EXEC=spectrum
OBJDIR=obj
TARGET=target
RESOURCES=Resources
SRC=$(wildcard *.cpp)
OBJS=$(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))

all: $(EXEC)

$(EXEC): $(TARGET) copyResources $(OBJDIR) $(OBJS)
	$(CC) -o $(TARGET)/$@ $(OBJS) $(LDFLAGS)

copyResources:
	rsync -rupE $(RESOURCES) $(TARGET)/

$(TARGET):
	mkdir -p $(TARGET)

$(OBJS): $(OBJDIR)/%.o: %.cpp
	$(CC) -o $@ -c $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJDIR)/*.o

mrproper: clean
	rm -f $(TARGET)/$(EXEC)