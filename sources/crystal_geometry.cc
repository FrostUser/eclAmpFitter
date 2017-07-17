#include "crystal_geometry.h"

#include <iostream>
#include <string>
#include <sstream>

// ECL Mapping from ECLChannelMapper (basf2/ecl/data/ecl_channels_map.txt).
const char* ecl_channels_data =
{
  #include "ecl_channels_map.inc"
};

ECLCrystalGeometry* ECLCrystalGeometry::m_instance = 0;

ECLCrystalGeometry::ECLCrystalGeometry()
{
}

ECLCrystalGeometry* ECLCrystalGeometry::getInstance()
{
  if (m_instance == 0) {
    m_instance = new ECLCrystalGeometry();

    std::string str = ecl_channels_data;

    std::istringstream is(str);

    m_instance->mapping = new int*[3];
    for (int i = 0; i < 3; i++) {
      m_instance->mapping[i] = new int[8736+1];
    }

    double iCrate, iShaper, iChannel, thetaID, phiID, cellID;

    while (is.good()) {
      is >> iCrate >> iShaper >> iChannel >> thetaID >> phiID >> cellID;

      if (cellID > 0 && cellID < 8736+1) {
        m_instance->mapping[0][(int)cellID] = iCrate;
        m_instance->mapping[1][(int)cellID] = iShaper;
        m_instance->mapping[2][(int)cellID] = iChannel;
      }
    }
  }

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

int ECLCrystalGeometry::getCrate(int ch)
{
  return mapping[0][ch];
}

int ECLCrystalGeometry::getShaper(int ch)
{
  return mapping[1][ch];
}

int ECLCrystalGeometry::getShaperChannel(int ch)
{
  return mapping[2][ch];
}
