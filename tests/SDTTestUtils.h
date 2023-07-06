#include <stdint.h>
#include "CuTest.h"
#include "SDT/SDTCommon.h"

/** @file SDTTestUtils.h
@defgroup test_utils SDTTestUtils.h: Utilities for tests
@{ */

#ifndef SDT_TESTUTILS_H
#define SDT_TESTUTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Opaque data structure for random number sequences. */
typedef struct SDTRandomSequence SDTRandomSequence;

/** @brief Object destructor.
@param[in] x Pointer to the instance to destroy */
extern void SDTRandomSequence_free(SDTRandomSequence *x);

/** @brief Check if random sequence is over.
@param[in] x Pointer to the SDTRandomSequence instance
@return If the sequence is done 1, otherwise 0 */
extern unsigned char SDTRandomSequence_done(SDTRandomSequence *x);

/** @brief Get sequence index.
@param[in] x Pointer to the SDTRandomSequence instance
@return The sequence index */
extern unsigned int SDTRandomSequence_index(SDTRandomSequence *x);

/** @brief Set seed for the first element.
@param[in] x Pointer to the SDTRandomSequence instance
@param[in] b Base seed */
extern void SDTRandomSequence_setBase(SDTRandomSequence *x, unsigned int b);

/** @brief Start random sequence of integers.
@param[in] x Pointer to the SDTRandomSequence instance
@return First random integer */
extern int SDTRandomSequence_startInt(SDTRandomSequence *x);

/** @brief Get next value in the random sequence of integers.
@param[in] x Pointer to the SDTRandomSequence instance
@return Next random integer */
extern int SDTRandomSequence_nextInt(SDTRandomSequence *x);

/** @brief Start random sequence of floats.
@param[in] x Pointer to the SDTRandomSequence instance
@return First random float */
extern float SDTRandomSequence_startFloat(SDTRandomSequence *x);

/** @brief Get next value in the random sequence of floats.
@param[in] x Pointer to the SDTRandomSequence instance
@return Next random float */
extern float SDTRandomSequence_nextFloat(SDTRandomSequence *x);

/** @brief Object constructor for integer RNG
@param[in] n Number of numbers in the sequence
@param[in] min Minimum random number admissible
@param[in] max Maximum random number admissible
@return Pointer to the new instance */
extern SDTRandomSequence *SDTRandomSequence_newInt(unsigned int n, int min,
                                                   int max);

/** @brief Object constructor for float RNG
@param[in] n Number of numbers in the sequence
@param[in] min Minimum random number admissible
@param[in] max Maximum random number admissible
@return Pointer to the new instance */
extern SDTRandomSequence *SDTRandomSequence_newFloat(unsigned int n, float min,
                                                     float max);

/** @brief Object constructor for float RNG with logarithmic scaling
@param[in] n Number of numbers in the sequence
@param[in] min Minimum random number admissible
@param[in] max Maximum random number admissible
@return Pointer to the new instance */
extern SDTRandomSequence *SDTRandomSequence_newLog(unsigned int n, float min,
                                                   float max);

/** @brief Object constructor for float RNG with exponential distribution
@param[in] n Number of numbers in the sequence
@param[in] min Offset: minimum random number admissible
@param[in] rate Lambda: rate (inverse scale)
@return Pointer to the new instance */
extern SDTRandomSequence *SDTRandomSequence_newExp(unsigned int n, float min,
                                                   float rate);

