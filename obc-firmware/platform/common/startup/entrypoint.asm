;===============================================================================
; Entrypoint
;===============================================================================

    .sect   ".entrypoint"
    .retain ".entrypoint"
    .arm

;-------------------------------------------------------------------------------
; Imported References
;-------------------------------------------------------------------------------

; sys_startup.c
    .ref _c_int00

; startup_crt.c
    .ref _c_int01

; linker.cmd
    .ref FW_ENTRYPOINT_MAGIC_VALUE
    .ref FW_HEADER

;-------------------------------------------------------------------------------
; Jump Instruction
;-------------------------------------------------------------------------------

entrypoint
        .word FW_ENTRYPOINT_MAGIC_VALUE
        .word FW_HEADER
        b   _c_int00 ; Default startup routine supplied by HALCoGen
