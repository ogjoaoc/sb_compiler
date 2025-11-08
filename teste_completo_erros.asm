; ===============================================
; TESTE COMPLETO DE DETECÇÃO DE ERROS
; ===============================================

; --- TESTES DE ERROS LÉXICOS ---

; Erro: Rótulo começa com número
2INVALID: SPACE 1

; Erro: Rótulo contém caractere inválido (@)
VAR@ERRO: CONST 5

; Erro: Símbolo com caractere especial
LOAD VAR#INVALIDO

; Erro: Operando começando com número (usado como símbolo)
ADD 3ROTULO


; --- TESTES DE ERROS SINTÁTICOS ---

; Erro: SPACE com argumento não numérico
LABEL1: SPACE TEXTO

; Erro: SPACE com múltiplos argumentos
LABEL2: SPACE 1 2 3

; Erro: CONST com argumento não numérico
LABEL3: CONST OUTRO_LABEL

; Erro: CONST sem argumento
LABEL4: CONST

; Erro: Número incorreto de operandos (ADD precisa de 1)
ADD X Y

; Erro: Número incorreto de operandos (COPY precisa de 2)
COPY A

; Erro: Offset não numérico (X + Y onde Y não é número)
LOAD X + Y

; Erro: Offset não numérico com subtração
STORE Z - OUTRO

; Erro: Operador sem operando seguinte
INPUT W +

; Erro: Instrução inexistente
INVALID_OP X


; --- TESTES DE ERROS SEMÂNTICOS ---

; Erro: SPACE com valor negativo
NEG: SPACE -5

; Erro: SPACE com valor zero
ZERO: SPACE 0

; Erro: Rótulo não declarado (forward reference sem declaração)
JMP INEXISTENTE

; Erro: Uso de símbolo não declarado
OUTPUT NAO_DECLARADO


; --- TESTES VÁLIDOS (para garantir que não quebra código correto) ---

; Válido: Declarações corretas
VETOR: SPACE 10
NUM_POS: CONST 42
NUM_NEG: CONST -15

; Válido: Instruções com offsets corretos
INPUT VETOR+2
LOAD VETOR
STORE VETOR+5
ADD VETOR+3
SUB VETOR-1
OUTPUT VETOR+9

; Válido: Instrução COPY com 2 operandos
COPY VETOR+1, VETOR+2

; Válido: Forward reference que será resolvida
JMP FIM

; Válido: Instruções simples
LOAD NUM_POS
ADD NUM_NEG

; Rótulo válido sendo declarado
FIM: STOP


; --- TESTES ADICIONAIS DE MÚLTIPLOS ERROS ---

; Erro: Rótulo duplicado (FIM já foi declarado acima)
FIM: SPACE 1

; Erro: Múltiplos problemas na mesma linha
; - Rótulo inválido (começa com número)
; - SPACE com argumento não numérico
1ERRO: SPACE ABC
