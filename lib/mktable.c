#include <stdio.h>
#include <string.h>

#include "poker_defs.h"
#include "mktable.h"


static const char *gTableName, *gFileName, *gTableType;
static int gTableSize;
static int gInUse = 0, gHeaderDone=0, gLineCount;
static FILE *gCFile, *gHFile;

void
MakeTable_begin(const char *tableName,
                const char *fileName, 
                const char *tableType,
                int tableSize) {
  char fnBuf[128];

  if (gInUse) 
    fprintf(stderr, "MakeTable_begin called before previous call to _end\n");
  gTableName = tableName;
  gFileName  = fileName;
  gTableType = tableType;
  gTableSize = tableSize;
  gInUse = 1;
  gLineCount = 0;
  gHeaderDone = 0;

  strcpy(fnBuf, fileName);
  strcat(fnBuf, ".c");
  gCFile = fopen(fnBuf, "w");
  if (!gCFile) 
    fprintf(stderr, "Could not open %s for writing\n", fnBuf);
  else {
    fprintf(gCFile, "#include \"poker_defs.h\"\n\n");
    fprintf(gCFile, "/* \n * Table %s\n */\n\n", gTableName);
  };
}


static void 
outputHeader(void) {
  if (gCFile) {
    fprintf(gCFile, "%s %s[%d] = { \n", gTableType, gTableName, gTableSize);
  };
  gHeaderDone = 1;
}

void
MakeTable_comment(const char *commentString) {
  if (!gInUse) 
    fprintf(stderr, "MakeTable_comment called before _begin\n");
  else if (gHeaderDone) 
    fprintf(stderr, "MakeTable_comment called after first table entry\n");
  else 
    if (gCFile) 
      fprintf(gCFile, "/*\n%s\n */\n\n", commentString);
}

void 
MakeTable_outputString(const char *string) {
  if (!gHeaderDone)
    outputHeader();
  ++gLineCount;
  if (gCFile)
    fprintf(gCFile, "     %s %s\n", string,
            (gLineCount == gTableSize ? "" : ","));
}

#ifdef HAVE_INT64
void 
MakeTable_outputUInt64(uint64 arg) {
  uint32 high, low;
  char buf[80];

  high = arg >> 32;
  low  = (uint32) arg;
  sprintf(buf, " { 0x%08x%08xLL } ", high, low);
  MakeTable_outputString(buf);
}
#endif

void 
MakeTable_outputUInt32(uint32 arg) {
  char buf[80];
  sprintf(buf, "0x%08x", arg);
  MakeTable_outputString(buf);
}

void 
MakeTable_outputUInt16(uint16 arg) {
  char buf[80];
  sprintf(buf, "0x%04x", arg);
  MakeTable_outputString(buf);
}

void 
MakeTable_outputUInt8(uint8 arg) {
  char buf[80];
  sprintf(buf, "0x%02x", arg);
  MakeTable_outputString(buf);
}

void 
MakeTable_end(void) {
  if (gCFile) 
    fprintf(gCFile, "};\n");
  fclose(gCFile);

  gInUse = 0;
}
    
