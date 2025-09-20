# HALCoGen Project

## User Code Modifications

### `source/sys_startup.c`

- `USER CODE (1)`
    ```c
    #include "exc_ram.h"
    #include "cpy_tbl.h"
    ```
- `USER CODE (39)`
    ```c
        copy_in(&EXCRAM_COPY_TABLE);
    ```
