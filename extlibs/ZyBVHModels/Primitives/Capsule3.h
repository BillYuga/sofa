#ifndef CAPSULE3_H
#define CAPSULE3_H
// Geometric Tools, LLC
// Copyright (c) 1998-2014
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#include "DistanceComputable.h"
#include "Intersectable.h"
#include "Segment3.h"

namespace BVHModels
{

    template <typename Real>
    class Capsule3: public Intersectable<Real, Vec<3,Real> >, public DistanceComputable<Real, Vec<3,Real> >
    {
        public:
            // Construction and destruction.  A capsule is the set of points that are
            // equidistant from a segment, the common distance called the radius.
            Capsule3 ();  // uninitialized
            ~Capsule3 ();

            Capsule3 (const Segment3<Real>& segment, Real radius);

            Segment3<Real> Segment;
            Real Radius;

            /*virtual Real GetDistance(const DistanceComputable<Real, Vec<3,Real> >& other, DistanceResult& result);
            virtual Real GetSquaredDistance(const DistanceComputable<Real, Vec<3,Real> >& other, DistanceResult& result);

            virtual Real GetDistance(const DistanceComputable<Real, Vec<3, Real> > &other, Real t, const Vec<3,Real>& velocity0, const Vec<3,Real>& velocity1, DistanceResult& result);
            virtual Real GetSquaredDistance(const DistanceComputable<Real, Vec<3, Real> > &other, Real t, const Vec<3,Real>& velocity0, const Vec<3,Real>& velocity1, DistanceResult& result);*/
    };

    typedef Capsule3<float> Capsule3f;
    typedef Capsule3<double> Capsule3d;
}

#endif // CAPSULE3_H
