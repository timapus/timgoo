#include <stdlib.h>
#include <string.h>

/* file extension name */
int GetFileType(char* pname) {
	if(pname != NULL)
		strcpy(pname, "SAV|LVT|LV0|LV1|LV2"); // Emulator ROM extensions (use "EXT|EXT|EXT" for several file-type associations)
	return 0;
}

/* to get default path */
int GetDefaultPath(char* path) {
	if(path != NULL)
		strcpy(path, "A:\\GAME\\LVIV");
	return 0;
}

/* module description, optional */
int GetModuleName(char* name, int code_page) {
	if((name != NULL) && (code_page == 0)) // ansi
		strcpy(name, "lviv.sim");  //your emulator file name
	return 0;
}
