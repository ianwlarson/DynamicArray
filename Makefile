CC=gcc
CFLAGS=-O2 -Wall -Wpedantic -std=gnu11 -fsanitize=undefined -fsanitize=address -ggdb3 #-fprofile-arcs -ftest-coverage

TESTS = test_dyar1 \
		test_dyar2 \
		test_dyar3 \
		test_dyar4 \
		test_dyar5 \
		test_dyar6 \
		test_dyar7 \
		test_dyar8 \
		test_dyar9 \
		test_dyar10 \
		test_dyar11 \
		test_dyar12


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
test_dyar9: test_dyar9.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)
test_dyar10: test_dyar10.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)
test_dyar11: test_dyar11.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)
test_dyar12: test_dyar12.c dyar_check.c dyar_check.h dyar.h
	$(CC) $(filter %.c,$^) -o $@ $(CFLAGS)

clean:
	@rm -f $(TESTS)
	@rm -f *.gcno
	@rm -f *.gcda
	@rm -f *.info
