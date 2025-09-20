;===============================================================================
; Flash Exception Handlers
;
; Reference: https://www.ti.com/lit/an/spna236/spna236.pdf
;===============================================================================

    .sect   ".exc_flash"
    .retain ".exc_flash"
    .arm

;-------------------------------------------------------------------------------
; Imported References
;-------------------------------------------------------------------------------

; sys_startup.c
    .ref _c_int00

; exc_ram.asm
    .ref exc_vec_ram_undef
    .ref exc_vec_ram_swi
    .ref exc_vec_ram_pref
    .ref exc_vec_ram_data
    .ref exc_vec_ram_phantom

;-------------------------------------------------------------------------------
; Exported References
;-------------------------------------------------------------------------------

    .def exc_vec_reset

;-------------------------------------------------------------------------------
; Exception Vectors
;-------------------------------------------------------------------------------

; Each exception handler (except IRQ/FIQ) loads a value from the trampoline table
; to the program counter (to allow each firmware image to handle exceptions
; differently)

; NOTE: The pc always contains the address of the current instruction + 0x08
;       (due to pipelining)

; 0x00
exc_vec_reset
        b   _c_int00
; 0x04
exc_vec_undef
        ldr pc, exc_tab_undef
; 0x08
exc_vec_swi
        ldr pc, exc_tab_swi
; 0x0C
exc_vec_prefetch
        ldr pc, exc_tab_pref
; 0x10
exc_vec_data
        ldr pc, exc_tab_data
; 0x14
exc_vec_phantom
        ldr pc, exc_tab_phantom

; 0x18
exc_vec_irq
        ldr pc, [pc, #-0x1b0] ; pc = (0x18 + 0x08) - 0x1B0 = 0xFFFFFE70 (IRQVECREG)
; 0x1C
exc_vec_fiq
        ldr pc, [pc, #-0x1b0] ; pc = (0x18 + 0x08) - 0x1B0 = 0xFFFFFE74 (FIQVECREG)

;-------------------------------------------------------------------------------
; Exception Trampoline Table
;-------------------------------------------------------------------------------

; This table contains the addresses of the trampolines defined in RAM (so each
; firmware image can define their own trampolines).

; The trampolines must always be placed at the same RAM addresses since this
; table is stored in the startup image's flash and cannot change at runtime

; 0x20
exc_tab_undef:   .word exc_vec_ram_undef
exc_tab_swi:     .word exc_vec_ram_swi
exc_tab_pref:    .word exc_vec_ram_pref
exc_tab_data:    .word exc_vec_ram_data
exc_tab_phantom: .word exc_vec_ram_phantom

; TODO handle infinite exception loop in flash assembly routine (in case the RAM table is not initialized)
