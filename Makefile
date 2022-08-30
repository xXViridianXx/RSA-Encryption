CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra -g $(shell pkg-config --cflags gmp)
LFLAGS = $(shell pkg-config --libs gmp)
HEADERS = $(wildcard *.h)
MODULES = numtheory randstate rsa
OBJECTS_KEY = $(KEYGEN).o $(patsubst %, %.o, ${MODULES})
OBJECTS_ENCRY = $(ENCRYPT).o $(patsubst %, %.o, ${MODULES})
OBJECTS_DECRY = $(DECRYPT).o $(patsubst %, %.o, ${MODULES})
KEYGEN = keygen
ENCRYPT = encrypt
DECRYPT = decrypt


.PHONY: all done

all: KEYGEN ENCRYPT DECRYPT 


KEYGEN: $(KEYGEN)

ENCRYPT: $(ENCRYPT)

DECRYPT: $(DECRYPT)

$(KEYGEN): $(OBJECTS_KEY)
	$(CC) -o $@ $^ $(LFLAGS) -lm

$(ENCRYPT): $(OBJECTS_ENCRY)
	$(CC) -o $@ $^ $(LFLAGS) -lm

$(DECRYPT): $(OBJECTS_DECRY)
	$(CC) -o $@ $^ $(LFLAGS) -lm

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -c 

clean: tidy
	$(RM) $(KEYGEN) $(ENCRYPT) $(DECRYPT)

tidy:
	$(RM) $(OBJECTS_KEY) $(OBJECTS_ENCRY) $(OBJECTS_DECRY)

format:
	clang-format -i -style=file *.[c,h]

