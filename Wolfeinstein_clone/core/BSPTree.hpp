
#ifndef BSP_TREE_HPP
#define BSP_TREE_HPP
/*

#include <vector>
#include <cfloat>
#include <cstdlib>

#include "../CL/GeometryData.hpp"

#include "../RAL/UsingOpenGL.hpp"

#include "../RL/Renderer.hpp"

struct Polygon {
	Polygon(int numVertices, std::vector<Vec3>& vertices) : numVertices(numVertices), vertices(vertices) {}
	int numVertices;
	std::vector<Vec3> vertices;
};


//	Taken almost line for line from Christer Ericson's book "Real-time Collision Detection"


struct Plane3D {
	Plane3D() {}
	Plane3D(Vec3 n, float d) : n(n), d(d) {}
	Plane3D(Vec3 A, Vec3 B, Vec3 C){
		Vec3 u1 = B - A;
		Vec3 u2 = C - A;
		n = cross(u2, u1).normalize();
		d = dot(n, A);
	}
	Vec3 n;
	float d;
};

#define COPLANAR_WITH_PLANE 0
#define IN_FRONT_OF_PLANE   1
#define BEHIND_PLANE        2
#define STRADDLING_PLANE    3

#define PLANE_THICKNESS_EPSILON 0.001f

#define MIN_POLYGON_LIMIT 5
#define MAX_DEPTH_LIMIT	  100

struct BSPNode{
	BSPNode(const std::vector<TrianglePolygon*>& polygons) : polygons(polygons) {
		generateGeometry();
		color = Vec3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
								 static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
								 static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
		//color = Vec3(1,0,1);
	}
	BSPNode(Plane3D& plane, BSPNode* front, BSPNode* back) : plane(plane), front(front), back(back) {}
	
	std::vector<TrianglePolygon*> polygons;
	
	Plane3D plane;
	
	BSPNode* front;
	BSPNode* back;

	GLuint vao;
	GLuint vbo;
	int numVertices = 0;

	Vec3 color;

	bool hasGeometry = false;
	void generateGeometry(){
		hasGeometry = true;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//std::cout << "polygons = " << polygons.size() << std::endl;

		float vertices[polygons.size() * 9];
		int index = 0;
		for(int i = 0; i < polygons.size(); ++i){
			vertices[i*9+0] = polygons[i]->operator[](2).x;
			vertices[i*9+1] = polygons[i]->operator[](2).y;
			vertices[i*9+2] = polygons[i]->operator[](2).z;
			vertices[i*9+3] = polygons[i]->operator[](1).x;
			vertices[i*9+4] = polygons[i]->operator[](1).y;
			vertices[i*9+5] = polygons[i]->operator[](1).z;
			vertices[i*9+6] = polygons[i]->operator[](0).x;
			vertices[i*9+7] = polygons[i]->operator[](0).y;
			vertices[i*9+8] = polygons[i]->operator[](0).z;
			numVertices += 3;
		}

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);
		
		glBindVertexArray(0);
	}
	void render(){
		// Set color
		//glBindBuffer(GL_UNIFORM_BUFFER, Renderer::uboColor);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(float)*4, new Vec4(color.x, color.y, color.z, 1.0f), GL_DYNAMIC_DRAW);
		
		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLES, 0, numVertices);
		
		glBindVertexArray(0);
	}
};

class BSPTree {
public:
	BSPNode* root;

	void build(const std::vector<TrianglePolygon*>& polygons){
		root = build_impl(polygons, 0);
	}
private:
	BSPNode* build_impl(const std::vector<TrianglePolygon*>& polygons, int depth){
		// No more polygons
		if(!polygons.size())
			return nullptr; // return null tree

		// Criterions to stop recursion
		int numPolygons = polygons.size();
		if(numPolygons <= MIN_POLYGON_LIMIT || depth >= MAX_DEPTH_LIMIT)
			return new BSPNode(polygons);

		// Select best partitioning plane
		Plane3D splitingPlane = selectPartitioningPlane(polygons);

		// Test each polygon against the spliting plane
		std::vector<TrianglePolygon*> backPolygons, frontPolygons;
		for(int i = 0; i < numPolygons; ++i){
			switch(testPolygonWithPlane(polygons[i], splitingPlane)){
			case COPLANAR_WITH_PLANE:
				// Treated as in front of the plane
			case IN_FRONT_OF_PLANE:
				frontPolygons.push_back(polygons[i]);
				break;
			case BEHIND_PLANE:
				backPolygons.push_back(polygons[i]);
				break;
			case STRADDLING_PLANE:
				Polygon* frontPart,* backPart;
				splitPolygon(polygons[i], splitingPlane, frontPart, backPart);
				// Triangulate polygon quad
				assert(frontPart->numVertices <= 4);
				assert(backPart->numVertices <= 4);
				if(frontPart->numVertices == 4){
					// Triangulate front quad
					TrianglePolygon* a,* b;
					a = new TrianglePolygon(frontPart->vertices[0], frontPart->vertices[1], frontPart->vertices[2]);
					b = new TrianglePolygon(frontPart->vertices[2], frontPart->vertices[3], frontPart->vertices[0]);
					frontPolygons.push_back(a);
					frontPolygons.push_back(b);
				}else
					frontPolygons.push_back(new TrianglePolygon(frontPart->vertices[0], frontPart->vertices[1], frontPart->vertices[2]));
				if(backPart->numVertices == 4){
					// Triangulate back quad
					TrianglePolygon* a,* b;
					a = new TrianglePolygon(backPart->vertices[0], backPart->vertices[1], backPart->vertices[2]);
					b = new TrianglePolygon(backPart->vertices[2], backPart->vertices[3], backPart->vertices[0]);
					backPolygons.push_back(a);
					backPolygons.push_back(b);
				}else
					backPolygons.push_back(new TrianglePolygon(backPart->vertices[0], backPart->vertices[1], backPart->vertices[2]));
				break;
			}
		}

		// Recursively build nodes
		std::cout << "depth = " << depth << std::endl;
		std::cout << "front = " << frontPolygons.size() << std::endl;
		std::cout << "back = " << backPolygons.size() << std::endl;
		BSPNode* frontTree = build_impl(frontPolygons, depth+1);
		BSPNode* backTree = build_impl(backPolygons, depth+1);
		return new BSPNode(splitingPlane, frontTree, backTree);
	}

	Plane3D selectPartitioningPlane(const std::vector<TrianglePolygon*> polygons){
		// Blend factor constant
		const float K = 0.8f;
		// Caching of current best plane
		Plane3D bestPlane;
		float bestScore = FLT_MAX;
		
		// Auto-partitioning
		for(int i = 0; i < polygons.size(); ++i){
			int numFront = 0, numBehind = 0, numStraddling = 0;
			Plane3D plane = Plane3D(polygons[i]->operator[](0), polygons[i]->operator[](1), polygons[i]->operator[](2));
			// Test against each other polygon
			for(int j = 0; j < polygons.size(); ++j){
				// Ignore against itself
				if(i == j) continue;
				// Keep count of various polygon - plane relationships
				switch(testPolygonWithPlane(polygons[j], plane)){
				case COPLANAR_WITH_PLANE:
					// Treated as in front of the plane
				case IN_FRONT_OF_PLANE:
					++numFront;
					break;
				case BEHIND_PLANE:
					++numBehind;
					break;
				case STRADDLING_PLANE:
					++numStraddling;
					break;
				}
			}

			// Compute score of plane
			float score = K * numStraddling + (1.0f-K) * std::abs(numFront - numBehind);
			if(score < bestScore){
				bestScore = score;
				bestPlane = plane;
			}
		}
		
		return bestPlane;
	}

	Plane getPlaneFromPolygon(TrianglePolygon* polygon){
		// TODO
	}
public:
	static int testPointWithPlane(Vec3 point, Plane3D plane){
		float dist = dot(plane.n, point) - plane.d;
		
		if(dist > PLANE_THICKNESS_EPSILON)
			return IN_FRONT_OF_PLANE;
		if(dist < -PLANE_THICKNESS_EPSILON)
			return BEHIND_PLANE;
		return STRADDLING_PLANE;
	}
	static int testPolygonWithPlane(TrianglePolygon* polygon, Plane3D plane){
		int numFront = 0, numBehind = 0;
		// Loop over vertices of triangle
		for(int i = 0; i < 3; ++i){
			switch(testPointWithPlane(polygon->operator[](i), plane)){
			case IN_FRONT_OF_PLANE:
				++numFront;
				break;
			case BEHIND_PLANE:
				++numBehind;
				break;
			}
		}

		// Classify polygon to plane
		if(numFront != 0 && numBehind != 0)
			return STRADDLING_PLANE;
		if(numFront)
			return IN_FRONT_OF_PLANE;
		if(numBehind)
			return BEHIND_PLANE;
		return COPLANAR_WITH_PLANE;
	}

	void splitPolygon(TrianglePolygon* polygon, Plane3D plane, Polygon* frontPart, Polygon* backPart){
		int numFront = 0, numBack = 0;
#define MAX_POINTS 10
    Vec3 frontVerts[MAX_POINTS], backVerts[MAX_POINTS];
    // Test all edges (a, b) starting with edge from last to first vertex
    int numVerts = 3;
		Vec3 a = polygon->operator[](numVerts - 1);
    int aSide = testPointWithPlane(a, plane);
		// Loop over all edges given by vertex pair (n - 1, n)
    for (int n = 0; n < numVerts; n++) {
			Vec3 b = polygon->operator[](n);
			int bSide = testPointWithPlane(b, plane);
			if (bSide == IN_FRONT_OF_PLANE) {
				if (aSide == BEHIND_PLANE) {
					// Edge (a, b) straddles, output intersection point to both sides
					Vec3 i = IntersectEdgeAgainstPlane(a, b, plane);
					assert(testPointWithPlane(i, plane) == COPLANAR_WITH_PLANE);
					frontVerts[numFront++] = backVerts[numBack++] = i;
				}
				// In all three cases, output b to the front side
				frontVerts[numFront++] = b;
			} else if (bSide == BEHIND_PLANE) {
				if (aSide == IN_FRONT_OF_PLANE) {
					// Edge (a, b) straddles plane, output intersection point
					Vec3 i = IntersectEdgeAgainstPlane(a, b, plane);
					assert(testPointWithPlane(i, plane) == COPLANAR_WITH_PLANE);
					frontVerts[numFront++] = backVerts[numBack++] = i;
				} else if (aSide == COPLANAR_WITH_PLANE) {
					// Output a when edge (a, b) goes from ‘on’ to ‘behind’ plane
					backVerts[numBack++] = a;
				}
				// In all three cases, output b to the back side
				backVerts[numBack++] = b;
			} else {
				// b is on the plane. In all three cases output b to the front side
				frontVerts[numFront++] = b;
				// In one case, also output b to back side
				if (aSide == BEHIND_PLANE)
					backVerts[numBack++] = b;
			}
			// Keep b as the starting point of the next edge
			a = b;
			aSide = bSide;
    }
    // Create (and return) two new polygons from the two vertex lists
		std::vector<Vec3> front, back;
		for(int i = 0; i < numFront; ++i)
			front.push_back(frontVerts[i]);
		for(int i = 0; i < numBack; ++i)
			back.push_back(backVerts[i]);
    frontPart = new Polygon(numFront, front);
    backPart = new Polygon(numBack, back);
	}

	Vec3 IntersectEdgeAgainstPlane(Vec3 a, Vec3 b, Plane3D plane){
		Vec3 q;
		// Compute the t value for the directed line ab intersecting the plane
		Vec3 ab = b - a;
		float t = (plane.d - dot(plane.n, a)) / dot(plane.n, ab);
		// If t in [0..1] compute and return intersection point
		if (t >= 0.0f && t <= 1.0f) {
			q = a + ab * t;
			return q;
		}
		// Else no intersection
		assert(false); // Should not reach since we know edge intersects plane!
		return Vec3(0);
	}
};
*/

#endif
