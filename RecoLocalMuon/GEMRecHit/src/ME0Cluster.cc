#include "RecoLocalMuon/GEMRecHit/interface/ME0Cluster.h"

ME0Cluster::ME0Cluster() : fstrip(0), lstrip(0), bunchx(0)
{
}

ME0Cluster::ME0Cluster(int fs, int ls, int bx) :
  fstrip(fs), lstrip(ls), bunchx(bx)
{
}

ME0Cluster::~ME0Cluster()
{
}

int
ME0Cluster::firstStrip() const
{
  return fstrip;
}

int
ME0Cluster::lastStrip() const
{
  return lstrip;
}

int
ME0Cluster::clusterSize() const
{
  return -(fstrip-lstrip)+1;
}

int
ME0Cluster::bx() const
{
  return bunchx;
}

bool ME0Cluster::isAdjacent(const ME0Cluster& cl) const{

    return ((cl.firstStrip() == this->firstStrip()-1) &&
	    (cl.bx() == this->bx()));
}

void ME0Cluster::merge(const ME0Cluster& cl){

   if(this->isAdjacent(cl))
     {
       fstrip = cl.firstStrip();
     }
}

bool ME0Cluster::operator<(const ME0Cluster& cl) const{

if(cl.bx() == this->bx())
 return cl.firstStrip()<this->firstStrip();
else
 return cl.bx()<this->bx();
}

bool
ME0Cluster::operator==(const ME0Cluster& cl) const {
  return ( (this->clusterSize() == cl.clusterSize()) &&
	   (this->bx()          == cl.bx())          &&
	   (this->firstStrip()  == cl.firstStrip()) );
}
