; Programa para calcular a potência
; Calcula BASE^EXP = BASE * BASE * BASE * ... (EXP vezes)

INICIO:
    INPUT BASE       ; Lê a base
    INPUT EXP        ; Lê o expoente
    
    LOAD EXP
    JMPZ ZERO_EXP    ; Se EXP = 0, resultado = 1
    
    LOAD BASE        ; Inicializa resultado = BASE
    STORE RESULTADO
    
    LOAD EXP         ; Inicializa contador = EXP - 1
    SUB UM
    STORE CONTADOR

LOOP:
    LOAD CONTADOR
    JMPZ FIM         ; Se contador = 0, termina
    
    LOAD RESULTADO   ; resultado = resultado * BASE
    MULT BASE
    STORE RESULTADO
    
    LOAD CONTADOR    ; contador = contador - 1
    SUB UM
    STORE CONTADOR
    
    JMP LOOP

ZERO_EXP:
    LOAD UM          ; BASE^0 = 1
    STORE RESULTADO

FIM:
    OUTPUT RESULTADO
    STOP

; Variáveis
BASE: SPACE
EXP: SPACE
RESULTADO: SPACE
CONTADOR: SPACE
UM: CONST 1
