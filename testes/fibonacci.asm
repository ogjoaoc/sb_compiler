; Programa para calcular a sequência de Fibonacci
; Calcula os primeiros N números da sequência

INICIO:
    INPUT N          ; Lê quantos números calcular
    
    LOAD ZERO        ; Inicializa F0 = 0
    STORE F0
    OUTPUT F0        ; Imprime F0
    
    LOAD UM          ; Inicializa F1 = 1
    STORE F1
    OUTPUT F1        ; Imprime F1
    
    LOAD N
    SUB DOIS         ; N - 2 (já imprimimos 2 números)
    STORE CONTADOR

LOOP:
    LOAD CONTADOR
    JMPZ FIM         ; Se contador = 0, termina
    
    LOAD F0          ; Calcula próximo Fibonacci
    ADD F1
    STORE TEMP
    OUTPUT TEMP      ; Imprime resultado
    
    LOAD F1          ; Atualiza F0 = F1
    STORE F0
    
    LOAD TEMP        ; Atualiza F1 = TEMP
    STORE F1
    
    LOAD CONTADOR    ; Decrementa contador
    SUB UM
    STORE CONTADOR
    
    JMP LOOP

FIM:
    STOP

; Variáveis
N: SPACE
F0: SPACE
F1: SPACE
TEMP: SPACE
CONTADOR: SPACE
ZERO: CONST 0
UM: CONST 1
DOIS: CONST 2
