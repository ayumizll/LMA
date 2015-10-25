//==============================================================================
//         Copyright 2015 INSTITUT PASCAL UMR 6602 CNRS/Univ. Clermont II
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================



#include <libv/lma/graph/graph.hpp>

using namespace lma;

// =======================================================================================//
//  USAGE OF DEPENDENCIES GRAPH TO MANAGE DIFFERENT CONFIGURATION OF OPTIMIZATION PROBLEM //
// =======================================================================================//

// User defined :
//   - the poses parameters to optimize
//   - the 3D points parameters to optimize
//   - the reprojections errors of the cost function
// Graph automatically :
//   1. generates a solver with LMA
//   2. manages the graph coverage
//   3. fills the solver with the constraints
//   4. solves the problem with the given policies

// =======================================================================================//
// The following lines are all the user needs to define for a bundle adjustment problem   //
// =======================================================================================//


// a simple pose type
using Pose = Eigen::Matrix<double,6,1>;
// a 3d points type
using Point3D = Eigen::Matrix<double,3,1>;

// an example of a reprojection function
struct Reprojection
{
  bool operator()(const Pose& pose, const Point3D& point3d, double (&res) [2]) const
  {
    // some computations ...
    res[0] = pose.norm() + point3d.norm();
    res[1] = pose.squaredNorm() + point3d.squaredNorm();
    return true;
  }
};


// Parameter node of type Pose : Poses
struct Poses : Parameter<Pose>
{
  AlignedVector<Pose> poses;

  void cover(auto&)
  {
    // list the pose to optimize
    poses = {Pose::Random(),Pose::Random()};
  }
};


// Parameter node of type 3D point : Points3D
struct Points3D : Parameter<Point3D>
{
  AlignedVector<Point3D> points;

  void cover(auto& graph)
  {
    // get the poses node
    Poses& poses = graph.node();
    // in real case, poses node needs to be covered
    graph.cover(poses);
    // list the 3D points to optimize
    points = {Point3D::Random(),Point3D::Random(),Point3D::Random()};
  }
};


// Constraint node of type reprojection : Reprojections
struct Reprojections : Constraint<Reprojection>
{
  void cover(auto& graph)
  {
    
    // get the 3D points node
    Points3D& points3d = graph.node();

    // points 3D node needs to be covered
    graph.cover(points3d);

    // get the poses node
    Poses& poses = graph.node();

    // list all the reprojection error
    for(Pose& pose : poses.poses)
      for(auto& p3d : points3d.points)
        add(Reprojection(),&pose,&p3d);
  }
};


int main()
{
  // create, fill and solve a mini bundle adjustement problem:
  Graph<Constraints(Reprojections),Parameters(Poses,Points3D)>().solve(DENSE,enable_verbose_output());
}