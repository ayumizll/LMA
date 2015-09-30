#include <libv/lma/lma.hpp>

using namespace lma;
using namespace Eigen;

int main(){

  auto f = [] (Vector4d sphere, Vector3d point, double& error)
  {
    error = (sphere.head<3>() - point).norm() - sphere[3];
    return true;
  };
  
  Vector4d sphere(0,0,0,100);
  Vector3d point(60,30,80);
  
  Solver<decltype(f)>().add(f,&sphere,&point).solve(DENSE_SCHUR);
}