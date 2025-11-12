; Programa para calcular o fatorial de um número
; Calcula N! = N * (N-1) * (N-2) * ... * 1

INICIO:
    INPUT N          ; Lê o número N
    
    LOAD N
    JMPZ ZERO_FAT    ; Se N = 0, fatorial = 1
    
    LOAD UM          ; Inicializa resultado = 1
    STORE RESULTADO
    
    LOAD N           ; Inicializa contador = N
    STORE CONTADOR

LOOP:
    LOAD CONTADOR
    JMPZ FIM         ; Se contador = 0, termina
    
    LOAD RESULTADO   ; resultado = resultado * contador
    MULT CONTADOR
    STORE RESULTADO
    
    LOAD CONTADOR    ; contador = contador - 1
    SUB UM
    STORE CONTADOR
    
    JMP LOOP

ZERO_FAT:
    LOAD UM          ; 0! = 1
    STORE RESULTADO
    
FIM:
    OUTPUT RESULTADO
    STOP

; Variáveis
N: SPACE
RESULTADO: SPACE
CONTADOR: SPACE
UM: CONST 1
