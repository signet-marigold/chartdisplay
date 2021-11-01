# General settings
CC = g++
IDIR = include
ODIR = build
SDIR = src
# Compiler flags
CFLAGS = -std=c++1z -g
# Linker flags
LLIBS = -lSDL2 -lSDL2_image -lSDL2_ttf -ljson-c
LDFLAGS =
# Application name
TARGET = chartdisplay

# -------------------------------------------------------------

_OBJ = $(patsubst %.c,%.o,$(wildcard $(SDIR)/*.c)) $(patsubst %.cpp,%.o,$(wildcard $(SDIR)/*.cpp))
OBJ = $(patsubst $(SDIR)/%,$(ODIR)/%,$(_OBJ))
DEPENDS = $(patsubst %.o,%.d,$(OBJ))

HDR = $(wildcard $(IDIR)/*.h) $(wildcard $(IDIR)/*.hpp)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LLIBS)

-include $(DEPENDS)

$(ODIR)/%.o : $(SDIR)/%.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ -I$(IDIR)

$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@ -I$(IDIR)


.PHONY : all clean

all : $(BDIR)/$(TARGET)

clean :
	rm -f $(ODIR)/*.o $(ODIR)/*.d $(TARGET) ChartDisplay.log
