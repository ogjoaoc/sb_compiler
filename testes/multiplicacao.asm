; Programa para multiplicar dois números usando somas sucessivas
; Calcula A * B = A + A + A + ... (B vezes)

INICIO:
    INPUT A          ; Lê primeiro número
    INPUT B          ; Lê segundo número
    
    LOAD ZERO        ; Inicializa resultado = 0
    STORE RESULTADO
    
    LOAD B           ; Inicializa contador = B
    STORE CONTADOR

LOOP:
    LOAD CONTADOR
    JMPZ FIM         ; Se contador = 0, termina
    
    LOAD RESULTADO   ; resultado = resultado + A
    ADD A
    STORE RESULTADO
    
    LOAD CONTADOR    ; contador = contador - 1
    SUB UM
    STORE CONTADOR
    
    JMP LOOP

FIM:
    OUTPUT RESULTADO
    STOP

; Variáveis
A: SPACE
B: SPACE
RESULTADO: SPACE
CONTADOR: SPACE
ZERO: CONST 0
UM: CONST 1
