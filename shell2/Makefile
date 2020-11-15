# the compiler
CC = gcc

# compiler flags:
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall

# the build target executable:
TARGET = shell
TARGET2 = myShell

all: $(TARGET) $(TARGET2)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

$(TARGET2): $(TARGET2).c
	$(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).c
	
clean:
	$(RM) $(TARGET)
	$(RM) $(TARGET2)
