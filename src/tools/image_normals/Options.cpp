/**
 * Copyright (C) 2013 Universität Osnabrück
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



#include "Options.hpp"

namespace image_normals
{

Options::Options(int argc, char** argv) : m_descr("Supported options")
{

	// Create option descriptions

	m_descr.add_options()
	("help", "Produce help message")
    ("inputFile",       value< vector<string> >(), "Input file name. If input is an directory the programm will check for scans in SLAM6D uos format.")
    ("outputFile,o",    value<string>(&m_outputFile)->default_value("normals.ply"), "Output file name.")
    ("minH",            value<int>(&m_minH)->default_value(0),          "Minimal horizontal opening angle.")
    ("maxH",            value<int>(&m_maxH)->default_value(360),        "Maximal horizontal opening angle.")
    ("minV",            value<int>(&m_minV)->default_value(-90),        "Minimal vertical opening angle.")
    ("maxV",            value<int>(&m_maxV)->default_value(90),         "Maximal vertical opening angle.")
    ("minZ",            value<float>(&m_minZ)->default_value(0),        "Minimal depth value.")
    ("maxZ",            value<float>(&m_maxZ)->default_value(1e6),      "Maximal depth value.")
    ("maxZimg",         value<float>(&m_maxZimg)->default_value(1e6),   "Maximal depth value in depth image.")
    ("minZimg",         value<float>(&m_maxZimg)->default_value(0),     "Maximal depth value in depth image.")
    ("img",             value<string>(&m_imageOut)->default_value("panorama.pgm"), "Output file for projection image.")
    ("imageWidth,w",    value<int>(&m_width)->default_value(2800),      "Image width.")
    ("imageHeight,h",   value<int>(&m_height)->default_value(1000),     "Image height.")
    ("regionWidth,i",    value<int>(&m_width)->default_value(5),      "Width of the nearest neighbor region of a pixel for normal estimation.")
    ("regionHeight,j",   value<int>(&m_height)->default_value(5),     "Height of the nearest neighbor region of a pixel for normal estimation.")
    ("optimizeRatio,r",      "Optimize image aspect ratio.")
    ("system,s",        value<string>(&m_system)->default_value("NATIVE"), "The coordinate system in which the points are stored. Use NATIVE to interpret the points as they are. Use SLAM6D for scans in 3dtk's coordinate system and UOS for scans that where taken with a tilting laser scanner at Osnabrueck University.")
	;

    m_pdescr.add("inputFile", -1);

	// Parse command line and generate variables map
	store(command_line_parser(argc, argv).options(m_descr).positional(m_pdescr).run(), m_variables);
	notify(m_variables);

	if(m_variables.count("help"))
	{
		::std::cout << m_descr << ::std::endl;
        exit(-1);
	}

}



Options::~Options()
{
	// TODO Auto-generated destructor stub
}

}

