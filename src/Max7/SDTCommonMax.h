#include "ext.h"
#include "ext_obex_util.h"

#ifndef SDT_COMMONMAX_H
#define SDT_COMMONMAX_H

#define MAXSDTMAXSTRING 512

/** @brief Logger function for SDT in Max
@param[in] fmt C string that contains a format string that follows the same
specifications as format in printf */
int SDT_maxPost(const char *fmt, ...);

/** @brief Set up logger functions for SDT in Max */
void SDT_setupMaxLoggers();

/** @brief Unregister the SDT object in the hashmap and free the SDT object
memory This macro works for destroyer functions whose signature is
"classname_free(t_classname *x)".
@param[in] T The SDT type (without the leading "SDT")
@param[in] F The name of the Max object field where the SDT object is stored */
#define SDT_MAX_FREE(T, F)                       \
  if (x->key) SDT_unregister##T(x->key->s_name); \
  SDT##T##_free(x->F);

/** @brief Define the setter function for the attribute "key".
It will register the SDT object in the hashmap if a key is specified
This macro works if the Max object has a field "x->key" of type "t_symbol *".
If the registration is unsuccessful, the key is not set
@param[in] M The Max type (without the leading "t_")
@param[in] T The SDT type (without the leading "SDT")
@param[in] F The name of the Max object field where the SDT object is stored
@param[in] O The name of the Max object (string)
@param[in] N The explicative name of the Max object (string)  */
#define SDT_MAX_KEY(M, T, F, O, N)                                           \
  void M##_key(t_##M *x, void *attr, long ac, t_atom *av) {                  \
    if (x->key) SDT_unregister##T(x->key->s_name);                           \
    t_symbol *key = atom_getsym(av);                                         \
    if (SDT_register##T(x->F, key->s_name))                                  \
      error("sdt.%s: Error registering the %s. Probably a duplicate id?", O, \
            N);                                                              \
    else                                                                     \
      x->key = key;                                                          \
  }

/** @brief Register the attribute "key" in the Max class
The t_class pointer should be a variable "c"
@param[in] M The Max type (without the leading "t_")
@param[in] I The attribute order index for "key" (string) */
#define SDT_CLASS_KEY(M, I)                              \
  CLASS_ATTR_SYM(c, "key", 0, t_##M, key);               \
  CLASS_ATTR_ACCESSORS(c, "key", NULL, (method)M##_key); \
  CLASS_ATTR_ORDER(c, "key", 0, I);

/** @brief Define the getter function for an attribute
@param[in] M The Max type (without the leading "t_")
@param[in] T The SDT type (without the leading "SDT")
@param[in] F The name of the Max object field where the SDT object is stored
@param[in] A The name of the attribute */
#define SDT_MAX_GETTER(M, T, F, A)                                    \
  t_max_err M##_get##A(t_##M *x, void *attr, long *ac, t_atom **av) { \
    if (!(*ac && *av)) {                                              \
      *ac = 1;                                                        \
      *av = (t_atom *)getbytes(sizeof(t_atom) * (*ac));               \
      if (!*av) {                                                     \
        *ac = 0;                                                      \
        return MAX_ERR_OUT_OF_MEM;                                    \
      }                                                               \
    }                                                                 \
    atom_setfloat(*av, SDT##T##_get##A(x->F));                        \
    return MAX_ERR_NONE;                                              \
  }

#define _SDT_TYPE_UPDATE_(T, O)
#define _SDT_TYPE_UPDATE_update(T, O) SDT##T##_update(O)

/** @brief Define the setter function for an attribute
@param[in] M The Max type (without the leading "t_")
@param[in] T The SDT type (without the leading "SDT")
@param[in] F The name of the Max object field where the SDT object is stored
@param[in] A The name of the attribute
@param[in] U Type `update` to trigger the structure update function */
#define SDT_MAX_SETTER(M, T, F, A, U)                               \
  t_max_err M##_set##A(t_##M *x, void *attr, long ac, t_atom *av) { \
    if (ac && av) {                                                 \
      SDT##T##_set##A(x->F, atom_getfloat(av));                     \
    }                                                               \
    _SDT_TYPE_UPDATE_##U(T, x->F);                                  \
    return MAX_ERR_NONE;                                            \
  }

#endif
