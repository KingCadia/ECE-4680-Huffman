comp = gcc
comp_flags = -g -Wall
Bins = encode decode

all	: $(Bins) 

encode	:	encode.c huffmanEncode.c huffmanEncode.h
	$(comp) $(comp_flags) encode.c huffmanEncode.c -o encode

decode	:	decode.c 
	$(comp) $(comp_flags) decode.c -o decode

clean :
	rm $(Bins)
