#ifndef FITTING_TREE
#define FITTING_TREE

#include <TTree.h>
#include <map>
#include <string>

class FittingTree
{
private:
  TTree* m_tree;

  std::map<std::string, int> m_ints_map;
  std::map<std::string, double> m_doubles_map;

public:
  FittingTree();
  ~FittingTree();

  int Fill();
  int Write(const char *name = 0);

  TTree* getTree();

  /**
   * @return Success of setting operation.
   */
  void setInt(std::string branch, int newval);
  /**
   * @return Success of setting operation.
   */
  void setDouble(std::string branch, double newval);
};

#endif // FITTING_TREE

