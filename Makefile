CC=clang
CFLAGS=-Ofast -Wall -Wpedantic -std=gnu11 -fsanitize=undefined -fsanitize=address -ggdb3

TESTS = test_dyar1 \
		test_dyar2 \
		test_dyar3 \
		test_dyar4 \
		test_dyar5


.PHONY: clean all

#all: bench test_rbtree
all: $(TESTS)

test_dyar1: test_dyar1.c dyar.h
	$(CC) $< -o $@ $(CFLAGS)
test_dyar2: test_dyar2.c dyar.h
	$(CC) $< -o $@ $(CFLAGS)
test_dyar3: test_dyar3.c dyar.h
	$(CC) $< -o $@ $(CFLAGS)
test_dyar4: test_dyar4.c dyar.h
	$(CC) $< -o $@ $(CFLAGS)
test_dyar5: test_dyar5.c dyar.h
	$(CC) $< -o $@ $(CFLAGS)

clean:
	rm -rf $(TESTS)
