;section text faltante
;SECTION TEXT
COPY ZERO,OLDER
COPY ONE,OLD
INPUT 2LIMIT ;token invalido
OUTPUT OLD
FRONT: GO: LOAD OLDER ;2 rotulos na msm linha
ADD OLD
STORE NEW
SUB LIMIT
JMPP FINAL
OUTPUT NEW
COPY OLD,OLDER
COPY NEW,OLD
SRL NEW ;instrucao invalida
FINAL: OUTPUT LIMIT
; stop faltante (tirar outros erros para ver)
STOP
SECTION BSS ;secao invalida
SECTION DATA
JMP FRONT ;instrucoes na sessao errada
ZERO: CONST 0
ONE: CONST 1
OLDER: SPACE
OLD: SPACE
NEW: SPACE
NEW: CONST 1 ;declaracao repetida
LIMIT: SPACE
2LIMIT: SPACE
