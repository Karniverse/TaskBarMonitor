CC = g++
CFLAGS = -fdiagnostics-color=always -g
LDFLAGS = -lgdi32

SRCS = TaskBarMonitor.cpp
OBJS = $(SRCS:.cpp=.o)
EXEC = TaskBarMonitor.exe

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
