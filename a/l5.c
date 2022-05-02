#include "uerror.h"
#include "ustring.h"
#include "l5eval.h"

int main (int argc,char** args)
{
    l5eval* eval;
    char* path = NULL;
    char* vm_asz_str;
    int vm_asz_int = 10*1024;
    char* vm_ssz_str;
    int vm_ssz_int = 100*1024;
    char* vm_rsz_str;
    int vm_rsz_int = 1024;
    char* conf = NULL;
    char* script = NULL;
    int i;

    for (i = 0; i < argc; i++) {
        if (!ustrcmp(args[i], "--self-path")) {
            path = args[++i];
        }
        else if (!ustrcmp(args[i], "--conf")) {
            conf = args[++i];
        }
	else if (!ustrcmp(args[i], "--vm-asz")) {
            vm_asz_str = args[++i];
	    if (ucharp_isint(vm_asz_str)) {
	      vm_asz_int = atoi(vm_asz_str);
	    } else {
	      uabort("--vm-asz not a integer");
	    }
        }
	else if (!ustrcmp(args[i], "--vm-ssz")) {
            vm_ssz_str = args[++i];
	    if (ucharp_isint(vm_ssz_str)) {
	      vm_ssz_int = atoi(vm_ssz_str);
	    } else {
	      uabort("--vm-ssz not a integer");
	    }
        }
	else if (!ustrcmp(args[i], "--vm-rsz")) {
            vm_rsz_str = args[++i];
	    if (ucharp_isint(vm_rsz_str)) {
	      vm_rsz_int = atoi(vm_rsz_str);
	    } else {
	      uabort("--vm-rsz not a integer");
	    }
        }
        else {
            script = args[++i];
        }
    }
    if (!path) {
      uabort("--self-path not file");
    }
    if (!script) {
      uabort("script file not define");
    }
    eval = l5startup(path,vm_asz_int,vm_ssz_int,vm_rsz_int);
    l5eval_conf_load(eval, conf);
    l5eval_src_load(eval, script);
    return 0;
}
