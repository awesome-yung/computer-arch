CC=gcc
CFLAGS=-g -Wall -O0
OBJS=main.o reg.o
TARGET=main
SUBMIT = pa2-submission.tar.gz

all: $(TARGET)


$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

.PHONY: clean test help
clean:
	rm -rf $(OBJS) $(TARGET)

help:
	@echo "The following options are provided with Make\n\t-make:\t\tbuild simulator\n\t-make clean:\tclean the build\n\t-make test:\ttest your simulator"

test: $(TARGET) test_0 test_1 

test_0: $(TARGET)
	@echo "Testing example0"; \
	./$(TARGET) traces/sample_input_gcc | diff -Naur validation/sample_output_gcc - ;\
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_1: $(TARGET)
	@echo "Testing example1"; \
	./$(TARGET) -m 0x10000000:0x10000010 -n 50 sample_input/example1.o | diff -Naur sample_output/example1 - ;\
	if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi


submission:
	@echo "Generating a compressed file ($(SUBMIT)) including loader.c and proc.c"; \
	tar cvzf $(SUBMIT) loader.c proc.c

