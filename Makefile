CC=clang
CFLAGS=-O0 -Wall -Wpedantic -std=gnu11 -fsanitize=undefined -fsanitize=address -ggdb3

TESTS = test_dyar1 \
		test_dyar2 \
		test_dyar3 \
		test_dyar4 \
		test_dyar5 \
		test_dyar6 \
		test_dyar7 \
		test_dyar8


.PHONY: clean all

all: $(TESTS)

test_dyar1: test_dyar1.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)
test_dyar2: test_dyar2.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)
test_dyar3: test_dyar3.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)
test_dyar4: test_dyar4.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)
test_dyar5: test_dyar5.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)
test_dyar6: test_dyar6.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)
test_dyar7: test_dyar7.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)
test_dyar8: test_dyar8.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)

clean:
	@rm -f $(TESTS)
