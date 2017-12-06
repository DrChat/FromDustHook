#include <stdio.h>
#include <Windows.h>
#include <assert.h>
#include <vector>
#include <cstdarg>

// Herp, avoid some silly compiler warnings
#define stricmp _stricmp

// Some stuff about .lin.bf that I know so far:
// - Starts with linheader, bunch of dummy data
// - 200 byte file records begin, header has the file count
// - File data, data usually begins with a header
// -- File data is not compressed

// 5184 bytes
typedef struct linheader {
	char	magic[4];	// 0
	char	unk1[8];	// 4
	int		unk2;	// 12
	int		unk3;	// 16
	int		unk4;	// 20
	int		headerSize;	// 24 appears to be constant; size starting from the magic number. Could call this the first file offset too
	int		unk6;	// 28
	int		unk7;	// 32
	int		unk8;	// 36
	int		unk9;	// 40 always 1?
	int		fileCount;	// 44
	int		unk11;	// 48 always 1?
	char	unk12[12];	// 52 always 0xFF?
	int		unk13;	// 64 always 0x7F?
	char	unk14[4];	// 68 always 0x91022016?
	char	dummyfill[512];	// 73 filled with 0x7F
	char	dummyfill2[4600];	// 584 filled with 0x00
} linheader_t;

// 200 bytes
typedef struct linfilerecord {
	char	name[64]; // unused data filled with 0x7F, null-terminated string. name[63] is also null-terminated
	int		unk2; // 64
	int		unk3; // 68
	int		unk4; // 72
	int		unk5; // 76
	int		unk6; // 80
	int		id; // 84 file id maybe? starting from -1
	int		fileSize; // 88 size of entire file (inc. header)
	int		unk9; // 92
	int		unk10; // 96 same every rec?
	int		unk11; // 100
	int		unk12; // 104 always 0?
	int		fileOffset; // 108 offset to data from begin of file
	int		unk13; // 112 always 0?
	int		unk14; // 116 same as fileSize (always?)
	int		unk15; // 120 always 0x00007F7F?
	char	unk16[64]; // 124 0-filled data
	int 	unk17; // 188 always 0?
	int		unk18; // 192 always 128?
	int		unk19; // 196 always 0x7F?
} linfilerecord_t;

// 32 bytes
// present on most files before the file data
typedef struct linfile {
	int		size; // 0 size after this header
	int		unk2; // 4 size again?
	int		unk3; // 8
	int		unk4; // 12
	int		unk5; // 16
	char	ext[4]; // 20
	int		unk6; // 24
	int		unk7; // 28
} linfile_t;

// Global bools
bool	g_bStripFileHeader = false;

void printhelp() {
	printf("Arguments: \n");
	printf("\t-h: Prints this help text\n");
	printf("\t-u <file>: Extracts a file to a subdirectory of the same name\n");
	printf("\t-sh: Strips the file header from any files that have it (unpacking only)\n");
	printf("\t-p <folder>: Packs files in a folder into an archive of the same name (EXPERIMENTAL)\n");
	printf("\t-a <file>: Analyzes a file\n");
}

// Purpose: Make pretty red text
void Warning(const char *fmt, ...) {
	char buff[1024];

	va_list va;
	va_start(va, fmt);

	vsprintf_s(buff, fmt, va);
	buff[1023] = 0;

	va_end(va);

	// Write to console...

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	// Store old text attributes
	CONSOLE_SCREEN_BUFFER_INFO info;
	ZeroMemory(&info, sizeof(info));

	GetConsoleScreenBufferInfo(hStdOut, &info);
	SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_INTENSITY);

	DWORD dwNumBytesWritten;
	WriteFile(hStdOut, buff, strlen(buff) + 1, &dwNumBytesWritten, NULL);
	
	// Restore old text attributes
	SetConsoleTextAttribute(hStdOut, info.wAttributes);
}

// NOT THREAD SAFE!
// A terribly written function to get the base name of a file
// Don't keep a reference to the return value!
const char *basename(const char *rawName) {
	static char baseName[_MAX_FNAME];

	// Not using _s version because I have to make arrays for all of the unused parameters
	_splitpath(rawName, NULL, NULL, baseName, NULL);
	baseName[_MAX_FNAME - 1] = 0;

	return baseName;
}

