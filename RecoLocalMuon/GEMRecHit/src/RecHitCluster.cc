#include "RecoLocalMuon/GEMRecHit/interface/RecHitCluster.h"

RecHitCluster::RecHitCluster() : fstrip(0), lstrip(0), bunchx(0)
{
}

RecHitCluster::RecHitCluster(int fs, int ls, int bx) :
  fstrip(fs), lstrip(ls), bunchx(bx)
{
}

RecHitCluster::~RecHitCluster()
{
}

int
RecHitCluster::firstStrip() const
{
  return fstrip;
}

int
RecHitCluster::lastStrip() const
{
  return lstrip;
}

int
RecHitCluster::clusterSize() const
{
  return -(fstrip-lstrip)+1;
}

int
RecHitCluster::bx() const
{
  return bunchx;
}

bool RecHitCluster::isAdjacent(const RecHitCluster& cl) const{

    return ((cl.firstStrip() == this->firstStrip()-1) &&
	    (cl.bx() == this->bx()));
}

void RecHitCluster::merge(const RecHitCluster& cl){

   if(this->isAdjacent(cl))
     {
       fstrip = cl.firstStrip();
     }
}

bool RecHitCluster::operator<(const RecHitCluster& cl) const{

if(cl.bx() == this->bx())
 return cl.firstStrip()<this->firstStrip();
else
 return cl.bx()<this->bx();
}

bool
RecHitCluster::operator==(const RecHitCluster& cl) const {
  return ( (this->clusterSize() == cl.clusterSize()) &&
	   (this->bx()          == cl.bx())          &&
	   (this->firstStrip()  == cl.firstStrip()) );
}
