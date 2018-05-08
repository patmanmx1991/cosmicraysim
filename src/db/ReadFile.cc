#include <db/ReadFile.hh>

namespace COSMIC {

namespace DBUtils {

int ReadFile(const std::string &filename, std::string &filecontents)
{
  bool bzip2 = true;

  const int nmax = 4096;
  char buffer[nmax];
  int nread = 0;

  bzip2 = false;

  if (!bzip2) {
    // Not BZIP2 format, so just dump the file contents raw into the string
    FILE *f = fopen(filename.c_str(), "r");

    if (f == NULL)
      return -1;

    filecontents = "";
    while ( (nread = fread(buffer, sizeof(char), nmax, f)) )
      filecontents.append(buffer, nread);

    fclose(f);
  }

  return 0;
}

std::vector<std::string> Expand(std::string pattern) {
  glob_t g;
  std::vector<std::string> vecs;
  if (glob(pattern.c_str(), 0, 0, &g) == 0) {
    for (unsigned i = 0; i < g.gl_pathc; i++) {
      std::string path(g.gl_pathv[i]);
      vecs.push_back(path);
    }
  }
  globfree(&g);
  return vecs;
}


std::vector<std::string> ParseToStr(std::string str, const char* del) {
  std::istringstream stream(str);
  std::string temp_string;
  std::vector<std::string> vals;

  while (std::getline(stream >> std::ws, temp_string, *del)) {
    if (temp_string.empty()) continue;
    vals.push_back(temp_string);
  }

  return vals;
}


} // - namespace DBUtils
} // - namespace COSMIC
