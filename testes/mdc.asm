; Programa para calcular o MDC (Máximo Divisor Comum)
; Usa o algoritmo de Euclides

INICIO:
    INPUT A          ; Lê primeiro número
    INPUT B          ; Lê segundo número

LOOP:
    LOAD B
    JMPZ FIM         ; Se B = 0, MDC = A
    
    ; Calcula resto de A / B
    LOAD A
    STORE TEMP_A
    
CALC_RESTO:
    LOAD TEMP_A
    SUB B
    JMPN ATUALIZA    ; Se TEMP_A < B, TEMP_A é o resto
    
    STORE TEMP_A
    JMP CALC_RESTO

ATUALIZA:
    LOAD TEMP_A      ; TEMP_A já é o resto correto
    STORE RESTO
    
    LOAD B           ; A = B
    STORE A
    
    LOAD RESTO       ; B = resto
    STORE B
    
    JMP LOOP

FIM:
    OUTPUT A         ; Imprime o MDC
    STOP

; Variáveis
A: SPACE
B: SPACE
TEMP_A: SPACE
RESTO: SPACE
