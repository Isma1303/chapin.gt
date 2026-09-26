# Makefile - compilador front-end de Chapin (.gt)
#
# Uso:
#   make          -> genera el binario ./chapin
#   make test     -> lo corre contra tests/valido.gt y tests/invalido.gt
#   make clean    -> borra binario y artefactos generados

CC      ?= cc
FLEX    ?= flex
BISON   ?= bison

CFLAGS  = -std=gnu11 -Wall -Wextra -g -Isrc -I$(BUILD_DIR)

SRC_DIR   = src
BUILD_DIR = build
BIN       = chapin

.PHONY: all test clean

all: $(BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/parser.tab.h: $(SRC_DIR)/parser.y | $(BUILD_DIR)
	$(BISON) -d -o $(BUILD_DIR)/parser.tab.c $(SRC_DIR)/parser.y

$(BUILD_DIR)/lex.yy.c: $(SRC_DIR)/lexer.l $(BUILD_DIR)/parser.tab.h | $(BUILD_DIR)
	$(FLEX) -o $(BUILD_DIR)/lex.yy.c $(SRC_DIR)/lexer.l

$(BIN): $(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/lex.yy.c $(SRC_DIR)/ast.c $(SRC_DIR)/symtab.c $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -o $@ $(BUILD_DIR)/parser.tab.c $(BUILD_DIR)/lex.yy.c $(SRC_DIR)/ast.c $(SRC_DIR)/symtab.c $(SRC_DIR)/main.c

test: $(BIN)
	@echo "== Programa valido (tests/valido.gt) =="
	./$(BIN) tests/valido.gt
	@echo
	@echo "== Programa invalido (tests/invalido.gt) =="
	-./$(BIN) tests/invalido.gt

clean:
	rm -rf $(BUILD_DIR) $(BIN) $(BIN).dSYM
