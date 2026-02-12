//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2026-01-26 17:42:17 taubin>
//------------------------------------------------------------------------
//
// Faces.cpp
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

#include <math.h>
#include "Faces.hpp"
  
Faces::Faces(const int nV, const vector<int>& coordIndex) {
    this->coordIndex = coordIndex;

    // Count faces and find max vertex index
    nFaces = 0;
    int maxVertexIndex = -1;

    for (int i = 0; i < coordIndex.size(); i++) {
        if (coordIndex[i] == -1) {
            nFaces++;
        } else if (coordIndex[i] > maxVertexIndex) {
            maxVertexIndex = coordIndex[i];
        }
    }

    // Set number of vertices and build faceFirstCorner
    nVertices = nV;
    if (maxVertexIndex + 1 > nVertices) {
        nVertices = maxVertexIndex + 1;
    }

    faceFirstCorner.resize(nFaces);

    int faceIdx = 0;

    if (!coordIndex.empty() && coordIndex[0] != -1) {
        faceFirstCorner[0] = 0;
        faceIdx = 1;
    }

    for (int i = 0; i < coordIndex.size() - 1; i++) {
        if (coordIndex[i] == -1 && coordIndex[i + 1] != -1) {
            faceFirstCorner[faceIdx] = i + 1;
            faceIdx++;
        }
    }

    // Build cornerToFace
    cornerToFace.resize(coordIndex.size());

    faceIdx = 0;

    for (int i = 0; i < coordIndex.size(); i++) {
        if (coordIndex[i] == -1) {
            cornerToFace[i] = -1;
            faceIdx++;
        } else {
            cornerToFace[i] = faceIdx;
        }
    }
}

int Faces::getNumberOfVertices() const {
    return nVertices;
}

int Faces::getNumberOfFaces() const {
    return nFaces;
}

int Faces::getNumberOfCorners() const {
    return coordIndex.size();
}

int Faces::getFaceSize(const int iF) const {
    if (iF < 0 || iF >= nFaces) {
        return 0;
    }

    int start = faceFirstCorner[iF];
    int count = 0;

    for (int i = start; i < coordIndex.size() && coordIndex[i] != -1; i++) {
        count++;
    }

    return count;
}

int Faces::getFaceFirstCorner(const int iF) const {
    if (iF < 0 || iF >= nFaces) {
        return -1;
    }
    return faceFirstCorner[iF];
}


int Faces::getFaceVertex(const int iF, const int j) const {
    if (iF < 0 || iF >= nFaces || j < 0 || j >= getFaceSize(iF)) {
        return -1;
    }

    int cornerIdx = faceFirstCorner[iF] + j;
    return coordIndex[cornerIdx];
}

int Faces::getCornerFace(const int iC) const {
    if (iC < 0 || iC >= coordIndex.size()) {
        return -1;
    }

    return cornerToFace[iC];
}

int Faces::getNextCorner(const int iC) const {
    if (iC < 0 || iC >= coordIndex.size() || coordIndex[iC] == -1) {
        return -1;
    }

    // Find the next corner
    int nextIdx = iC + 1;

    if (nextIdx >= coordIndex.size() || coordIndex[nextIdx] == -1) {
        int faceIdx = cornerToFace[iC];
        if (faceIdx >= 0 && faceIdx < nFaces) {
            return faceFirstCorner[faceIdx];
        }
        return -1;
    }

    return nextIdx;
}
