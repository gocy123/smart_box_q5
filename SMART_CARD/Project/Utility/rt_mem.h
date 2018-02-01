

/**
 * @addtogroup Error
 */

/*@{*/

#include "rtdef.h"
/* RT-Thread error code definitions */
#define RT_EOK                          0               /**< There is no error */
#define RT_ERROR                        1               /**< A generic error happens */
#define RT_ETIMEOUT                     2               /**< Timed out */
#define RT_EFULL                        3               /**< The resource is full */
#define RT_EEMPTY                       4               /**< The resource is empty */
#define RT_ENOMEM                       5               /**< No memory */
#define RT_ENOSYS                       6               /**< No system */
#define RT_EBUSY                        7               /**< Busy */
#define RT_EIO                          8               /**< IO error */

/*@}*/
// <integer name="RT_NAME_MAX" description="Maximal size of kernel object name length" default="6" />
//#define RT_NAME_MAX	8
// <integer name="RT_ALIGN_SIZE" description="Alignment size for CPU architecture data access" default="4" />
#define RT_ALIGN_SIZE	4


/**
 * @ingroup BasicDef
 *
 * @def RT_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. RT_ALIGN(13, 4)
 * would return 16.
 */
#define RT_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def RT_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. RT_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define RT_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

extern void rt_system_heap_init(void *begin_addr, void *end_addr);
extern void *rt_malloc(rt_size_t size,char const*caller);
extern void *rt_realloc(void *rmem, rt_size_t newsize);
extern void *rt_calloc(rt_size_t count, rt_size_t size);
extern void rt_free(void *rmem,char const*caller);