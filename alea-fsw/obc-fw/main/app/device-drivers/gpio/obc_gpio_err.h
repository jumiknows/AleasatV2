/**
 * @file obc_gpio_err.h
 * @brief Error codes for GPIO operations
 */

#ifndef OBC_GPIO_ERR_H_
#define OBC_GPIO_ERR_H_

/**
 * @brief Return value for GPIO functions, indicating if any errors occurred.
 */
typedef enum {
    /**
     * @brief Indicates that the function performed correctly without errors.
     */
    GPIO_SUCCESS = 0,

    /**
     * @brief Generic failure case returned from GPIO API.
     */
    GPIO_FAILURE = -1,

    /**
     * @brief Indicates an error occurred accessing the GPIO expander over I2C
     */
    GPIO_I2C_ERR = -2
} gpio_err_t;

#endif // OBC_GPIO_ERR_H_
