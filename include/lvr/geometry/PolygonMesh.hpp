/*
 * PolygonMesh.hpp
 *
 *  Created on: 06.03.2014
 *      Author: Dominik Feldschnieders (dofeldsc@uos.de)
 */

#ifndef POLYGONMESH_HPP_
#define POLYGONMESH_HPP_

#include "PolygonRegion.hpp"
#include <vector>

namespace lvr {

/**
 * @brief This class represents a hole PolygonMesh, with all its PolygonRegions.
 * 		There is no functionality for the fuision in this class but it is the "main-container"
 * 		for the data, which is needed by PolygonFusion.
 */
template<typename VertexT, typename NormalT>
class PolygonMesh {
public:

	/**
	 * @brief standard constructor
	 */
	PolygonMesh();

	/**
	 * @brief constructor
	 *
	 * @param first_regions the "first" Polygonregions of this Mesh, first means if you do not want to start with an empty Mesh
	 */
	PolygonMesh(std::vector<PolygonRegion<VertexT, NormalT>> first_regions);


	/**
	 * @brief destructor
	 */
	virtual ~PolygonMesh();


	/**
	 * @brief Add one new PolygonRegion, it will be pushed into the container (std::vector)
	 *
	 * @param polyregion the PolygonRegion, which will be added to the mesh
	 */
	void addPolyRegion(PolygonRegion<VertexT, NormalT> polyregion);


	/**
	 * @brief Add few new PolygonRegion, it will be pushed into the container (std::vector)
	 *
	 * @param polyregions a vector of PolygonRegions, which will be added to the mesh
	 */
	void addPolyRegions(std::vector<PolygonRegion<VertexT, NormalT>> polyregions);


	/**
	 * @brief Get all the regions of this mesh
	 *
	 * @return all (added) regions of this mesh
	 */
	std::vector<PolygonRegion<VertexT, NormalT>> getPolyRegions();


private:
	// container for all PolygonRegions in this Mesh
	std::vector<PolygonRegion<VertexT, NormalT>> m_regions;
};

} /* namespace lvr */

#include "PolygonMesh.tcc"
#endif /* POLYGONMESH_HPP_ */
