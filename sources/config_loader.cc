#include "config_loader.h"

ConfigLoader::ConfigLoader()
{

}


ConfigLoader::ConfigLoader(const char *fname)
{
  std::ifstream stream;
  stream.open(fname);

  std::string line;

  while (std::getline(stream, line)) {
    parseLine(line);
  }

  for (auto &iter: values) {
    std::cout << iter.first << " => " << iter.second << std::endl;
  }
}

// Code here is mostly based at http://www.cplusplus.com/forum/general/21115/
// and http://www.adp-gmbh.ch/cpp/config_file.html
// (Duoas and René Nyffenegger, respectively)
void ConfigLoader::parseLine(std::string line)
{
  // Remove comments
  auto comment_start = line.find_first_of('#');
  if (comment_start != std::string::npos) line.erase(comment_start);

  // Skip blank lines
  std::string::size_type begin = line.find_first_not_of(" \f\t\v");
  if (begin == std::string::npos) return;

  // Select key
  std::string key, value;
  auto end = line.find('=', begin);
  key = line.substr(begin, end - begin);

  // (No leading or trailing whitespace allowed)
  key.erase(key.find_last_not_of(" \f\t\v") + 1);

  // No blank keys allowed
  if (key.empty()) return;

  // Extract the value (no leading or trailing whitespace allowed)
  begin = line.find_first_not_of(" \f\n\r\t\v", end + 1);
  end   = line.find_last_not_of(" \f\n\r\t\v") + 1;

  value = line.substr(begin, end - begin);

  values[key] = value;
}

template<>
bool ConfigLoader::get<bool>(const std::string &name)
{
  return values[name] == "true";
}

template<>
std::string ConfigLoader::get<std::string>(const std::string &name)
{
  return values[name];
}

bool ConfigLoader::getBool(const std::string &name)
{
  return get<bool>(name);
}

int ConfigLoader::getInt(const std::string &name)
{
  return get<int>(name);
}

double ConfigLoader::getDouble(const std::string &name)
{
  return get<double>(name);
}

std::string ConfigLoader::getString(const std::string &name)
{
  return get<std::string>(name);
}
