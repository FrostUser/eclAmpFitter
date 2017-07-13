#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

/**
 * Class to store configuration and load it from file.
 */
class ConfigLoader
{
public:
  /**
   * Default constructor, creates standard configuration.
   */
  ConfigLoader();
  /**
   * Constructor to load configuration from file.
   */
  ConfigLoader(const char* fname);

  void parseLine(std::string line);

  template <typename T = std::string>
  T get(const std::string &name);

  template <typename T>
  void set(const std::string &name, const T &newval);

  bool getBool(const std::string &name);
  int getInt(const std::string &name);
  double getDouble(const std::string &name);
  std::string getString(const std::string &name);

private:
  std::map<std::string, std::string> values;
};

/********************************************/
/***        TEMPLATE DEFINITIONS          ***/
/********************************************/

template <typename T>
void ConfigLoader::set(const std::string &name, const T &newval)
{
  std::ostringstream convert;
  convert << newval;

  values[name] = convert.str();
}

template <typename T>
T ConfigLoader::get(const std::string &name)
{
  std::istringstream convert(values[name]);
  T ret;
  convert >> ret;
  return ret;
}

#endif // CONFIG_LOADER_H
