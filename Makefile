CC = gcc
CFLAG = -g

all: master bin_adder

%.o: %.c
	$(CC) $(CFLAG) -c $< -o $@

master: master.o
	$(CC) $(CFLAG) $< -o $@ -lm

bin_adder: bin_adder.o
	$(CC) $(CFLAG) $< -o $@ -lm

clean:
	rm -f *.o master bin_adder 
	