int unpackfile(const char *fileName) {
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		Warning("File %s could not be opened for reading!\n", fileName);
		return 1;
	}

	DWORD dwNumBytesRead;
	DWORD dwNumBytesWritten;

	linheader_t header;
	ReadFile(hFile, &header, sizeof(linheader_t), &dwNumBytesRead, NULL);
	if (*(int *)header.magic != 4538945) { // != "ABE\0"
		Warning("Magic number is invalid!\n");
		return 1;
	}

	// Mangle ourselves together a directory
	CreateDirectory(basename(fileName), NULL);

	printf("File count: %d\n", header.fileCount);
	SetFilePointer(hFile, header.headerSize, NULL, FILE_BEGIN);

	int unk;
	ReadFile(hFile, &unk, sizeof(int), &dwNumBytesRead, NULL);

	// Game does this, if header.fileCount is less than unk, header.fileCount is used.
	// unk is identical to header.fileCount in almost all cases.
	assert(header.fileCount >= unk);
	
	// Right after header
	// char dummy[8];
	// ReadFile(hFile, &dummy, 8, &dwNumBytesRead, NULL);
	SetFilePointer(hFile, header.headerSize + 12, NULL, FILE_BEGIN);

	// Read all file records at once
	linfilerecord_t *records = new linfilerecord_t[header.fileCount];
	ReadFile(hFile, records, header.fileCount * sizeof(linfilerecord_t), &dwNumBytesRead, NULL);

	// 1MB buffer
	char *contentsBuff = new char[1048576];

	for (int i = 0; i < header.fileCount; i++) {
		printf("%s contents @ 0x%p, size %d\n", records[i].name, records[i].fileOffset, records[i].fileSize);

		// Some files have no special header!
		bool bHasHeader = true;
		char *ext = strrchr(records[i].name, '.');
		if (ext && !stricmp(ext, ".bik") || !stricmp(ext, ".tga")) {
			bHasHeader = false;
		}

		if (bHasHeader && g_bStripFileHeader)
			SetFilePointer(hFile, records[i].fileOffset + sizeof(linfile_t), NULL, FILE_BEGIN);
		else
			SetFilePointer(hFile, records[i].fileOffset, NULL, FILE_BEGIN);

		char newFileName[MAX_PATH];
		sprintf_s(newFileName, "%s\\%s", basename(fileName), records[i].name);
		HANDLE hOutFile = CreateFile(newFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hOutFile == INVALID_HANDLE_VALUE) {
			Warning("File %s could not be created!\n", newFileName);
			continue;
		}

		int remainingSize = records[i].fileSize;

		if (bHasHeader && g_bStripFileHeader)
			remainingSize -= sizeof(linfile_t);

		// TODO: Progress bar on extraction (cur file and total progress)!

		while (remainingSize > 0) {
			// Don't read more than the buffer size
			DWORD readBytes = remainingSize < 1048576 ? remainingSize : 1048576;
			ReadFile(hFile, contentsBuff, readBytes, &dwNumBytesRead, NULL);
			if (dwNumBytesRead != readBytes) {
				Warning("Read error - closing file\n");
				break;
			}

			WriteFile(hOutFile, contentsBuff, readBytes, &dwNumBytesWritten, NULL);
			if (dwNumBytesWritten != readBytes) {
				Warning("Write error - closing file\n");
				break;
			}

			remainingSize -= readBytes;
		}

		CloseHandle(hOutFile);
	}

	delete records;
	delete contentsBuff;

	CloseHandle(hFile);

	return 0;
}

