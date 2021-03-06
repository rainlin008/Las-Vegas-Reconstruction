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


 /**
 * @mainpage LSSR Toolkit Documentation
 *
 * @section Intro Introduction
 *
 * This software delivers tools to build surface reconstructions from
 * point cloud data and a simple viewer to display the results.
 * Additionally, the found surfaces can be classified into
 * categories in terms of floor, ceiling walls etc.. The main aim of this
 * project is to deliver fast and accurate surface extraction algorithms
 * for robotic applications such as teleoperation in unknown environments
 * and localization.
 *
 * LSSR is under permanent development and runs under Linux and MacOS.
 * A Windows version will be made available soon. The software is currently
 * under heavy reorganization, so it may happen that some interfaces change.
 * Additionally, not all features have been ported to the new structure,
 * so some functionalities may not be available at all times.
 *
 * In the current version the previously available plane clustering and
 * classification algorithms are not available. Please use the previous
 * release (0.1) if your are interested in these functionalities. The
 * missing features will be re-integrated in the next release.
 *
 * @section Compiling Compiling the software
 *
 * This software uses cmake \ref [http://www.cmake.org]. The easiest way
 * to build LSSR is to perform an out of source build:
 * \verbatim
 * mkdir build
 * cd build
 * cmake .. && make
 * cd ../bin
 * \endverbatim
 *
 * External library dependencies:
 *
 * <ul>
 * <li>OpenGL</li>
 * <li>OpenGL Utility Toolkit (glut)</li>
 * <li>OpenGL Utility Library (glu)</li>
 * <li>OpenMP</li>
 * <li>Boost
 *   <ul>
 *     <li>Thread</li>
 *     <li>Filesystem</li>
 *     <li>Program Options</li>
 *     <li>System</li>
 *   </ul>
 * </li>
 * <li>Qt 4.7 or above (for viewer and qviewer)</li>
 * <li>libQGLViewer 2.3.9 or newer (for qviewer)</li>
 * <li>X.Org X11 libXi runtime library</li>
 * <li>X.Org X11 libXmu/libXmuu runtime libraries</li>
 * </ul>
 *
 *
 * @section Usage Software Usage
 *
 * LSSR comes with a tool to reconstruct surfaces from unorganized
 * points and two viewer applications. To build a surface from the
 * provided example data set, just call from the program directory
 *
 * \verbatim
 * bin/reconstruct dat/points.pts -v5
 * \endverbatim
 *
 * This command will produce a triangle mesh of the data points stored in
 * a file called "triangle_mesh.ply". The data set and the reconstruction
 * can be displayed with one of the provided viewers. Important
 * parameters for "reconstruct" are
 *
 * <table border="0">
 * <tr>
 * <td width = 10%>
 * --help
 * </td>
 * <td>
 * Prints a short description of all relevant parameters.
 * </td>
 * </tr>
 * <tr>
 * <td>-v or -i</td>
* <td>
* <p>These parameters affect the accuracy of the reconstruction.
* <i>-i</i> defines the number of intersections on the longest side
* of the scanned scene and determines the corresponding voxelsize.
* Using this parameter is useful if the scaling of a scene is
* unknown. A value of about 100 will usually generate coarse surface.
* Experiment with this value to get a tradeoff between accuracy and
* mesh size. If you know the scaling of the objects, you can set a
* fixed voxelsize by using the <i>-v</i> parameter.
* </p>
* </td>
* </tr>
* <tr>
* <td>--ki, --kn, --kd</td>
* <td>These parameters determine the number of nearest neighbors used
* for initial normal estimation (<i>--kn</i>), normal interpolation
* (<i>--ki</i>) and distance value evaluation (<i>--kd</i>). In data
* sets with a lot of noise, increasing these values can lead to better
* approximations at the cost of running time. Increasing <i>--kd</i>
* usually helps to generate more continuous surfaces in sparse
* scans, but yields in a lot of smoothing, i.e. in the
* reconstuctions, sharp features will be smoothed out.</td>
* </tr>
* </table>
*
* @section API API Description
*
* A detailed API documentation will be made available soon.
*
* @section Tutorials Tutorials
*
* A set of tutorials how to use LSSR will be made available soon.
*/


// Program options for this tool

#ifndef DEBUG
  #include "Options.hpp"
#endif

// Local includes
#include <lvr/reconstruction/AdaptiveKSearchSurface.hpp>
#include <lvr/reconstruction/FastReconstruction.hpp>
#include <lvr/reconstruction/PointsetGrid.hpp>
#include <lvr/reconstruction/FastBox.hpp>
#include <lvr/reconstruction/SharpBox.hpp>
#include <lvr/reconstruction/TetraederBox.hpp>

