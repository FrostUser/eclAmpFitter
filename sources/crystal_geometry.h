#ifndef CRYSTAL_GEOMETRY_H
#define CRYSTAL_GEOMETRY_H

class ECLCrystalGeometry
{
private:
  /// First crystal id in the beginning of i-th ECL ring.
  /// Taken from basf2/ecl/data/ecl_channels_map (two last columns)
  const int m_ring_start_id[70] = {
    // forward (theta_id 0-12)
    1,    49,   97,   161,  225,  289,  385,  481,  577,  673,
    769,  865,  1009,
    // barrel (theta_id 13-58)
    1153, 1297, 1441, 1585, 1729, 1873, 2017, 2161, 2305, 2449,
    2593, 2737, 2881, 3025, 3169, 3313, 3457, 3601, 3745, 3889,
    4033, 4177, 4321, 4465, 4609, 4753, 4897, 5041, 5185, 5329,
    5473, 5617, 5761, 5905, 6049, 6193, 6337, 6481, 6625, 6769,
    6913, 7057, 7201, 7345, 7489, 7633,
    // forward (theta_id 59-68)
    7777, 7921, 8065, 8161, 8257, 8353, 8449, 8545, 8609, 8673,
    // last_crystal+1
    8736+1
  };

  static ECLCrystalGeometry* m_instance;
  /**
   * mapping[0] : CellID -> CrateID
   * mapping[1] : CellID -> ShaperID
   * mapping[2] : CellID -> ShaperChannel
   */
  int** mapping;

public:
  ECLCrystalGeometry();

  static ECLCrystalGeometry* getInstance();

  int getPhiId(int ch);
  int getThetaId(int ch);

  int getCrate(int ch);
  int getShaper(int ch);
  int getShaperChannel(int ch);
};

#endif // CRYSTAL_GEOMETRY_H