int packfile(const char *folder) {
	char packedFileName[MAX_PATH];
	sprintf_s(packedFileName, "%s.lin.bf", basename(folder));

	HANDLE hPackedFile = CreateFile(packedFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hPackedFile != INVALID_HANDLE_VALUE) {
		DWORD dwNumBytesRead;
		DWORD dwNumBytesWritten;

		linheader_t header;
		ZeroMemory(&header, sizeof(linheader_t));

		*(int *)header.magic = 4538945; // = "ABE\0";
		header.headerSize = sizeof(linheader_t);

		header.dummyfill[0] = 0;
		for (int i = 1; i < 512; i++) {
			header.dummyfill[i] = 0x7F;
		}

		for (int i = 0; i < 4600; i++) {
			header.dummyfill2[i] = 0;
		}

		// Gather up all the files to be added
		// (and don't add too many unnecessary variables)
		std::vector<std::string> fileNames;
		{
			char findName[MAX_PATH];
			sprintf_s(findName, "%s\\*", folder);

			WIN32_FIND_DATA fd;
			HANDLE hFind = FindFirstFile(findName, &fd);

			while (FindNextFile(hFind, &fd)) {
				if (fd.cFileName[0] == '.')
					continue;

				header.fileCount++;
				fileNames.push_back(fd.cFileName);
			}
			
			FindClose(hFind);
		}

		// Write header
		WriteFile(hPackedFile, &header, sizeof(linheader_t), &dwNumBytesWritten, NULL);
		int fill[3];
		fill[0] = 0;
		fill[1] = -1;
		fill[2] = 0x7F7F7F7F;
		WriteFile(hPackedFile, &fill, 12, &dwNumBytesWritten, NULL);

		DWORD curRecPos = sizeof(linheader_t) + 12;
		DWORD curDataPos = header.fileCount * sizeof(linfilerecord_t) + sizeof(linheader_t) + 12;

		// Write file records & data
		for (int i = 0; i < header.fileCount; i++) {
			linfilerecord_t rec;
			ZeroMemory(&rec, sizeof(linfilerecord_t));

			curRecPos += sizeof(linfilerecord_t);

			strcpy_s(rec.name, fileNames[i].c_str());
			rec.id = i - 1;
			rec.fileOffset = curDataPos;

			char fileName[MAX_PATH];
			sprintf_s(fileName, "%s\\%s", folder, rec.name);

			HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) {
				Warning("Failed to open file %s for reading!\n", fileName);
				continue;
			}

			LARGE_INTEGER fileSize;
			GetFileSizeEx(hFile, &fileSize);

			rec.fileSize = fileSize.LowPart;
			printf("Packing file %s (size %d, id %d, offset 0x%p, data 0x%p)\n", rec.name, rec.fileSize, rec.id, rec.fileOffset, curDataPos);

			WriteFile(hPackedFile, &rec, sizeof(linfilerecord_t), &dwNumBytesWritten, NULL);

			// Write the file data
			SetFilePointer(hPackedFile, curDataPos, NULL, FILE_BEGIN);
			curDataPos += rec.fileSize;

			// 1 megabyte buffer
			char *buff = new char[1048576];

			while (ReadFile(hFile, buff, 1048576, &dwNumBytesRead, NULL)) {
				// EOF
				if (dwNumBytesRead == 0)
					break;

				WriteFile(hPackedFile, buff, dwNumBytesRead, &dwNumBytesWritten, NULL);
			}

			delete buff;

			SetFilePointer(hPackedFile, curRecPos, NULL, FILE_BEGIN);

			CloseHandle(hFile);
		}

		CloseHandle(hPackedFile);
	} else {
		Warning("Failed to create file %s for writing\n", packedFileName);
		return 1;
	}

	return 0;
}

int analyzefile(const char *fileName) {
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		Warning("Failed to open file %s for reading\n", fileName);
		return 1;
	}

	DWORD dwNumBytesRead;

	linheader_t header;
	ReadFile(hFile, &header, sizeof(linheader_t), &dwNumBytesRead, NULL);
	if (*(int *)header.magic != 4538945) { // != "ABE\0"
		Warning("Magic number is invalid!\n");
		return 1;
	}

	printf("%d files, %d header size\n", header.fileCount, header.headerSize);

	printf("--FILE LISTING--\n");

	SetFilePointer(hFile, header.headerSize, NULL, FILE_BEGIN);

	// Read some unknowns
	int unk;
	ReadFile(hFile, &unk, sizeof(int), &dwNumBytesRead, NULL);

	// Game does this, if header.fileCount is less than unk, header.fileCount is used.
	// unk is identical to header.fileCount in almost all cases.
	assert(header.fileCount >= unk);

	// Right after header
	// char dummy[8];
	// ReadFile(hFile, &dummy, 8, &dwNumBytesRead, NULL);
	SetFilePointer(hFile, header.headerSize + 12, NULL, FILE_BEGIN);

	// Read all file records at once
	linfilerecord_t *records = new linfilerecord_t[header.fileCount];
	ReadFile(hFile, records, header.fileCount * sizeof(linfilerecord_t), &dwNumBytesRead, NULL);

	for (int i = 0; i < header.fileCount; i++) {
		printf("%s\n", records[i].name);
	}
}

int main(int argc, const char *argv[]) {
	if (argc <= 1 || !stricmp(argv[1], "-h")) {
		printhelp();
		return 0;
	}

	for (int i = 1; i < argc; i++) {
		const char *arg = argv[i];
		if (!stricmp(arg, "-u")) {
			i++;
			// No buffer overflows please
			if (i >= argc) {
				Warning("Invalid argument\n");
				printhelp();
				break;
			}

			const char *file = argv[i];
			unpackfile(file);
		} else if (!stricmp(arg, "-p")) {
			i++;
			if (i >= argc) {
				Warning("Invalid argument\n");
				printhelp();
				break;
			}

			const char *folder = argv[i];
			packfile(folder);
		} else if (!stricmp(arg, "-a")) {
			i++;
			if (i >= argc) {
				Warning("Invalid argument\n");
				printhelp();
				break;
			}

			const char *file = argv[i];
			analyzefile(file);
		} else if (!stricmp(arg, "-sh")) {
			g_bStripFileHeader = true;
			continue;
		}
	}

	return 0;
}