#include <lvr/io/PLYIO.hpp>
#include <lvr/config/lvropenmp.hpp>
#include <lvr/geometry/Matrix4.hpp>
#include <lvr/geometry/HalfEdgeMesh.hpp>
#include <lvr/texture/Texture.hpp>
#include <lvr/texture/Transform.hpp>
#include <lvr/texture/Texturizer.hpp>
#include <lvr/texture/Statistics.hpp>
#include <lvr/geometry/QuadricVertexCosts.hpp>


// PCL related includes
#ifdef LVR_USE_PCL
#include <lvr/reconstruction/PCLKSurface.hpp>
#endif


#include <iostream>


using namespace lvr;

typedef ColorVertex<float, unsigned char> cVertex;
typedef Normal<float> cNormal;
typedef PointsetSurface<ColorVertex<float, unsigned char> > psSurface;
typedef AdaptiveKSearchSurface<ColorVertex<float, unsigned char>, Normal<float> > akSurface;

#ifdef LVR_USE_PCL
typedef PCLKSurface<ColorVertex<float, unsigned char> , Normal<float> > pclSurface;
#endif

/**
 * @brief   Main entry point for the LSSR surface executable
 */
int main(int argc, char** argv)
{
	try
	{
		// Parse command line arguments
		reconstruct::Options options(argc, argv);

		// Exit if options had to generate a usage message
		// (this means required parameters are missing)
		if ( options.printUsage() )
		{
			return 0;
		}

		OpenMPConfig::setNumThreads(options.getNumThreads());

		std::cout << options << std::endl;

		// Create a point loader object
		ModelPtr model = ModelFactory::readModel( options.getInputFileName() );
		PointBufferPtr p_loader;

		// Parse loaded data
		if ( !model )
		{
			cout << timestamp << "IO Error: Unable to parse " << options.getInputFileName() << endl;
			exit(-1);
		}
		p_loader = model->m_pointCloud;

		// Create a point cloud manager
		string pcm_name = options.getPCM();
		psSurface::Ptr surface;

		// Create point set surface object
		if(pcm_name == "PCL")
		{
#ifdef LVR_USE_PCL
			surface = psSurface::Ptr( new pclSurface(p_loader));
#else 
			cout << timestamp << "Can't create a PCL point set surface without PCL installed." << endl;
			exit(-1);
#endif
		}
		else if(pcm_name == "STANN" || pcm_name == "FLANN" || pcm_name == "NABO" || pcm_name == "NANOFLANN")
		{
			akSurface* aks = new akSurface(
					p_loader, pcm_name,
					options.getKn(),
					options.getKi(),
					options.getKd(),
					options.useRansac(),
					options.getScanPoseFile()
			);

			surface = psSurface::Ptr(aks);
			// Set RANSAC flag
			if(options.useRansac())
			{
				aks->useRansac(true);
			}
		}
		else
		{
			cout << timestamp << "Unable to create PointCloudManager." << endl;
			cout << timestamp << "Unknown option '" << pcm_name << "'." << endl;
			cout << timestamp << "Available PCMs are: " << endl;
			cout << timestamp << "STANN, STANN_RANSAC";
#ifdef LVR_USE_PCL
			cout << ", PCL";
#endif
#ifdef LVR_USE_NABO
			cout << ", Nabo";
#endif
			cout << endl;
			return 0;
		}

		// Set search options for normal estimation and distance evaluation
		surface->setKd(options.getKd());
		surface->setKi(options.getKi());
		surface->setKn(options.getKn());

		// Calculate normals if necessary
		if(!surface->pointBuffer()->hasPointNormals()
				|| (surface->pointBuffer()->hasPointNormals() && options.recalcNormals()))
		{
		    Timestamp ts;
			surface->calculateSurfaceNormals();
		}
		else
		{
			cout << timestamp << "Using given normals." << endl;
		}

		// Save points and normals only
		if(options.savePointNormals())
		{
			ModelPtr pn( new Model);
			pn->m_pointCloud = surface->pointBuffer();
			ModelFactory::saveModel(pn, "pointnormals.ply");
		}

		// Create an empty mesh
		HalfEdgeMesh<ColorVertex<float, unsigned char> , Normal<float> > mesh( surface );

		// Set recursion depth for region growing
		if(options.getDepth())
		{
			mesh.setDepth(options.getDepth());
		}

		if(options.getTexelSize())
		{
			Texture::m_texelSize = options.getTexelSize();
		}
		
		if(options.getTexturePack() != "")
		{
			Texturizer<Vertex<float> , Normal<float> >::m_filename = options.getTexturePack();
			if(options.getStatsCoeffs())
			{	
				float* sc = options.getStatsCoeffs();
				for (int i = 0; i < 14; i++)
				{
					Statistics::m_coeffs[i] = sc[i];
				}
				delete sc;
			}
			if(options.getNumStatsColors())
			{
				Texturizer<Vertex<float> , Normal<float> >::m_numStatsColors = options.getNumStatsColors();
			}
			if(options.getNumCCVColors())
			{
				Texturizer<Vertex<float> , Normal<float> >::m_numCCVColors = options.getNumCCVColors();
            }
			if(options.getCoherenceThreshold())
			{
				Texturizer<Vertex<float> , Normal<float> >::m_coherenceThreshold = options.getCoherenceThreshold();
			}

			if(options.getColorThreshold())
			{
				Texturizer<Vertex<float> , Normal<float> >::m_colorThreshold = options.getColorThreshold();
			}
			if(options.getStatsThreshold())
			{
				Texturizer<Vertex<float> , Normal<float> >::m_statsThreshold = options.getStatsThreshold();
			}
			if(options.getUseCrossCorr())
			{
				Texturizer<Vertex<float> , Normal<float> >::m_useCrossCorr = options.getUseCrossCorr();
			}
			if(options.getFeatureThreshold())
			{
				Texturizer<Vertex<float> , Normal<float> >::m_featureThreshold = options.getFeatureThreshold();
            }
			if(options.getPatternThreshold())
			{
				Texturizer<Vertex<float> , Normal<float> >::m_patternThreshold = options.getPatternThreshold();
			}
			if(options.doTextureAnalysis())
			{
			    Texturizer<Vertex<float> , Normal<float> >::m_doAnalysis = true;
			}
			if(options.getMinimumTransformationVotes())
			{
				Transform::m_minimumVotes = options.getMinimumTransformationVotes();
			}
		}

		if(options.getSharpFeatureThreshold())
		{
			SharpBox<Vertex<float> , Normal<float> >::m_theta_sharp = options.getSharpFeatureThreshold();
		}
		if(options.getSharpCornerThreshold())
		{
			SharpBox<Vertex<float> , Normal<float> >::m_phi_corner = options.getSharpCornerThreshold();
		}

		// Determine whether to use intersections or voxelsize
		float resolution;
		bool useVoxelsize;
		if(options.getIntersections() > 0)
		{
			resolution = options.getIntersections();
			useVoxelsize = false;
		}
		else
		{
			resolution = options.getVoxelsize();
			useVoxelsize = true;
		}

		// Create a point set grid for reconstruction
		string decomposition = options.getDecomposition();

		// Fail safe check
        if(decomposition != "MT" && decomposition != "MC" && decomposition != "PMC" && decomposition != "SF" )
		{
			cout << "Unsupported decomposition type " << decomposition << ". Defaulting to PMC." << endl;
			decomposition = "PMC";
		}

		GridBase* grid;
		FastReconstructionBase<ColorVertex<float, unsigned char>, Normal<float> >* reconstruction;
		if(decomposition == "MC")
		{
			grid = new PointsetGrid<ColorVertex<float, unsigned char>, FastBox<ColorVertex<float, unsigned char>, Normal<float> > >(resolution, surface, surface->getBoundingBox(), useVoxelsize, options.extrude());
			PointsetGrid<ColorVertex<float, unsigned char>, FastBox<ColorVertex<float, unsigned char>, Normal<float> > >* ps_grid = static_cast<PointsetGrid<ColorVertex<float, unsigned char>, FastBox<ColorVertex<float, unsigned char>, Normal<float> > > *>(grid);
			ps_grid->calcDistanceValues();
			reconstruction = new FastReconstruction<ColorVertex<float, unsigned char> , Normal<float>, FastBox<ColorVertex<float, unsigned char>, Normal<float> >  >(ps_grid);

		}
		else if(decomposition == "PMC")
		{
			BilinearFastBox<ColorVertex<float, unsigned char>, Normal<float> >::m_surface = surface;
			grid = new PointsetGrid<ColorVertex<float, unsigned char>, BilinearFastBox<ColorVertex<float, unsigned char>, Normal<float> > >(resolution, surface, surface->getBoundingBox(), useVoxelsize, options.extrude());
			PointsetGrid<ColorVertex<float, unsigned char>, BilinearFastBox<ColorVertex<float, unsigned char>, Normal<float> > >* ps_grid = static_cast<PointsetGrid<ColorVertex<float, unsigned char>, BilinearFastBox<ColorVertex<float, unsigned char>, Normal<float> > > *>(grid);
			ps_grid->calcDistanceValues();
			reconstruction = new FastReconstruction<ColorVertex<float, unsigned char> , Normal<float>, BilinearFastBox<ColorVertex<float, unsigned char>, Normal<float> >  >(ps_grid);

		}
		else if(decomposition == "SF")
		{
			SharpBox<ColorVertex<float, unsigned char>, Normal<float> >::m_surface = surface;
			grid = new PointsetGrid<ColorVertex<float, unsigned char>, SharpBox<ColorVertex<float, unsigned char>, Normal<float> > >(resolution, surface, surface->getBoundingBox(), useVoxelsize, options.extrude());
			PointsetGrid<ColorVertex<float, unsigned char>, SharpBox<ColorVertex<float, unsigned char>, Normal<float> > >* ps_grid = static_cast<PointsetGrid<ColorVertex<float, unsigned char>, SharpBox<ColorVertex<float, unsigned char>, Normal<float> > > *>(grid);
			ps_grid->calcDistanceValues();
			reconstruction = new FastReconstruction<ColorVertex<float, unsigned char> , Normal<float>, SharpBox<ColorVertex<float, unsigned char>, Normal<float> >  >(ps_grid);
		}
        else if(decomposition == "MT")
        {
            grid = new PointsetGrid<ColorVertex<float, unsigned char>, TetraederBox<ColorVertex<float, unsigned char>, Normal<float> > >(resolution, surface, surface->getBoundingBox(), useVoxelsize, options.extrude());
            PointsetGrid<ColorVertex<float, unsigned char>, TetraederBox<ColorVertex<float, unsigned char>, Normal<float> > >* ps_grid = static_cast<PointsetGrid<ColorVertex<float, unsigned char>, TetraederBox<ColorVertex<float, unsigned char>, Normal<float> > > *>(grid);
            ps_grid->calcDistanceValues();
            reconstruction = new FastReconstruction<ColorVertex<float, unsigned char> , Normal<float>, TetraederBox<ColorVertex<float, unsigned char>, Normal<float> >  >(ps_grid);
        }


		
		// Create mesh
		reconstruction->getMesh(mesh);
		
		// Save grid to file
		if(options.saveGrid())
		{
			grid->saveGrid("fastgrid.grid");
		}

		if(options.getDanglingArtifacts())
 		{
			mesh.removeDanglingArtifacts(options.getDanglingArtifacts());
		}

		// Optimize mesh
		mesh.cleanContours(options.getCleanContourIterations());
		mesh.setClassifier(options.getClassifier());
		mesh.getClassifier().setMinRegionSize(options.getSmallRegionThreshold());

		if(options.optimizePlanes())
		{
			mesh.optimizePlanes(options.getPlaneIterations(),
					options.getNormalThreshold(),
					options.getMinPlaneSize(),
					options.getSmallRegionThreshold(),
					true);

			mesh.fillHoles(options.getFillHoles());
			mesh.optimizePlaneIntersections();
			mesh.restorePlanes(options.getMinPlaneSize());

			if(options.getNumEdgeCollapses())
			{
				QuadricVertexCosts<ColorVertex<float, unsigned char> , Normal<float> > c = QuadricVertexCosts<ColorVertex<float, unsigned char> , Normal<float> >(true);
				mesh.reduceMeshByCollapse(options.getNumEdgeCollapses(), c);
			}
		}
		else if(options.clusterPlanes())
		{
			mesh.clusterRegions(options.getNormalThreshold(), options.getMinPlaneSize());
			mesh.fillHoles(options.getFillHoles());
		}

		// Save triangle mesh
		if ( options.retesselate() )
		{
			mesh.finalizeAndRetesselate(options.generateTextures(), options.getLineFusionThreshold());
		}
		else
		{
			mesh.finalize();
		}

		// Write classification to file
		if ( options.writeClassificationResult() )
		{
			mesh.writeClassificationResult();
		}

		// Create output model and save to file
		ModelPtr m( new Model( mesh.meshBuffer() ) );

		if(options.saveOriginalData())
		{
			m->m_pointCloud = model->m_pointCloud;
		}
		cout << timestamp << "Saving mesh." << endl;
		ModelFactory::saveModel( m, "triangle_mesh.ply");

		// Save obj model if textures were generated
		if(options.generateTextures())
		{
			ModelFactory::saveModel( m, "triangle_mesh.obj");
		}		
		cout << timestamp << "Program end." << endl;

	}
	catch(...)
	{
		std::cout << "Unable to parse options. Call 'reconstruct --help' for more information." << std::endl;
	}
	return 0;
}

