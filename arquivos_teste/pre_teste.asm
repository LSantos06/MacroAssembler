L1: eqU 1
L2: eqU 0

SECTION TEXT
iF l1
LOAD l1		;faz esta operaçao se L1 for verdadeiro
iF L2
INPUT N  	;faz esta operaçao se L2 for verdadeiro

SECTION DATA
N: SPACE L1
Z: CONST L2