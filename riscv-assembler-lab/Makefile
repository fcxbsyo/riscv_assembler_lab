CC     = gcc
CFLAGS = -Wall -Wextra -Isrc

SRC = src/main.c src/assembler.c src/parser.c \
      src/registers.c src/instr_table.c \
      src/symbols.c src/encode.c

# ── Main assembler binary ────────────────────────────────────────────────────
assembler: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o assembler

# ── Individual unit tests ────────────────────────────────────────────────────
test_registers: tests/test_registers.c src/registers.c
	$(CC) $(CFLAGS) $^ -o $@

test_parser: tests/test_parser.c src/parser.c
	$(CC) $(CFLAGS) $^ -o $@

test_instr_table: tests/test_instr_table.c src/instr_table.c
	$(CC) $(CFLAGS) $^ -o $@

test_symbols: tests/test_symbols.c src/symbols.c
	$(CC) $(CFLAGS) $^ -o $@

test_encode: tests/test_encode.c src/encode.c
	$(CC) $(CFLAGS) $^ -o $@

test_assembler: tests/test_assembler.c src/assembler.c src/parser.c \
                src/registers.c src/instr_table.c src/symbols.c src/encode.c
	$(CC) $(CFLAGS) $^ -o $@

# ── Run all tests ────────────────────────────────────────────────────────────
TESTS = test_registers test_parser test_instr_table test_symbols \
        test_encode test_assembler

test: $(TESTS)
	@echo ""
	@echo "=============================="
	@echo "  Running all unit tests"
	@echo "=============================="
	@total_pass=0; total_fail=0; \
	for t in $(TESTS); do \
	    echo ""; echo ">> $$t"; \
	    ./$$t; \
	    rc=$$?; \
	    if [ $$rc -ne 0 ]; then total_fail=$$((total_fail+1)); \
	    else total_pass=$$((total_pass+1)); fi; \
	done; \
	echo ""; echo "============================"; \
	echo "  Suites: $$total_pass passed, $$total_fail failed"; \
	echo "============================"; \
	[ $$total_fail -eq 0 ]

# ── Assemble example files ───────────────────────────────────────────────────
examples: assembler
	./assembler examples/test_case_1.s examples/test_case_1.hex
	./assembler examples/test_case_2.s examples/test_case_2.hex
	./assembler examples/test_case_3.s examples/test_case_3.hex

clean:
	rm -f assembler $(TESTS) /tmp/t*.s /tmp/t*.hex examples/*.hex

.PHONY: test examples clean