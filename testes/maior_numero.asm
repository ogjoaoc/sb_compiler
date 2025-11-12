; Programa para encontrar o maior de 3 números

INICIO:
    INPUT NUM1       ; Lê primeiro número
    INPUT NUM2       ; Lê segundo número
    INPUT NUM3       ; Lê terceiro número
    
    LOAD NUM1        ; Assume NUM1 como maior
    STORE MAIOR
    
    ; Compara com NUM2
    LOAD NUM2
    SUB MAIOR
    JMPN COMP3       ; Se NUM2 < MAIOR, pula para comparar NUM3
    
    LOAD NUM2        ; NUM2 >= MAIOR
    STORE MAIOR
    
COMP3:
    ; Compara com NUM3
    LOAD NUM3
    SUB MAIOR
    JMPN FIM         ; Se NUM3 < MAIOR, termina
    
    LOAD NUM3        ; NUM3 >= MAIOR
    STORE MAIOR
    
FIM:
    OUTPUT MAIOR
    STOP

; Variáveis
NUM1: SPACE
NUM2: SPACE
NUM3: SPACE
MAIOR: SPACE
