# the compiler
CC = gcc

# compiler flags:
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall -pthread

# the build target executable:
TARGET = barbershop
TARGET2 = searchinsertdelete
TARGET3 = senatebus


all: $(TARGET) $(TARGET2) $(TARGET3)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

$(TARGET2): $(TARGET2).c
	$(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).c

$(TARGET3): $(TARGET3).c
	$(CC) $(CFLAGS) -o $(TARGET3) $(TARGET3).c
	
clean:
	$(RM) $(TARGET)
	$(RM) $(TARGET2)
	$(RM) $(TARGET3)
