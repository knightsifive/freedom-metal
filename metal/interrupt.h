/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef METAL__INTERRUPT_H
#define METAL__INTERRUPT_H

/*! @file interrupt.h
 *  @brief API for registering and manipulating interrupts
 */

#include <stddef.h>

/*!
 * @brief Possible interrupt controllers
 */
typedef enum metal_interrupt_controller_ {
    METAL_CPU_CONTROLLER = 0,
    METAL_CLINT_CONTROLLER = 1,
    METAL_CLIC_CONTROLLER = 2,
    METAL_PLIC_CONTROLLER = 3
} metal_intr_cntrl_type;

/*!
 * @brief Possible mode of interrupts to operate
 */
typedef enum metal_vector_mode_ {
    METAL_DIRECT_MODE = 0,
    METAL_VECTOR_MODE = 1,
    METAL_SELECTIVE_VECTOR_MODE = 2,
    METAL_HARDWARE_VECTOR_MODE = 3
} metal_vector_mode;

/*!
 * @brief Function signature for interrupt callback handlers
 */
typedef void (*metal_interrupt_handler_t) (int, void *);

struct metal_interrupt;

struct metal_interrupt_vtable {
    void (*interrupt_init)(struct metal_interrupt *controller);
    int (*interrupt_register)(struct metal_interrupt *controller, int id,
			      metal_interrupt_handler_t isr, void *priv_data);
    int (*interrupt_enable)(struct metal_interrupt *controller, int id);
    int (*interrupt_disable)(struct metal_interrupt *controller, int id);
    int (*interrupt_vector_enable)(struct metal_interrupt *controller,
                                   int id, metal_vector_mode mode);
    int (*interrupt_vector_disable)(struct metal_interrupt *controller, int id);
    unsigned int (*interrupt_get_threshold)(struct metal_interrupt *controller);
    int (*interrupt_set_threshold)(struct metal_interrupt *controller, unsigned int threshold);
    unsigned int (*interrupt_get_priority)(struct metal_interrupt *controller, int id);
    int (*interrupt_set_priority)(struct metal_interrupt *controller, int id, unsigned int priority);
    int (*command_request)(struct metal_interrupt *controller, int cmd, void *data);
    int (*mtimecmp_set)(struct metal_interrupt *controller, int hartid, unsigned long long time);
};

/*!
 * @brief A handle for an interrupt
 */
struct metal_interrupt {
    const struct metal_interrupt_vtable *vtable;
};

/*!
 * @brief Initialize a given interrupt controller
 *
 * Initialize a given interrupt controller. This function must be called
 * before any interrupts are registered or enabled with the handler. It
 * is invalid to initialize an interrupt controller more than once.
 *
 * @param controller The handle for the interrupt controller
 */
__inline__ void metal_interrupt_init(struct metal_interrupt *controller)
{
    controller->vtable->interrupt_init(controller);
}

/*!
 * @brief Get the handle for an given interrupt controller type
 * @param cntrl The type ofinterrupt controller
 * @param id The instance of the interrupt controller
 * @return A handle to the interrupt controller (CLINT, CLIC, PLIC), or
 * NULL if none is found for the requested label
 */
struct metal_interrupt* metal_interrupt_get_controller(metal_intr_cntrl_type cntrl,
                                                       int id);

/*!
 * @brief Register an interrupt handler
 * @param controller The handle for the interrupt controller
 * @param id The interrupt ID to register
 * @param handler The interrupt handler callback
 * @param priv_data Private data for the interrupt handler
 * @return 0 upon success
 */
__inline__ int metal_interrupt_register_handler(struct metal_interrupt *controller,
                                          int id,
                                          metal_interrupt_handler_t handler,
                                          void *priv_data)
{
    return controller->vtable->interrupt_register(controller, id, handler, priv_data);
}

/*!
 * @brief Enable an interrupt
 * @param controller The handle for the interrupt controller
 * @param id The interrupt ID to enable
 * @return 0 upon success
 */
__inline__ int metal_interrupt_enable(struct metal_interrupt *controller, int id)
{
    return controller->vtable->interrupt_enable(controller, id);
}

/*!
 * @brief Disable an interrupt
 * @param controller The handle for the interrupt controller
 * @param id The interrupt ID to disable
 * @return 0 upon success
 */
__inline__ int metal_interrupt_disable(struct metal_interrupt *controller, int id)
{
    return controller->vtable->interrupt_disable(controller, id);
}

/*!
 * @brief Enable an interrupt vector
 * @param controller The handle for the interrupt controller
 * @param id The interrupt ID to enable
 * @param mode The interrupt mode type to enable
 * @return 0 upon success
 */
__inline__ int metal_interrupt_vector_enable(struct metal_interrupt *controller,
                                       int id, metal_vector_mode mode)
{
    return controller->vtable->interrupt_vector_enable(controller, id, mode);
}

/*!
 * @brief Set interrupt threshold level
 * @param controller The handle for the interrupt controller
 * @param threshold The interrupt threshold level
 * @return 0 upon success
 */
inline int metal_interrupt_set_threshold(struct metal_interrupt *controller, unsigned int level)
{
    return controller->vtable->interrupt_set_threshold(controller, level);
}

/*!
 * @brief Get an interrupt threshold level
 * @param controller The handle for the interrupt controller
 * @return The interrupt threshold level
 */
inline unsigned int metal_interrupt_get_threshold(struct metal_interrupt *controller)
{
  return controller->vtable->interrupt_get_threshold(controller);
}

/*!
 * @brief Set an interrupt priority level
 * @param controller The handle for the interrupt controller
 * @param id The interrupt ID to enable
 * @param priority The interrupt priority level
 * @return 0 upon success
 */
inline int metal_interrupt_set_priority(struct metal_interrupt *controller,
					int id, unsigned int priority)
{
    return controller->vtable->interrupt_set_priority(controller, id, priority);
}

/*!
 * @brief Get an interrupt priority level
 * @param controller The handle for the interrupt controller
 * @param id The interrupt ID to enable
 * @return The interrupt priority level
 */
inline unsigned int metal_interrupt_get_priority(struct metal_interrupt *controller, int id)
{
  return controller->vtable->interrupt_get_priority(controller, id);
}

/*!
 * @brief Disable an interrupt vector
 * @param controller The handle for the interrupt controller
 * @param id The interrupt ID to disable
 * @return 0 upon success
 */
__inline__ int metal_interrupt_vector_disable(struct metal_interrupt *controller, int id)
{
    return controller->vtable->interrupt_vector_disable(controller, id);
}

/* Utilities function to controll, manages devices via a given interrupt controller */
__inline__ int _metal_interrupt_command_request(struct metal_interrupt *controller,
					 int cmd, void *data)
{
    return controller->vtable->command_request(controller, cmd, data);
}

#endif
