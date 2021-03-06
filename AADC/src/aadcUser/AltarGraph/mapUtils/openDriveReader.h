/*********************************************************************
Copyright (c) 2018
Audi Autonomous Driving Cup. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3.  All advertising materials mentioning features or use of this software must display the following acknowledgement: ?This product includes software developed by the Audi AG and its contributors for Audi Autonomous Driving Cup.?
4.  Neither the name of Audi nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AUDI AG AND CONTRIBUTORS AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AUDI AG OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

**********************************************************************/


/*********************************************************************
* This code was provided by HERE
*
* *******************************************************************/

#ifndef _OPEN_DRIVE_PLANNER_
#define _OPEN_DRIVE_PLANNER_

#include <vector>
#include <string>
#include <map>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <adtf_filtersdk.h>
//always include filtersdk, systemsdk or streaming3 sdk BEFORE adtfui!!

#include "tinyxml2.h"
#include <locale>


#define BIDIRECTIONAL false
#define SINGLELANE false
#define ALTITUDE 0.0
#define MAP_SCALE 0.125
#define LANEWIDTH 0.23
#define NUM 3

enum turn {
    LEFT,
    STRAIGHT,
    RIGHT
};


struct node {
    int id;
    int uniqueId;
    const char* contactPoint;
    turn type;
};

// MY STRUCTRS
struct mapPoint {
    tFloat32 x;
    tFloat32 y;
};

struct nodeId{
    int roadId;
    int uniqueId;
    bool const operator <(const nodeId& o) const {
        return roadId < o.roadId || (roadId == o.roadId && uniqueId < o.uniqueId);
    }
};

enum contact {
    START, //0
    END, //num - 1
};

struct graphElement {
    std::vector<nodeId> succId;
    std::vector<turn> succType;
    mapPoint p;
};


struct graphNode {
    float g;
    float f;
    float rhs;
    int key;
    double distanceToMid = 0;
    std::vector<nodeId>* succ = new std::vector<nodeId>;
    std::vector<nodeId>* pred = new std::vector<nodeId>;
    mapPoint p;
    double cost = 0;
};

using namespace tinyxml2;

    struct Quaternion {
        float x;
        float y;
        float z;
        float w;
    };
    struct Euler {
        float pitch;
        float roll;
        float yaw;
    };
    struct Point {
        float x;
        float y;
        float z;
    };
    struct Pose3D {
        Point p;
        Quaternion q;
    };
    struct roadGeometryWrapper {
        float x, y, hdg, s, length;
        float aU, aV, bU, bV, cU, cV, dU, dV;
    };
    struct lane {
        int id;
        const char *type, *direction;
    };

    struct conection {
        // x - y cordinates are relative to the COM of ca
        int uniqueId;
        contact START;

        // encode negative if you need to connect to 10 inside of 0
    };

    struct roadElementWrapper {
        int id, junction;
        std::vector<roadGeometryWrapper>  geometry;
        std::vector<lane>  lanes;
        std::vector<conection>  nodes;
        const char * predCP, *succCP, *predET, *succET;
        int predId, succId;
        int nextRoad;
        float cost;
        float altitude;
        float scale;
        float laneSplit;
        bool reverse = false;
        int vectId = 0;
        int uniqueId = 0;
    
        // TODO INitalize vectors
        std::vector<nodeId> startNodes;
        std::vector<nodeId> endNodes;
    
    
        // For GraphSearch
    
        std::vector<int> succIds;
        std::vector<turn> succType;
    
        graphElement startElement;
    };
    
    
    struct roadElement {
        int id, junction;
        std::vector<roadGeometryWrapper>  geometry;
        std::vector<lane>  lanes;
        std::vector<conection>  nodes;
        const char * predCP, *succCP, *predET, *succET;
        int predId, succId;
        int nextRoad;
        float cost;
        float altitude;
        float scale;
        float laneSplit;
        bool reverse = false;
        int vectId = 0;
        int uniqueId = 0;

        // TODO INitalize vectors
        std::vector<nodeId> startNodes;
        std::vector<nodeId> endNodes;


        // For GraphSearch

        std::vector<int> succIds;
        std::vector<turn> succType;

        graphElement startElement;
    };

    struct MapElement {
        Pose3D pose;
        int roadId;
        std::vector<int> nodes;
        const char * predCP, *succCP, *predET, *succET;
        int predId, succId;
    };




    class openDriveReader {
    public:

        openDriveReader();
        ~openDriveReader();
        openDriveReader(std::string file);
        tinyxml2::XMLDocument Doc;
        XMLError FileReadErr;
        std::vector<roadElementWrapper> RoadListOrg, RoadList;
        bool BiDirectional, SingleLane;
        float Altitude, Scale, LaneWidth;
        std::vector<Pose3D> MapPoints;
        std::vector<MapElement> MapElList;
        //std::vector<PathElement> PathElList;
        //std::vector<Conection> Conections;


        nodeId startNodeId;
        nodeId endNodeId;

        std::map<int, int> succ_map;

        std::map<nodeId, graphElement> graph;

        std::map<int, roadElementWrapper> roads;

        std::map<nodeId, graphNode> lattice;
        std::map<nodeId, std::vector<int>> endpoints;

        std::vector<std::vector<graphNode>> pathGraph;

        const char* FileName;
        void LoadMap(std::string file, int num = 10);
        void ReadFile(int num = 10);
        std::vector<Pose3D> GetRoadPoints(roadElementWrapper el, int num = 10);
        void LatticePoints(std::map<int, roadElementWrapper> roadNetwork, std::vector<int> segmentId, float lateral_density, float longitudinal_density);

    private:

        void MergeLanes();
        void SplitLanes();
        void GetNodes();
        Quaternion toQuaternion(double pitch, double roll, double yaw);
        Euler toEulerianAngle(Quaternion q);
        float CubicPoly(float a1, float b1, float c1, float d1, float ds);
        float RotateCCWX(float u2, float v2, float hdg2);
        float RotateCCWY(float u1, float v1, float hdg1);
        float EuclideanDistance(Pose3D pose1, Pose3D pose2);
        std::vector<Pose3D> UpdatePoseHeading(std::vector<Pose3D> path);
        std::vector<Pose3D> GetRoadPoints(std::vector<roadElementWrapper> el, int num = 10);
        std::vector<MapElement> GetMapPoints(std::map<int, roadElementWrapper> roads, int num = 10);
        tFloat32 normalizeAngle(tFloat32 alpha, tFloat32 center);
        tFloat32 mod(tFloat32 x, tFloat32 y);




    };

#endif
