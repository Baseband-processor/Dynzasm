#include "disas.h"

int main()
{
	struct disassembler *ds = ds_init(MIPS_ARCH, MODE_64B);

	//unsigned char bytes[] = "\x55\x89\xe5\x83\xec\x10\xe8\x44\x00\x00\x00\x05\xee\x1a\x00\x00\xc7\x45\xf8\x00\x00\x00\x00\xc7\x45\xfc\x00\x00\x00\x00\xeb\x22\x8b\x45\xfc\x2b\x45\xf8\x8b\x4d\xf8\x8b\x55\xfc\x01\xca\x50\xff\x75\xfc\x52\xe8\xae\xff\xff\xff\x83\xc4\x0c\x01\x45\xf8\x83\x45\xfc\x01\x83\x7d\xfc\x09\x7e\xd8\xb8\x00\x00\x00\x00\xc9\xc3";
	//unsigned char bytes[] = "\x55\x48\x89\xe5\x48\x83\xec\x10\xc7\x45\xf8\x00\x00\x00\x00\xc7\x45\xfc\x00\x00\x00\x00\xeb\x23\x8b\x45\xfc\x2b\x45\xf8\x89\xc2\x8b\x4d\xf8\x8b\x45\xfc\x01\xc1\x8b\x45\xfc\x89\xc6\x89\xcf\xe8\xb2\xff\xff\xff\x01\x45\xf8\x83\x45\xfc\x01\x83\x7d\xfc\x09\x7e\xd7\xb8\x00\x00\x00\x00\x5d\xc3";
	//unsigned char bytes[] = "\xf8\xff\xbd\x27\x04\x00\xbe\xaf\x25\xf0\xa0\x03\x08\x00\xc4\xaf\x08\x00\xc3\x8f\x08\x00\xc2\x8f\x00\x00\x00\x00\x18\x00\x62\x00\x12\x10\x00\x00\x25\xe8\xc0\x03\x04\x00\xbe\x8f\x08\x00\xbd\x27\x08\x00\xe0\x03\x00\x00\x00\x00\x00\x00\x00\x00";
	//unsigned char bytes[] = "\x00\x08\x02\x46";
	unsigned char bytes[] = "\x42\x31\x27\x46";

	ds_decode(ds, bytes, sizeof(bytes)-1, 0x0);
	struct dis *dis = NULL;
	int biter = 0;
	DS_FOREACH(ds, dis) {
		printf("%#08lx:\t", dis->address);
		for (int i = 0; i < 4; i++) {
			printf("%02x ", bytes[biter++]);
		}
		printf("\t%s\t%s\n",dis->mnemonic, dis->op_squash);
	}

	ds_destroy(ds);
	return 0;
}
