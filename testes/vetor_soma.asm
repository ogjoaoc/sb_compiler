; Programa para somar todos os elementos de um vetor
; Lê N números e calcula a soma total

INICIO:
    INPUT TAM        ; Lê o tamanho do vetor
    
    LOAD ZERO        ; Inicializa soma = 0
    STORE SOMA
    
    LOAD TAM         ; Inicializa contador = TAM
    STORE CONTADOR

LOOP_LER:
    LOAD CONTADOR
    JMPZ MOSTRAR     ; Se contador = 0, mostra resultado
    
    INPUT TEMP       ; Lê um número
    
    LOAD SOMA        ; soma = soma + TEMP
    ADD TEMP
    STORE SOMA
    
    LOAD CONTADOR    ; contador = contador - 1
    SUB UM
    STORE CONTADOR
    
    JMP LOOP_LER

MOSTRAR:
    OUTPUT SOMA      ; Imprime a soma total
    STOP

; Variáveis
TAM: SPACE
SOMA: SPACE
CONTADOR: SPACE
TEMP: SPACE
ZERO: CONST 0
UM: CONST 1
