MOD_B: BEGIN
	FAT: EXTERN
	N: EXTERN
	PUBLIC MOD_B
	SECTION TEXT
	STORE AUX ;0
	MULT N ;2
	STORE N ;4
	LOAD AUX ;6
	JMP FAT ;8
	SECTION DATA
	AUX: SPACE ;10
END
