; Programa para calcular o somatório de 1 até N
; Soma = 1 + 2 + 3 + ... + N

INICIO:
    INPUT N          ; Lê o número N
    
    LOAD ZERO        ; Inicializa soma = 0
    STORE SOMA
    
    LOAD UM          ; Inicializa contador = 1
    STORE CONTADOR

LOOP:
    LOAD CONTADOR
    SUB N
    JMPP FIM         ; Se contador > N, termina
    
    LOAD SOMA        ; soma = soma + contador
    ADD CONTADOR
    STORE SOMA
    
    LOAD CONTADOR    ; contador = contador + 1
    ADD UM
    STORE CONTADOR
    
    JMP LOOP

FIM:
    OUTPUT SOMA
    STOP

; Variáveis
N: SPACE
SOMA: SPACE
CONTADOR: SPACE
ZERO: CONST 0
UM: CONST 1
