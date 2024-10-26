;===============================================================================
; RAM Exception (Interrupt) Handlers
;
; Reference: https://www.ti.com/lit/an/spna236/spna236.pdf
;===============================================================================

    .sect   ".exc_ram"
    .retain ".exc_ram"
    .arm

;-------------------------------------------------------------------------------
; Imported References
;-------------------------------------------------------------------------------

; isr.asm
    .ref isr_undef
    .ref isr_swi
    .ref isr_pref
    .ref isr_data
    .ref isr_phantom

;-------------------------------------------------------------------------------
; Exported References
;-------------------------------------------------------------------------------

    .def exc_vec_ram_undef
    .def exc_vec_ram_swi
    .def exc_vec_ram_pref
    .def exc_vec_ram_data
    .def exc_vec_ram_phantom

;-------------------------------------------------------------------------------
; RAM Trampolines
;-------------------------------------------------------------------------------

; This is the RAM version of the exception vector table that will branch to the
; ISR address given in the RAM ISR table

exc_vec_ram_undef
        ldr pc, exc_tab_ram_undef

exc_vec_ram_swi
        ldr pc, exc_tab_ram_swi

exc_vec_ram_pref
        ldr pc, exc_tab_ram_pref

exc_vec_ram_data
        ldr pc, exc_tab_ram_data

exc_vec_ram_phantom
        ldr pc, exc_tab_ram_phantom

;-------------------------------------------------------------------------------
; RAM ISR Table
;-------------------------------------------------------------------------------

; This is a table of the actual ISR addresses

exc_tab_ram_undef:    .word isr_undef
exc_tab_ram_swi:      .word isr_swi
exc_tab_ram_pref:     .word isr_pref
exc_tab_ram_data:     .word isr_data
exc_tab_ram_phantom:  .word isr_phantom
