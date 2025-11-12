; Programa para verificar se um número é par ou ímpar
; Usa divisão por 2 e verifica o resto

INICIO:
    INPUT NUMERO     ; Lê o número
    
    LOAD NUMERO      ; Copia número para trabalhar
    STORE TEMP
    
    LOAD ZERO        ; Inicializa contador
    STORE CONTADOR

LOOP:
    LOAD TEMP
    SUB DOIS         ; temp = temp - 2
    JMPN VERIFICA    ; Se temp < 0, verifica
    
    STORE TEMP       ; Atualiza temp
    
    LOAD CONTADOR    ; contador++
    ADD UM
    STORE CONTADOR
    
    JMP LOOP

VERIFICA:
    LOAD TEMP
    JMPZ PAR         ; Se temp = 0, é par
    
IMPAR:
    OUTPUT UM        ; Imprime 1 (ímpar)
    JMP FIM

PAR:
    OUTPUT ZERO      ; Imprime 0 (par)

FIM:
    STOP

; Variáveis
NUMERO: SPACE
TEMP: SPACE
CONTADOR: SPACE
ZERO: CONST 0
UM: CONST 1
DOIS: CONST 2
