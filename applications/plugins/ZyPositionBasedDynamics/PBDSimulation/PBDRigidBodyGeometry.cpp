#include "PBDRigidBodyGeometry.h"

using namespace sofa::simulation::PBDSimulation;

PBDRigidBodyGeometry::PBDRigidBodyGeometry() :
    m_mesh()
{
}

PBDRigidBodyGeometry::~PBDRigidBodyGeometry(void)
{
    m_mesh.release();
}

PBDRigidBodyGeometry::Mesh &PBDRigidBodyGeometry::getMesh()
{
    return m_mesh;
}

void PBDRigidBodyGeometry::initMesh(const unsigned int nVertices, const unsigned int nFaces, const Vector3r *vertices, const unsigned int* indices, const Mesh::UVIndices& uvIndices, const Mesh::UVs& uvs, const Vector3r &scale)
{
    m_mesh.release();
    m_mesh.initMesh(nVertices, nFaces * 2, nFaces);
    m_vertexData_local.resize(nVertices);
    m_vertexData.resize(nVertices);
    for (unsigned int i = 0; i < nVertices; i++)
    {
        m_vertexData_local.getPosition(i) = vertices[i].cwiseProduct(scale);
        m_vertexData.getPosition(i) = m_vertexData_local.getPosition(i);
    }

    for (unsigned int i = 0; i < nFaces; i++)
    {
        m_mesh.addFace(&indices[3 * i]);
    }
    m_mesh.copyUVs(uvIndices, uvs);
    m_mesh.buildNeighbors();
    updateMeshNormals(m_vertexData);
}

void PBDRigidBodyGeometry::updateMeshNormals(const VertexData &vd)
{
    m_mesh.updateNormals(vd, 0);
    m_mesh.updateVertexNormals(vd);
}

void PBDRigidBodyGeometry::updateMeshTransformation(const Vector3r &x, const Matrix3r &R)
{
    for (unsigned int i = 0; i < m_vertexData_local.size(); i++)
    {
        m_vertexData.getPosition(i) = R * m_vertexData_local.getPosition(i) + x;
    }
    updateMeshNormals(m_vertexData);
}

VertexData & PBDRigidBodyGeometry::getVertexData()
{
    return m_vertexData;
}

const VertexData & PBDRigidBodyGeometry::getVertexData() const
{
    return m_vertexData;
}

VertexData & PBDRigidBodyGeometry::getVertexDataLocal()
{
    return m_vertexData_local;
}

const VertexData & PBDRigidBodyGeometry::getVertexDataLocal() const
{
    return m_vertexData_local;
}