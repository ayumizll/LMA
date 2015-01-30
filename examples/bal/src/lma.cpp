//==============================================================================
//         Copyright 2015 INSTITUT PASCAL UMR 6602 CNRS/Univ. Clermont II
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================


//#define USE_TOON

#include "lma.hpp"
#include <libv/lma/lma.hpp>
#include "pb_loader.hpp"
#include <boost/math/special_functions/sinc.hpp>

typedef Eigen::Matrix<double,3,1> Point3d;
typedef Eigen::Matrix<double,2,1> Point2d;


struct Cam
{
  Eigen::Matrix3d rotation;
  Eigen::Vector3d translation;
  double a,b,c;
  Cam():rotation(Eigen::Matrix3d::Identity()),translation(0,0,0),a(0),b(0),c(0){}

  void apply_rotation(const double delta[9])
  {
    Eigen::Matrix3d r;
    r << 0,-delta[2], delta[1],delta[2],0,-delta[0],-delta[1],delta[0],0;
    const double 
      theta2 = r(0,1)*r(0,1) + r(0,2)*r(0,2) +  r(1,2)*r(1,2) + std::numeric_limits<double>::epsilon(),
      theta = std::sqrt(theta2);
    rotation *= Eigen::Matrix3d::Identity() + boost::math::sinc_pi(theta)*r+(1.0-std::cos(theta))/theta2*r*r;
    translation.x() += delta[3];
    translation.y() += delta[4];
    translation.z() += delta[5];
    a += delta[6];
    b += delta[7];
    c += delta[8];
  }
};

namespace lma
{
  void apply_increment(Cam& camera, const double delta[9], const Adl&)
  {
    camera.apply_rotation(delta);
  }
  
  void apply_small_rotation(Eigen::Matrix3d &m, double x, double y, double z)
  {
    Eigen::Matrix3d skrew;
    skrew << 1, -z, y, z, 1, -x, -y, x, 1;
    m *= skrew;
  }
  
  template<int I> void apply_small_increment(Cam& camera, double h, v::numeric_tag<I>, const Adl&)
  {
    if (I<3) apply_small_rotation(camera.rotation,I==0?h:0,I==1?h:0,I==2?h:0);
    else if (I==3) camera.translation.x() += h;
    else if (I==4) camera.translation.y() += h;
    else if (I==5) camera.translation.z() += h;
    else if (I==6) camera.a += h;
    else if (I==7) camera.b += h;
    else if (I==8) camera.c += h;
  }
  
  template<> struct Size<Cam> { enum {value = 9}; };
}



struct Reprojection
{
  const Point2d& obs;

  Reprojection(const Point2d& p2d):obs(p2d){}

  bool operator()(const Cam& camera, const Point3d& point, double (&error)[2]) const
  {
    const Point3d p = camera.rotation * point + camera.translation;
    const double xp = - p[0] / p[2],
      yp = - p[1] / p[2],
      & l1 = camera.b,
      & l2 = camera.c,
      r2 = xp*xp + yp*yp,
      distortion = double(1.0) + r2  * (l1 + l2  * r2);

    error[0] = camera.a * distortion * xp - obs[0];
    error[1] = camera.a * distortion * yp - obs[1];
    return true;
  }
};

namespace ttt
{
  template<> struct Name< Cam > { static std::string name(){ return "Camera"; } };
  template<> struct Name< Point3d > { static std::string name(){ return "Point3d"; } };
  template<> struct Name< Reprojection > { static std::string name(){ return "Reprojection"; } };
}

void call_lma(std::string file, double lambda, int iteration_max)
{

  BALProblem bal_problem;
  if (!bal_problem.LoadFile(file.c_str())) {
    std::cerr << "ERROR: unable to open file " << file << "\n";
    return ;
  }

  std::vector<Cam> vcam(bal_problem.num_cameras_);
  std::vector<Point3d> v3d(bal_problem.num_points_);
  std::vector<Point2d> v2d(bal_problem.num_observations());

  for(int i = 0 ; i < bal_problem.num_cameras_; ++i)
  {
    double* camera_ptr = bal_problem.mutable_cameras()+(i*9);
    vcam[i].apply_rotation(camera_ptr);
  }

  for(int i = 0 ; i < bal_problem.num_points_; ++i)
  {
    double* point3d_ptr = bal_problem.mutable_points()+(i*3);
    std::copy(point3d_ptr,point3d_ptr+3,v3d[i].data());
  }

  for (int i = 0; i < bal_problem.num_observations(); ++i)
  {
    v2d[i] << bal_problem.observations()[2*i], bal_problem.observations()[2*i+1];
  }

  utils::Tic<true> tic("LMA Total Time");
  utils::Tic<true> tic_fill("Fill bundle");

  lma::Solver<Reprojection> solver(lambda,iteration_max);

  for (int i = 0; i < bal_problem.num_observations(); ++i)
    solver.add(
                Reprojection(v2d[i]),
                &vcam[bal_problem.camera_index_[i]],
                &v3d[bal_problem.point_index_[i]]
              );

  tic_fill.disp();

#ifdef USE_TOON
  solver.solve(lma::TOON_DENSE_SCHUR,lma::enable_verbose_output());
#else
  solver.solve(lma::DENSE_SCHUR,lma::enable_verbose_output());
#endif

  tic.disp();
}
