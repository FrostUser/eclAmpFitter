#include "fitting_tree.h"

#include <iostream>

FittingTree::FittingTree()
{
  m_tree = new TTree("tree", "Fitting results");
}

FittingTree::~FittingTree()
{
//  delete m_tree;
}

int FittingTree::Fill()
{
  return m_tree->Fill();
}

int FittingTree::Write(const char* name)
{
  return m_tree->Write();
}

TTree* FittingTree::getTree()
{
  return m_tree;
}

void FittingTree::setInt(std::string branch, int newval)
{
  if (m_ints_map.find(branch) == m_ints_map.end()) {
    m_ints_map[branch] = 0;
    m_tree->Branch(branch.c_str(), &m_ints_map[branch]);
  }

  m_ints_map[branch] = newval;
}

void FittingTree::setDouble(std::string branch, double newval)
{
  if (m_doubles_map.find(branch) == m_doubles_map.end()) {
    m_doubles_map[branch] = 0;
    m_tree->Branch(branch.c_str(), &m_doubles_map[branch]);
  }

  m_doubles_map[branch] = newval;
}
