/******************************************************************************
*                 SOFA, Simulation Open-Framework Architecture                *
*                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this program. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#pragma once
#include <SofaMeshCollision/config.h>

#include <sofa/core/CollisionModel.h>
#include <SofaMeshCollision/fwd.h>
#include <SofaBaseMechanics/MechanicalObject.h>
#include <sofa/core/topology/BaseMeshTopology.h>
#include <sofa/defaulttype/VecTypes.h>

namespace sofa::component::collision
{

template<class DataTypes>
class PointCollisionModel;

class PointLocalMinDistanceFilter;

template<class TDataTypes>
class TPoint : public core::TCollisionElementIterator<PointCollisionModel<TDataTypes> >
{
public:
    typedef TDataTypes DataTypes;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef PointCollisionModel<DataTypes> ParentModel;

    TPoint(ParentModel* model, sofa::Index index);
    TPoint() {}

    explicit TPoint(const core::CollisionElementIterator& i);

    const Coord& p() const;
    const Coord& pFree() const;
    const Deriv& v() const;
    Deriv n() const;

    /// Return true if the element stores a free position vector
    bool hasFreePosition() const;

    bool testLMD(const sofa::defaulttype::Vector3 &, double &, double &);
};

template<class TDataTypes>
class SOFA_SOFAMESHCOLLISION_API PointCollisionModel : public core::CollisionModel
{
public:
    SOFA_CLASS(SOFA_TEMPLATE(PointCollisionModel, TDataTypes), core::CollisionModel);

    typedef TDataTypes DataTypes;
    typedef DataTypes InDataTypes;
    typedef PointCollisionModel<DataTypes> ParentModel;
    typedef typename DataTypes::VecCoord VecCoord;
    typedef typename DataTypes::VecDeriv VecDeriv;
    typedef typename DataTypes::Coord Coord;
    typedef typename DataTypes::Deriv Deriv;
    typedef TPoint<DataTypes> Element;
    typedef helper::vector<Index> VecIndex;

    friend class TPoint<DataTypes>;
protected:
    PointCollisionModel();
public:
    void init() override;

    // -- CollisionModel interface

    void resize(Size size) override;

    void computeBoundingTree(int maxDepth=0) override;

    void computeContinuousBoundingTree(double dt, int maxDepth=0) override;

    void draw(const core::visual::VisualParams*, Index index) override;
    void draw(const core::visual::VisualParams* vparams) override;

    bool canCollideWithElement(Index index, CollisionModel* model2, Index index2) override;

    core::behavior::MechanicalState<DataTypes>* getMechanicalState() { return mstate; }

    Deriv getNormal(Index index){ return (normals.size()) ? normals[index] : Deriv();}

    PointLocalMinDistanceFilter *getFilter() const;

    void setFilter(PointLocalMinDistanceFilter * /*lmdFilter*/);

    const Deriv& velocity(Index index) const;

    Data<bool> bothSide; ///< to activate collision on both side of the point model (when surface normals are defined on these points)

    /// Pre-construction check method called by ObjectFactory.
    /// Check that DataTypes matches the MechanicalState.
    template<class T>
    static bool canCreate(T*& obj, core::objectmodel::BaseContext* context, core::objectmodel::BaseObjectDescription* arg)
    {
        if (dynamic_cast<core::behavior::MechanicalState<DataTypes>*>(context->getMechanicalState()) == nullptr)
        {
            arg->logError(std::string("No mechanical state with the datatype '") + DataTypes::Name() +
                          "' found in the context node.");
            return false;
        }
        return BaseObject::canCreate(obj, context, arg);
    }

    void computeBBox(const core::ExecParams* params, bool onlyVisible) override;
    void updateNormals();

    sofa::core::topology::BaseMeshTopology* getCollisionTopology() override
    {
        return l_topology.get();
    }

protected:

    core::behavior::MechanicalState<DataTypes>* mstate;

    Data<bool> computeNormals; ///< activate computation of normal vectors (required for some collision detection algorithms)

    VecDeriv normals;

    PointLocalMinDistanceFilter *m_lmdFilter;

    Data<bool> m_displayFreePosition; ///< Display Collision Model Points free position(in green)
                                      
    /// Link to be set to the topology container in the component graph.
    SingleLink<PointCollisionModel<DataTypes>, sofa::core::topology::BaseMeshTopology, BaseLink::FLAG_STOREPATH | BaseLink::FLAG_STRONGLINK> l_topology;

};


template<class DataTypes>
inline TPoint<DataTypes>::TPoint(ParentModel* model, sofa::Index index)
    : core::TCollisionElementIterator<ParentModel>(model, index)
{

}

template<class DataTypes>
inline TPoint<DataTypes>::TPoint(const core::CollisionElementIterator& i)
    : core::TCollisionElementIterator<ParentModel>(static_cast<ParentModel*>(i.getCollisionModel()), i.getIndex())
{

}

template<class DataTypes>
inline const typename DataTypes::Coord& TPoint<DataTypes>::p() const { return this->model->mstate->read(core::ConstVecCoordId::position())->getValue()[this->index]; }

template<class DataTypes>
inline const typename DataTypes::Coord& TPoint<DataTypes>::pFree() const
{
    if (hasFreePosition())
        return this->model->mstate->read(core::ConstVecCoordId::freePosition())->getValue()[this->index];
    else
        return p();
}

template<class DataTypes>
inline const typename DataTypes::Deriv& TPoint<DataTypes>::v() const { return this->model->mstate->read(core::ConstVecDerivId::velocity())->getValue()[this->index]; }

template<class DataTypes>
inline const typename DataTypes::Deriv& PointCollisionModel<DataTypes>::velocity(Index index) const { return mstate->read(core::ConstVecDerivId::velocity())->getValue()[index]; }

template<class DataTypes>
inline typename DataTypes::Deriv TPoint<DataTypes>::n() const { return ((unsigned)this->index<this->model->normals.size()) ? this->model->normals[this->index] : Deriv(); }

template<class DataTypes>
inline bool TPoint<DataTypes>::hasFreePosition() const { return this->model->mstate->read(core::ConstVecCoordId::freePosition())->isSet(); }

template <class TDataTypes> using TPointModel [[deprecated("The TPointModel is now deprecated, please use PointCollisionModel instead. Compatibility stops at v20.06")]] = PointCollisionModel<TDataTypes>;
using PointModel [[deprecated("The PointModel is now deprecated, please use PointCollisionModel<sofa::defaulttype::Vec3Types> instead. Compatibility stops at v20.06")]] = PointCollisionModel<sofa::defaulttype::Vec3Types>;
using Point = TPoint<sofa::defaulttype::Vec3Types>;

#if  !defined(SOFA_COMPONENT_COLLISION_POINTCOLLISIONMODEL_CPP)
extern template class SOFA_SOFAMESHCOLLISION_API PointCollisionModel<defaulttype::Vec3Types>;

#endif

//bool Point::testLMD(const Vector3 &PQ, double &coneFactor, double &coneExtension);

} //namespace sofa::component::collision
