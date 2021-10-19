#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"

#include "libraries/picosystem.hpp"

#define MP_OBJ_TO_PTR2(o, t) ((t *)(uintptr_t)(o))

using namespace picosystem;

extern "C" {
#include "picosystem.h"
#include "math.h"
#include "cstring"

typedef struct _PicosystemBuffer_obj_t {
    mp_obj_base_t base;
    buffer_t *buffer;
} _PicosystemBuffer_obj_t;

void PicosystemBuffer_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind; // Unused input parameter
    _PicosystemBuffer_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PicosystemBuffer_obj_t);
    mp_print_str(print, "Buffer(");

    mp_print_str(print, "w = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->buffer->w), PRINT_REPR);

    mp_print_str(print, ", h = ");
    mp_obj_print_helper(print, mp_obj_new_int(self->buffer->h), PRINT_REPR);

    mp_print_str(print, ")");
}

mp_obj_t PicosystemBuffer___del__(mp_obj_t self_in) {
    _PicosystemBuffer_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PicosystemBuffer_obj_t);
    uint32_t size = self->buffer->w * self->buffer->h;
    m_del(color_t, self->buffer->data, size);
    m_del(buffer_t, self->buffer, 1);
    return mp_const_none;
}

mp_obj_t PicosystemBuffer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    _PicosystemBuffer_obj_t *self = nullptr;

    enum { ARG_w, ARG_h };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_w, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
        { MP_QSTR_h, MP_ARG_INT | MP_ARG_REQUIRED, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int w = args[ARG_w].u_int;
    int h = args[ARG_h].u_int;

    if(w <= 0 || h <= 0) {
        mp_raise_ValueError("Buffer: both w and h must be greater than zero");
    }

    self = m_new_obj_with_finaliser(_PicosystemBuffer_obj_t);
    self->base.type = &PicosystemBuffer_type;

    self->buffer = m_new(buffer_t, 1);
    self->buffer->w = w;
    self->buffer->h = h;
    uint32_t size = w * h;
    self->buffer->data = m_new(color_t, size);

    return MP_OBJ_FROM_PTR(self);
}

mp_int_t PicosystemBuffer_get_buffer(mp_obj_t self_in, mp_buffer_info_t *bufinfo, mp_uint_t flags) {
    (void)flags;
    _PicosystemBuffer_obj_t *self = MP_OBJ_TO_PTR2(self_in, _PicosystemBuffer_obj_t);;
    bufinfo->buf = (void *)(self->buffer->data);
    bufinfo->len = self->buffer->w * self->buffer->h * sizeof(color_t);
    bufinfo->typecode = 'B'; // view PicosystemBuffer as bytes
    return 0;
}

}