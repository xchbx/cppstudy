
#include "InputFile.hh"
#include <string.h>

FILE* OpenInputFile(char const* fileName) {
  FILE* fid;

  // Check for a special case file name: "stdin"
  if (strcmp(fileName, "stdin") == 0) {
    fid = stdin;
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
    _setmode(_fileno(stdin), _O_BINARY); // convert to binary mode
#endif
  } else {
    fid = fopen(fileName, "rb");
    if (fid == NULL) {
        //fwrite(stderr, "unable to open file: %s\"",fileName, "\"");
      }
  }

  return fid;
}

void CloseInputFile(FILE* fid) {
  // Don't close 'stdin', in case we want to use it again later.
  if (fid != NULL && fid != stdin) fclose(fid);
}

uint64_t GetFileSize(char const* fileName, FILE* fid) {
  uint64_t fileSize = 0; // by default

  if (fid != stdin) {
#if !defined(_WIN32_WCE)
    if (fileName == NULL) {
#endif
      if (fid != NULL && SeekFile64(fid, 0, SEEK_END) >= 0) {
    fileSize = (uint64_t)TellFile64(fid);
    if (fileSize == (uint64_t)-1) fileSize = 0; // TellFile64() failed
	SeekFile64(fid, 0, SEEK_SET);
      }
#if !defined(_WIN32_WCE)
    } else {
      struct stat sb;
      if (stat(fileName, &sb) == 0) {
	fileSize = sb.st_size;
      }
    }
#endif
  }

  return fileSize;
}

int64_t SeekFile64(FILE *fid, int64_t offset, int whence) {
  if (fid == NULL) return -1;

  clearerr(fid);
  fflush(fid);
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
  return _lseeki64(_fileno(fid), offset, whence) == (int64_t)-1 ? -1 : 0;
#else
#if defined(_WIN32_WCE)
  return fseek(fid, (long)(offset), whence);
#else
  return fseeko(fid, (off_t)(offset), whence);
#endif
#endif
}

int64_t TellFile64(FILE *fid) {
  if (fid == NULL) return -1;

  clearerr(fid);
  fflush(fid);
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
  return _telli64(_fileno(fid));
#else
#if defined(_WIN32_WCE)
  return ftell(fid);
#else
  return ftello(fid);
#endif
#endif
}

bool FileIsSeekable(FILE *fid) {
  if (SeekFile64(fid, 1, SEEK_CUR) < 0) {
    return false;
  }

  SeekFile64(fid, -1, SEEK_CUR); // seek back to where we were
  return true;
}
