; Programa para dividir dois números usando subtrações sucessivas
; Calcula DIVIDENDO / DIVISOR = quantas vezes podemos subtrair DIVISOR de DIVIDENDO

INICIO:
    INPUT DIVIDENDO  ; Lê o dividendo
    INPUT DIVISOR    ; Lê o divisor
    
    LOAD ZERO        ; Inicializa quociente = 0
    STORE QUOCIENTE
    
    LOAD DIVIDENDO   ; Inicializa resto = DIVIDENDO
    STORE RESTO

LOOP:
    LOAD RESTO
    SUB DIVISOR      ; resto = resto - DIVISOR
    JMPN FIM         ; Se resto < 0, termina
    
    STORE RESTO      ; Atualiza resto
    
    LOAD QUOCIENTE   ; quociente = quociente + 1
    ADD UM
    STORE QUOCIENTE
    
    JMP LOOP

FIM:
    OUTPUT QUOCIENTE ; Imprime quociente
    OUTPUT RESTO     ; Imprime resto
    STOP

; Variáveis
DIVIDENDO: SPACE
DIVISOR: SPACE
QUOCIENTE: SPACE
RESTO: SPACE
ZERO: CONST 0
UM: CONST 1