#define _FOR_RANDOM_ITER_PROTO(IT, VAR, T, SUFFIX)  \
  for (T VAR = SDTRandomSequence_start##SUFFIX(IT); \
       !SDTRandomSequence_done(IT); VAR = SDTRandomSequence_next##SUFFIX(IT))

/** @brief Iterate integer values
@param[in] IT SDTRandomSequence instance pointer
@param[in] VAR Loop variable name */
#define FOR_RANDOM_ITER_INT(IT, VAR) _FOR_RANDOM_ITER_PROTO(IT, VAR, int, Int)

/** @brief Iterate float values
@param[in] IT SDTRandomSequence instance pointer
@param[in] VAR Loop variable name */
#define FOR_RANDOM_ITER_FLOAT(IT, VAR) \
  _FOR_RANDOM_ITER_PROTO(IT, VAR, float, Float)

#define CuAssertPointerEquals(tc, ex, ac) \
  CuAssertIntEquals(tc, (uintptr_t)ex, (uintptr_t)ac)
#define CuAssertPointerEquals_Msg(tc, ms, ex, ac) \
  CuAssertIntEquals_Msg(tc, ms, (uintptr_t)ex, (uintptr_t)ac)

/* Test prototype
void TestSDT_functionName(CuTest* tc)
{
  SDT_TEST_BEGIN()
  ...
  SDT_TEST_END()
}
*/

#ifdef SDT_DEBUG
#define SDT_TEST_BEGIN() \
  _SDT_resetArena();     \
  SDT_LOG(INFO, "\n");   \
  {
#define SDT_TEST_END()                                                  \
  size_t __arena_size = _SDT_currentArena();                            \
  _SDT_arenaWarnNonEmpty();                                             \
  _SDT_resetArena();                                                    \
  CuAssertIntEquals_Msg(tc, "Check everything has been deallocated", 0, \
                        __arena_size);                                  \
  }
#else
#define SDT_TEST_BEGIN() \
  SDT_LOG(INFO, "\n");   \
  {
#define SDT_TEST_END() }
#endif

#define _TEST_SDT_HASHMAP(TYPENAME, ...)                         \
  SDT##TYPENAME *x0 = SDT##TYPENAME##_new(__VA_ARGS__);          \
  SDT##TYPENAME *x1 = SDT##TYPENAME##_new(__VA_ARGS__);          \
  const char *key0 = #TYPENAME "_0";                             \
  const char *key1 = #TYPENAME "_1";                             \
                                                                 \
  /* No registered */                                            \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME(key0));      \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME(key1));      \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME("missing")); \
                                                                 \
  /* Register 0 */                                               \
  CuAssertIntEquals(tc, 0, SDT_register##TYPENAME(x0, key0));    \
  CuAssertPointerEquals(tc, x0, SDT_get##TYPENAME(key0));        \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME(key1));      \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME("missing")); \
                                                                 \
  /* Re-register 0 (will print a warning) */                     \
  CuAssertIntEquals(tc, 1, SDT_register##TYPENAME(x0, key0));    \
                                                                 \
  /* Register 1 */                                               \
  CuAssertIntEquals(tc, 0, SDT_register##TYPENAME(x1, key1));    \
  CuAssertPointerEquals(tc, x0, SDT_get##TYPENAME(key0));        \
  CuAssertPointerEquals(tc, x1, SDT_get##TYPENAME(key1));        \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME("missing")); \
                                                                 \
  /* Unregister missing */                                       \
  CuAssertIntEquals(tc, 1, SDT_unregister##TYPENAME("missing")); \
                                                                 \
  /* Unregister 0 */                                             \
  CuAssertIntEquals(tc, 0, SDT_unregister##TYPENAME(key0));      \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME(key0));      \
  CuAssertPointerEquals(tc, x1, SDT_get##TYPENAME(key1));        \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME("missing")); \
                                                                 \
  /* Unregister 1 */                                             \
  CuAssertIntEquals(tc, 0, SDT_unregister##TYPENAME(key1));      \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME(key0));      \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME(key1));      \
  CuAssertPointerEquals(tc, NULL, SDT_get##TYPENAME("missing")); \
                                                                 \
  /* Unregister missing on empty hashmap */                      \
  CuAssertIntEquals(tc, 1, SDT_unregister##TYPENAME("missing")); \
                                                                 \
  SDT##TYPENAME##_free(x0);                                      \
  SDT##TYPENAME##_free(x1);

#ifdef __cplusplus
};
#endif

#endif

/** @} */
