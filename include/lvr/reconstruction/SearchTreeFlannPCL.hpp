/* Copyright (C) 2011 Uni Osnabrück
 * This file is part of the LAS VEGAS Reconstruction Toolkit,
 *
 * LAS VEGAS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * LAS VEGAS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 */

/*
*  SearchTreePCL.hpp
*
*       Created on: 02.01.2012
*           Author: Florian Otte
 */

#ifndef SEARCHTREEFLANN_H_
#define SEARCHTREEFLANN_H_

// C++ Stl includes
#include <vector>

// PCL includes
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/io/pcd_io.h>
#include <pcl/features/normal_3d.h>
#include <pcl/features/normal_3d_omp.h>

// Superclass
#include "SearchTree.hpp"

#include <lvr/io/Timestamp.hpp>
#include <lvr/io/PointBuffer.hpp>

using std::vector;

namespace lvr {

/**
 * @brief SearchClass for point data.
 *
 *      This class uses the stann( https://sites.google.com/a/compgeom.com/stann/ )
 *      library to implement a nearest neighbour search for point-data.
 */
template< typename VertexT >
class SearchTreeFlannPCL : public SearchTree< VertexT>
{
public:

    typedef boost::shared_ptr< SearchTreeFlannPCL< VertexT> > Ptr;


    /**
     *  @brief Constructor. Takes the point-data and initializes the underlying searchtree.
     *
     *  @param loader  A PointBuffer point that holds the data.
     *  @param kn      The number of neighbour points used for normal estimation.
     *  @param ki      The number of neighbour points used for normal interpolation.
     *  @param kd      The number of neighbour points esed for distance value calculation.
     */
    SearchTreeFlannPCL( PointBufferPtr points,
            size_t &n_points,
            const size_t &kn = 10,
            const size_t &ki = 10,
            const size_t &kd = 10 );


    /**
     * @brief Destructor
     */
    virtual ~SearchTreeFlannPCL();


    /**
     * @brief This function performs a k-next-neightbour search on the
                     data that were given in the conrstructor.

     * @param qp          A float array which contains the query point for which the neighbours are searched.
     * @param neighbours  The number of neighbours that should be searched.
     * @param indices     A vector that stores the indices for the neighbours whithin the dataset.
     * @param distances   A vector that stores the distances for the neighbours that are found.
     */
    virtual void kSearch( coord < float >& qp, size_t neighbours, vector< size_t > &indices, vector< float > &distances );

    virtual void kSearch( VertexT qp, size_t k, vector< VertexT > &neighbors );

    virtual void radiusSearch( float              qp[3], float r, vector< size_t > &indices );
    virtual void radiusSearch( VertexT&              qp, float r, vector< size_t > &indices );
    virtual void radiusSearch( const VertexT&        qp, float r, vector< size_t > &indices );
    virtual void radiusSearch( coord< float >&       qp, float r, vector< size_t > &indices );
    virtual void radiusSearch( const coord< float >& qp, float r, vector< size_t > &indices );

protected:

    /// Store the pcl kd-tree
    pcl::PointCloud< pcl::PointXYZRGB >::Ptr       m_pointCloud;
    pcl::KdTreeFLANN< pcl::PointXYZRGB >::Ptr      m_kdTree;

}; // SearchTreePCL


} // namespace lvr
#include "SearchTreeFlannPCL.tcc"

#endif  // include-guard
