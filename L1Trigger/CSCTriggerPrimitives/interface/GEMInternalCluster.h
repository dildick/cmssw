#ifndef L1Trigger_CSCTriggerPrimitives_GEMInternalCluster_h
#define L1Trigger_CSCTriggerPrimitives_GEMInternalCluster_h

/** \class GEMInternalCluster
 *
 * Helper class to contain detids, clusters and corresponding
 * 1/8-strips and wiregroups for easy matching with CSC TPs
 *
 * Author: Sven Dildick
 *
 */

#include "DataFormats/MuonDetId/interface/GEMDetId.h"
#include "DataFormats/GEMDigi/interface/GEMPadDigiCluster.h"

class GEMInternalCluster {
public:
  // constructor
  GEMInternalCluster(const GEMDetId& id, const GEMPadDigiCluster& cluster1, const GEMPadDigiCluster& cluster2);

  GEMDetId id() const { return id_; }
  GEMPadDigiCluster cl1() const { return cl1_; }
  GEMPadDigiCluster cl2() const { return cl2_; }

  int bx() const { return bx_; }
  int roll() const { return id_.roll(); }
  int layer1_pad() const { return layer1_pad_; }
  int layer1_size() const { return layer1_size_; }
  int layer2_pad() const { return layer2_pad_; }
  int layer2_size() const { return layer2_size_; }
  int min_wg() const { return min_wg_; }
  int max_wg() const { return max_wg_; }
  bool isCoincidence() const { return isCoincidence_; }

  // first and last 1/8-strips
  int layer1_first_es() const { return layer1_first_es_; }
  int layer2_first_es() const { return layer2_first_es_; }
  int layer1_last_es() const { return layer1_last_es_; }
  int layer2_last_es() const { return layer2_last_es_; }

  int layer1_first_es_me1a() const { return layer1_first_es_me1a_; }
  int layer2_first_es_me1a() const { return layer2_first_es_me1a_; }
  int layer1_last_es_me1a() const { return layer1_last_es_me1a_; }
  int layer2_last_es_me1a() const { return layer2_last_es_me1a_; }

  // middle 1/8-strips (sum divided by two)
  int layer1_middle_es() const { return layer1_middle_es_; }
  int layer2_middle_es() const { return layer2_middle_es_; }

  int layer1_middle_es_me1a() const { return layer1_middle_es_me1a_; }
  int layer2_middle_es_me1a() const { return layer2_middle_es_me1a_; }

  // setters
  void set_layer1_first_es(const int es) { layer1_first_es_ = es; }
  void set_layer2_first_es(const int es) { layer2_first_es_ = es; }
  void set_layer1_last_es(const int es) { layer1_last_es_ = es; }
  void set_layer2_last_es(const int es) { layer2_last_es_ = es; }

  void set_layer1_first_es_me1a(const int es) { layer1_first_es_me1a_ = es; }
  void set_layer2_first_es_me1a(const int es) { layer2_first_es_me1a_ = es; }
  void set_layer1_last_es_me1a(const int es) { layer1_last_es_me1a_ = es; }
  void set_layer2_last_es_me1a(const int es) { layer2_last_es_me1a_ = es; }

  // setters for middle 1/8-strip
  void set_layer1_middle_es(const int es) { layer1_middle_es_ = es; }
  void set_layer2_middle_es(const int es) { layer2_middle_es_ = es; }
  void set_layer1_middle_es_me1a(const int es) { layer1_middle_es_me1a_ = es; }
  void set_layer2_middle_es_me1a(const int es) { layer2_middle_es_me1a_ = es; }

  // set the corresponding wiregroup numbers
  void set_min_wg(const int wg) { min_wg_ = wg; }
  void set_max_wg(const int wg) { max_wg_ = wg; }

  bool has_cluster(const GEMPadDigiCluster& cluster) const;

  // equality operator: detid, cluster 1 and cluster 2
  bool operator==(const GEMInternalCluster& cluster) const;

private:
  /*
    Detector id. There are three cases. For single clusters in layer 1
    the GEMDetId in layer 1 is stored. Similarly, for single clusters in
    layer 2 the GEMDetId in layer 2 is stored. For coincidences the  GEMDetId
    in layer 1 is stored
  */
  GEMDetId id_;
  GEMPadDigiCluster cl1_;
  GEMPadDigiCluster cl2_;

  // bunch crossing
  int bx_;

  // starting pads and sizes of the clusters in each layer
  // depending on the presence of a coincidence, layer 1, layer2, or both
  // can be filled
  int layer1_pad_;
  int layer1_size_;
  int layer2_pad_;
  int layer2_size_;

  // corresponding CSC 1/8-strip coordinates (es) of the cluster
  // in each layer (if applicable)
  int layer1_first_es_;
  int layer1_last_es_;
  int layer2_first_es_;
  int layer2_last_es_;
  // for ME1/a
  int layer1_first_es_me1a_;
  int layer1_last_es_me1a_;
  int layer2_first_es_me1a_;
  int layer2_last_es_me1a_;

  // middle CSC 1/8-strip
  int layer1_middle_es_;
  int layer2_middle_es_;
  // for ME1/a
  int layer1_middle_es_me1a_;
  int layer2_middle_es_me1a_;

  // corresponding min and max wiregroup
  int min_wg_;
  int max_wg_;

  // flag to signal if it is a coincidence
  bool isCoincidence_;
};

#endif
