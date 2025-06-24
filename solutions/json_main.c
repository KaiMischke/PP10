#include <jansson.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	    json_t *root = json_pack("{s:i, s:s}", "id", 1, "name", "Alice");
	        if(!root) {
			        fprintf(stderr, "json_pack failed\n");
				        return 1;
					    }
		    char *dump = json_dumps(root, 0);
		        if(!dump) {
				        fprintf(stderr, "json_dumps failed\n");
					        json_decref(root);
						        return 1;
							    }
			    printf("%s\n", dump);
			        free(dump);
				    json_decref(root);
				        return 0;
}
