//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// LoaderStl.cpp
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

#include <stdio.h>
#include "TokenizerFile.hpp"
#include "LoaderStl.hpp"
#include "StrException.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

// reference
// https://en.wikipedia.org/wiki/STL_(file_format)

const char* LoaderStl::_ext = "stl";


bool LoaderStl::parseFacet(Tokenizer& tkn,
                           vector<float>& coord,
                           vector<int>& coordIndex,
                           vector<float>& normal) {
    // Check if current token is "facet"
    if (!tkn.equals("facet")) {
        return false;
    }

    // Get next token - should be "normal"
    if (!tkn.get() || !tkn.equals("normal")) {
        throw new StrException("Expected 'normal' after 'facet'");
    }

    float nx, ny, nz;
    if (!tkn.getFloat(nx)) throw new StrException("Failed to parse normal x");
    if (!tkn.getFloat(ny)) throw new StrException("Failed to parse normal y");
    if (!tkn.getFloat(nz)) throw new StrException("Failed to parse normal z");
    normal.push_back(nx);
    normal.push_back(ny);
    normal.push_back(nz);

    // Parse: outer loop
    if (!tkn.get() || !tkn.equals("outer")) {
        throw new StrException("Expected 'outer' after normal");
    }
    if (!tkn.get() || !tkn.equals("loop")) {
        throw new StrException("Expected 'loop' after 'outer'");
    }

    // Read three vertices
    int startVertexIndex = coord.size() / 3;
    for (int i = 0; i < 3; i++) {
        if (!tkn.get() || !tkn.equals("vertex")) {
            throw new StrException("Expected 'vertex' in loop");
        }

        float x, y, z;
        if (!tkn.getFloat(x)) throw new StrException("Failed to parse vertex x");
        if (!tkn.getFloat(y)) throw new StrException("Failed to parse vertex y");
        if (!tkn.getFloat(z)) throw new StrException("Failed to parse vertex z");

        coord.push_back(x);
        coord.push_back(y);
        coord.push_back(z);
        coordIndex.push_back(startVertexIndex + i);
    }

    // Add face separator
    coordIndex.push_back(-1);

    if (!tkn.get() || !tkn.equals("endloop")) {
        throw new StrException("Expected 'endloop'");
    }

    if (!tkn.get() || !tkn.equals("endfacet")) {
        throw new StrException("Expected 'endfacet'");
    }

    return true;
}


bool LoaderStl::load(const char* filename, SceneGraph& wrl) {
  bool success = false;

  // clear the scene graph
  wrl.clear();
  wrl.setUrl("");

  FILE* fp = (FILE*)0;
  try {

    // open the file
    if(filename==(char*)0) throw new StrException("filename==null");
    fp = fopen(filename,"r");
    if(fp==(FILE*)0) throw new StrException("fp==(FILE*)0");

    // use the io/Tokenizer class to parse the input ascii file

    TokenizerFile tkn(fp);
    // first token should be "solid"
    if(tkn.expecting("solid") && tkn.get()) {
      string stlName = tkn; // second token should be the solid name

      // TODO ...

      // create the scene graph structure :
      // 1) the SceneGraph should have a single Shape node a child
      // 2) the Shape node should have an Appearance node in its appearance field
      // 3) the Appearance node should have a Material node in its material field
      // 4) the Shape node should have an IndexedFaceSet node in its geometry node

      // from the IndexedFaceSet
      // 5) get references to the coordIndex, coord, and normal arrays
      // 6) set the normalPerVertex variable to false (i.e., normals per face)

      // the file should contain a list of triangles in the following format

      // facet normal ni nj nk
      //   outer loop
      //     vertex v1x v1y v1z
      //     vertex v2x v2y v2z
      //     vertex v3x v3y v3z
      //   endloop
      // endfacet

      // - run an infinite loop to parse all the faces
      // - write a private method to parse each face within the loop
      // - the method should return true if successful, and false if not
      // - if your method returns tru
      //     update the normal, coord, and coordIndex variables
      // - if your method returns false
      //     throw an StrException explaining why the method failed


      // create the scene graph structure:
      // 1)
      Shape* shape = new Shape();
      wrl.addChild(shape);

      // 2)
      Appearance* appearance = new Appearance();
      shape->setAppearance(appearance);

      // 3)
      Material* material = new Material();
      appearance->setMaterial(material);

      // 4)
      IndexedFaceSet* ifs = new IndexedFaceSet();
      shape->setGeometry(ifs);
      ifs->setName(stlName);

      // 5)
      vector<int>& coordIndex = ifs->getCoordIndex();
      vector<float>& coord = ifs->getCoord();
      vector<float>& normal = ifs->getNormal();

      // 6)
      ifs->setNormalPerVertex(false);

      while (tkn.get() && parseFacet(tkn, coord, coordIndex, normal)) {
      }

      // Current token should be "endsolid"
      // However, it seems that some test files don't follow this
      // such as plastic-buckle-ascii.stl
      if (!tkn.equals("endsolid")) {
          throw new StrException("Expected 'endsolid' at end of file");
      }

      success = true;

    }

    // close the file (this statement may not be reached)
    fclose(fp);
    
  } catch(StrException* e) {
    
    if(fp!=(FILE*)0) fclose(fp);
    fprintf(stderr,"ERROR | %s\n",e->what());
    delete e;

  }

  return success;
}
