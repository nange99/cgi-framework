#include <stdlib.h>
#include "data.h"
#include "list.h"

void destroy_data (data *d) {
	switch (d->type) {
	case STRING:
		free (d->value.u_str);
		break;
	case LIST:
		cgi_destroy_list (d->value.u_hash);
		break;
	}
	free (d);
}
