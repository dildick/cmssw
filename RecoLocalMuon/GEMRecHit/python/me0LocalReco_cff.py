import FWCore.ParameterSet.Config as cms

from RecoLocalMuon.GEMRecHit.me0RecHits_cfi import *
from RecoLocalMuon.GEMRecHit.me0PseudoRecHits_cfi import *
from RecoLocalMuon.GEMSegment.me0Segments_cfi import *
from RecoLocalMuon.GEMSegment.me0PseudoSegments_cfi import *

me0LocalReco = cms.Sequence(me0PseudoRecHits *
                            me0PseudoSegments *
                            me0RecHits *
                            me0Segments)
