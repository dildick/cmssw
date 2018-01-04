#ifndef RecoLocalMuon_GEMRecHit_RecHitCluster_h
#define RecoLocalMuon_GEMRecHit_RecHitCluster_h

#include <stdint.h>
#include <set>

class RecHitCluster
{
 public:
  RecHitCluster();
  RecHitCluster(int fs,int ls, int bx);
  ~RecHitCluster();

  int firstStrip() const;
  int lastStrip() const;
  int clusterSize() const;
  int bx() const;

  void merge(const RecHitCluster& cl);

  bool operator<(const RecHitCluster& cl) const;
  bool operator==(const RecHitCluster& cl) const;
  bool isAdjacent(const RecHitCluster& cl) const;

 private:
  uint16_t fstrip;
  uint16_t lstrip;
  int16_t bunchx;
};

typedef std::set<RecHitCluster> RecHitClusterContainer;

#endif
