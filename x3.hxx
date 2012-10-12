#ifndef X3_HXX_
#define X3_HXX_

extern "C" {
#include "x3.h"
}

namespace x3 {

	class vm {
		x3_vm vm_;

	public:
		vm() : vm_(X3_VM) {}

		size_t symtable_size() {
			return x3_symtable_size(&vm_);
		}

		size_t symtable_load() {
			return x3_symtable_load(&vm_);
		}
	};

}

#endif
