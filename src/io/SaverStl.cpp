//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// SaverStl.cpp
//
// Written by: Ignas Karvelis
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2026, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Brown University nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "SaverStl.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

#include "core/Faces.hpp"
#include <string.h>
#include <stdio.h>

const char* SaverStl::_ext = "stl";

//////////////////////////////////////////////////////////////////////
bool SaverStl::save(const char* filename, SceneGraph& wrl) const {
  bool success = false;
  if(filename!=(char*)0) {

    // Check these conditions

    // 1) the SceneGraph should have a single child
    // 2) the child should be a Shape node
    // 3) the geometry of the Shape node should be an IndexedFaceSet node

    // - construct an instance of the Faces class from the IndexedFaceSet
    // - remember to delete it when you are done with it (if necessary)
    //   before returning

    // 4) the IndexedFaceSet should be a triangle mesh
    // 5) the IndexedFaceSet should have normals per face

    // if (all the conditions are satisfied) {


    // Check condition 1)
    if(wrl.getNumberOfChildren() != 1) {
        return false;
    }

    Node* child = wrl[0];
    if(child == (Node*)0) {
        return false;
    }

    // Check condition 2)
    if(!child->isShape()) {
        return false;
    }
    Shape* shape = (Shape*)child;

    // Check condition 3)
    Node* geom = shape->getGeometry();
    if(geom == (Node*)0 || !geom->isIndexedFaceSet()) {
        return false;
    }
    IndexedFaceSet* ifs = (IndexedFaceSet*)geom;

    // Get the data from IndexedFaceSet
    vector<float>& coord = ifs->getCoord();
    vector<int>& coordIndex = ifs->getCoordIndex();
    vector<float>& normal = ifs->getNormal();
    vector<int>& normalIndex = ifs->getNormalIndex();
    int nV = coord.size() / 3;
    Faces* faces = new Faces(nV, coordIndex);

    // Check condition 4)
    int nFaces = faces->getNumberOfFaces();
    for(int iF = 0; iF < nFaces; iF++) {
        if(faces->getFaceSize(iF) != 3) {
            delete faces;
            return false;
        }
    }

    // Check condition 5)
    if(ifs->getNormalPerVertex()) {
        delete faces;
        return false;
    }

    if(normal.size() == 0) {
        delete faces;
        return false;
    }

    int normalCount = 0;
    if(normalIndex.size() > 0) {
        for(int i = 0; i < (int)normalIndex.size(); i++) {
            if(normalIndex[i] != -1) {
                normalCount++;
            }
        }
    } else {
        normalCount = normal.size() / 3;
    }

    if(normalCount != nFaces) {
        delete faces;
        return false;
    }


    FILE* fp = fopen(filename,"w");
    if(	fp!=(FILE*)0) {

      // if set, use ifs->getName()
      // otherwise use filename,
      // but first remove directory and extension

      const char* solidName = "mesh";
      string ifsName = ifs->getName();
      if(ifsName.size() > 0) {
          solidName = ifsName.c_str();
      }

      fprintf(fp,"solid %s\n",filename);

      // TODO ...
      // for each face {
      //   ...
      // }

      // Write each face
      if(normalIndex.size() > 0) {
          // Normals are indexed
          int normalIdx = 0;
          for(int iF = 0; iF < nFaces; iF++) {
              while(normalIdx < (int)normalIndex.size() && normalIndex[normalIdx] == -1) {
                  normalIdx++;
              }

              if(normalIdx < (int)normalIndex.size()) {
                  int nIdx = normalIndex[normalIdx];
                  float nx = normal[3 * nIdx + 0];
                  float ny = normal[3 * nIdx + 1];
                  float nz = normal[3 * nIdx + 2];

                  fprintf(fp, "  facet normal %e %e %e\n", nx, ny, nz);
                  fprintf(fp, "    outer loop\n");

                  for(int j = 0; j < 3; j++) {
                      int vIdx = faces->getFaceVertex(iF, j);
                      float x = coord[3 * vIdx + 0];
                      float y = coord[3 * vIdx + 1];
                      float z = coord[3 * vIdx + 2];
                      fprintf(fp, "      vertex %e %e %e\n", x, y, z);
                  }

                  fprintf(fp, "    endloop\n");
                  fprintf(fp, "  endfacet\n");

                  normalIdx++;
              }
          }
      } else {
          // Normals are not indexed
          for(int iF = 0; iF < nFaces; iF++) {
              float nx = normal[3 * iF + 0];
              float ny = normal[3 * iF + 1];
              float nz = normal[3 * iF + 2];

              fprintf(fp, "  facet normal %e %e %e\n", nx, ny, nz);
              fprintf(fp, "    outer loop\n");

              for(int j = 0; j < 3; j++) {
                  int vIdx = faces->getFaceVertex(iF, j);
                  float x = coord[3 * vIdx + 0];
                  float y = coord[3 * vIdx + 1];
                  float z = coord[3 * vIdx + 2];
                  fprintf(fp, "      vertex %e %e %e\n", x, y, z);
              }

              fprintf(fp, "    endloop\n");
              fprintf(fp, "  endfacet\n");
          }
      }

      fprintf(fp, "endsolid %s\n", solidName);
      
      fclose(fp);
      success = true;
    }

    // } endif (all the conditions are satisfied)

  }
  return success;
}
