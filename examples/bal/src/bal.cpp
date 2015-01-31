//==============================================================================
//         Copyright 2015 INSTITUT PASCAL UMR 6602 CNRS/Univ. Clermont II
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================


#include <iostream>
#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/sinc.hpp>

//#define USE_TOON
#include <libv/lma/lma.hpp>

struct Camera
{
  Eigen::Matrix3d rotation;
  Eigen::Vector3d translation;
  double a,b,c;
  Camera():rotation(Eigen::Matrix3d::Identity()),translation(0,0,0),a(0),b(0),c(0){}

  void apply_rotation(const double delta[9])
  {
    // update a rotation according using exponential map
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


struct BALProblem 
{
  bool load(std::string filename)
  {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    file >> num_cameras_ >> num_points_ >> num_observations_;

    std::cout << boost::format("Cameras %1%, Points 3D %2%, Observations %3%")%num_cameras_%num_points_%num_observations_ << std::endl;

    point_index_.resize(num_observations_);
    camera_index_.resize(num_observations_);
    observations_.resize(num_observations_);
    
    cameras.resize(num_cameras_);
    points3d.resize(num_points_);

    for(int i = 0; i < num_observations_; ++i)
    {
      file >> camera_index_[i]
           >> point_index_[i]
           >> observations_[i].x()
           >> observations_[i].y();
    }

    for(int i = 0; i < num_cameras_; ++i)
    {
      std::array<double,9> params;
      for(int k = 0 ; k < 9 ; ++k)
        file >> params[k];
      cameras[i].apply_rotation(params.data());
    }

    for(int i = 0; i < num_points_; ++i)
    {
      file >> points3d[i].x() 
           >> points3d[i].y()
           >> points3d[i].z();
    }

    return true;
  }

  int num_cameras_;
  int num_points_;
  int num_observations_;
  int num_parameters_;

  std::vector<int> point_index_;
  std::vector<int> camera_index_;

  template<class T> using AlignedVector = std::vector<T,Eigen::aligned_allocator<T>>;
  AlignedVector<Eigen::Vector3d> points3d;
  AlignedVector<Eigen::Vector2d> observations_;
  AlignedVector<Camera> cameras;
};


void apply_small_rotation(Eigen::Matrix3d &m, double x, double y, double z)
{
  Eigen::Matrix3d skrew;
  skrew << 1, -z, y, z, 1, -x, -y, x, 1;
  m *= skrew;
}

namespace lma
{
  // Update policy of a Camera
  void apply_increment(Camera& camera, const double delta[9], const Adl&)
  {
    camera.apply_rotation(delta);
  }
  
  // Only for numerical derivative:
  // Update policy of a Camera according to the Ie parameter (h ~ 1e-8).
  template<int I> void apply_small_increment(Camera& camera, double h, v::numeric_tag<I>, const Adl&)
  {
    if (I<3) apply_small_rotation(camera.rotation,I==0?h:0,I==1?h:0,I==2?h:0);
      else if (I==3) camera.translation.x() += h;
    else if (I==4) camera.translation.y() += h;
    else if (I==5) camera.translation.z() += h;
    else if (I==6) camera.a += h;
    else if (I==7) camera.b += h;
    else if (I==8) camera.c += h;
  }
  
  //degree of freedom of a Camera
  template<> struct Size<Camera> { enum {value = 9}; };

  // Nothing to specify for the 3D points (Eigen::Vector3d) :
  //   - degree of freedom of an Eigen::Vector is the size.
  //   - The update policy of an Eigen::Vector is trivial.

}


struct Reprojection
{
  const Eigen::Vector2d& obs;

  Reprojection(const Eigen::Vector2d& p2d):obs(p2d){}

  bool operator()(const Camera& camera, const Eigen::Vector3d& point, double (&error)[2]) const
  {
    const Eigen::Vector3d p = camera.rotation * point + camera.translation;
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
  template<> struct Name< Reprojection > { static std::string name(){ return "Reprojection"; } };
}

void call_lma(std::string file, double lambda, int iteration_max)
{

  BALProblem bal_problem;
  if (!bal_problem.load(file)) {
    std::cerr << "ERROR: unable to open file " << file << "\n";
    return ;
  }

  lma::Solver<Reprojection> solver(lambda,iteration_max);

  for (int i = 0; i < bal_problem.num_observations_; ++i)
    solver.add(
                Reprojection(bal_problem.observations_[i]),
                &bal_problem.cameras[bal_problem.camera_index_[i]], 
                &bal_problem.points3d[bal_problem.point_index_[i]]
              );

#ifdef USE_TOON
  solver.solve(lma::TOON_DENSE_SCHUR,lma::enable_verbose_output());
#else
  solver.solve(lma::DENSE_SCHUR,lma::enable_verbose_output());
#endif
}



int main(int argc, char** argv)
{
  if (argc == 4) 
    call_lma(argv[1],boost::lexical_cast<int>(argv[2]),boost::lexical_cast<int>(argv[3]));
  else
    std::cerr << "usage: test-lma3d <bal_problem> lambda nb_iteration_max\n";

  return 0;
}
