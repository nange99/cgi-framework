#include <stdlib.h>
#include "data.h"

void destroy_data (data *d) {
	switch (d->type) {
		case STRING:
			free (d->value.u_str);
	}
	free (d);
}
