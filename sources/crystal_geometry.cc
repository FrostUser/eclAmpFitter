#include "crystal_geometry.h"

ECLCrystalGeometry* ECLCrystalGeometry::m_instance = 0;

ECLCrystalGeometry::ECLCrystalGeometry()
{
}

ECLCrystalGeometry* ECLCrystalGeometry::getInstance()
{
  if (m_instance == 0) m_instance = new ECLCrystalGeometry();

  return m_instance;
}

int ECLCrystalGeometry::getPhiId(int ch)
{
  for (int i = 0; i < 69; i++) {
    if (ch < m_ring_start_id[i+1])
      return ch - m_ring_start_id[i];
  }

  return -1;
}

int ECLCrystalGeometry::getThetaId(int ch)
{
  for (int i = 0; i < 69; i++) {
    if (ch < m_ring_start_id[i+1])
      return i;
  }

  return -1;
}
