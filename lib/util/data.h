#ifndef _DATA_H
#define _DATA_H

/**
 * @defgroup Data Generic data type
 *
 * @{
	 */

/**
 * Possible data types
 */
enum data_type { 
	STRING=1, /**> */
	INTEGER, /**> */
	FLOAT, /**> */
	LIST, /**> */
	HASHTABLE, /**> */
	BOGUS /**> */
};

/**
 * Data structure
 */
typedef struct _data data;

/**
 * Data structure definition
 */
struct _data {
	/**> type of store data */
	int type; 
	/**> union with the value */
	union {
		int u_int;
		double u_double;
		char *u_str;
		void *u_hash;
	} value; 
};

void destroy_data (data *d);

/** @} */

#endif
