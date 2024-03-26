/* 
 *  File: 3d.c
 *  Cooper Harris
 *  CSC 352
 *  
 *  This program implements functions defined in 3d.h deal with 
 *  3D scenes. It can create and destroy a scene which contains a linked list
 *  of triangles that make up 3D objects. Users can add rectangular prisms, 
 *  pyramids, spheres and fractals to the scene and then write a scene to an STL file
 *  in either ascii or binary format.
 *
 *  It relies on several structs defined in 3d.h so the user will have to know
 *  how to use them to sucessfully add shapes to an STL file.
 */

#include "3d.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>


void Scene3D_add_triangle(Scene3D* scene, Triangle3D newTriangle);
void write_helper(double d, char* destination, char* appendChar);
void fractal_helper(Scene3D* scene, Coordinate3D origin, double size, int levels);

Scene3D* Scene3D_create() {
	Scene3D* scene = malloc(sizeof(Scene3D));

	scene->count = 0;
	scene->root = NULL;

	return scene;
}

void Scene3D_destroy(Scene3D* scene) {
	Triangle3DNode* cur = scene->root;
	
	for (int i = 0; i < scene->count; i++) {
		cur = cur->next;
		free(scene->root);
		scene->root = cur;
	}

	free(scene);

}

void Scene3D_write_stl_text(Scene3D* scene, char* file_name) {
	FILE* file = fopen(file_name, "w");
	char beginning[] = "solid scene\n";
	char facetBegin[] = "  facet normal 0.0 0.0 0.0\n    outer loop\n";
	char facetEnd[] = "    endloop\n  endfacet\n";
	char end[] = "endsolid scene";
	
	fwrite(beginning, sizeof(char), strlen(beginning), file);
	
	Triangle3DNode* cur = scene->root;

	while (cur != NULL) {
		char aPoints[50] = "      vertex "; char bPoints[50] = "      vertex ";
		char cPoints[50] = "      vertex ";

		/* These lines take the the coordinate of the triangle and
		 get the x y and z values and concatenate them into a string
		 that will be one line of the stl file representing a 3D vertex
		*/
		write_helper(cur->triangle.a.x, aPoints, " ");
		write_helper(cur->triangle.a.y, aPoints, " ");
		write_helper(cur->triangle.a.z, aPoints, "\n");
		
		write_helper(cur->triangle.b.x, bPoints, " ");
		write_helper(cur->triangle.b.y, bPoints, " ");
		write_helper(cur->triangle.b.z, bPoints, "\n");
		
		write_helper(cur->triangle.c.x, cPoints, " ");
		write_helper(cur->triangle.c.y, cPoints, " ");
		write_helper(cur->triangle.c.z, cPoints, "\n");

		fwrite(facetBegin, sizeof(char), strlen(facetBegin), file);
		fwrite(aPoints, sizeof(char), strlen(aPoints), file); 
		fwrite(bPoints, sizeof(char), strlen(bPoints), file); 
		fwrite(cPoints, sizeof(char), strlen(cPoints), file); 
		fwrite(facetEnd, sizeof(char), strlen(facetEnd), file);
		
		cur = cur->next;
	}
	
	fwrite(end, sizeof(char), strlen(end), file);
	fclose(file);
}

/* 
 This function is a helper method that appends a double as a string into an existing string.
 It takes the double as input and uses the sprintf function to convert it to a string. It then
 appends that string along with another which will be either a space or newline into the
 given destination string.
*/
void write_helper(double d, char* destination, char* appendChar) {
	char vertex[50] = "";

	sprintf(vertex, "%.5f", d);
	strcat(vertex, appendChar);
	strcat(destination, vertex);
}


void Scene3D_write_stl_binary(Scene3D* scene, char* file_name) {
	FILE* file = fopen(file_name, "wb");
	char null = '\0';
	uint32_t count = scene->count;
	float normal[] = {0.0, 0.0, 0.0};
	float coords[9];
	uint16_t zero = 0;
	
	// Zero out the first 80 bytes
	for (int i = 0; i < 80; i++) {
		fwrite(&null, sizeof(char), 1, file);
	}
	// Write the next 4 as the triangle count
	fwrite(&count, sizeof(uint32_t), 1, file);

	Triangle3DNode* cur = scene->root;

	// loop through the list that contains the triangles and write them
	// Each triangle takes 50 bytes
	while (cur != NULL) {
		// facet normal
		fwrite(&normal, sizeof(float), 3, file);
		
		// each group is one point of the triangle
		coords[0] = (float) cur->triangle.a.x;
		coords[1] = (float)cur->triangle.a.y;
		coords[2] = (float)cur->triangle.a.z;

		
		coords[3] = (float)cur->triangle.b.x;
		coords[4] = (float)cur->triangle.b.y;
		coords[5] = (float)cur->triangle.b.z;
		
		coords[6] = (float)cur->triangle.c.x;
		coords[7] = (float)cur->triangle.c.y;
		coords[8] = (float)cur->triangle.c.z;
		
		fwrite(&coords, sizeof(float), 9, file);
		fwrite(&zero, sizeof(uint16_t), 1, file);

		cur = cur->next;
	}
	
}

void Scene3D_add_pyramid(Scene3D* scene, Coordinate3D origin, double width, double height, char* orientation) {
	Coordinate3D vertex1, vertex2, vertex3, vertex4;
	Coordinate3D peak;

	if (strcmp(orientation, "up") == 0) {
		peak.x = origin.x;
		peak.y = origin.y;
		peak.z = origin.z + height;

		vertex1.x = origin.x - width / 2;
		vertex1.y = origin.y + width / 2;
		vertex1.z = origin.z;

		vertex2.x = origin.x + width / 2;
		vertex2.y = origin.y + width / 2;
		vertex2.z = origin.z;

		vertex3.x = origin.x + width / 2;
		vertex3.y = origin.y - width / 2;
		vertex3.z = origin.z;

		vertex4.x = origin.x - width / 2;
		vertex4.y = origin.y - width / 2;
		vertex4.z = origin.z;
	} else if (strcmp(orientation, "down") == 0) {
		peak.x = origin.x;
		peak.y = origin.y;
		peak.z = origin.z - height;
	
		vertex1.x = origin.x - width / 2;
		vertex1.y = origin.y + width / 2;
		vertex1.z = origin.z;

		vertex2.x = origin.x + width / 2;
		vertex2.y = origin.y + width / 2;
		vertex2.z = origin.z;

		vertex3.x = origin.x + width / 2;
		vertex3.y = origin.y - width / 2;
		vertex3.z = origin.z;

		vertex4.x = origin.x - width / 2;
		vertex4.y = origin.y - width / 2;
		vertex4.z = origin.z;
	} else if (strcmp(orientation, "left") == 0) {
		peak.x = origin.x - height;
		peak.y = origin.y;
		peak.z = origin.z;
		
		vertex1.x = origin.x;
		vertex1.y = origin.y - width / 2;
		vertex1.z = origin.z - width / 2;

		vertex2.x = origin.x;
		vertex2.y = origin.y - width / 2; 
		vertex2.z = origin.z + width / 2;

		vertex3.x = origin.x;
		vertex3.y = origin.y + width / 2;
		vertex3.z = origin.z + width / 2;

		vertex4.x = origin.x;
		vertex4.y = origin.y + width / 2;
		vertex4.z = origin.z - width / 2;
	} else if (strcmp(orientation, "right") == 0) {
		peak.x = origin.x + height;
		peak.y = origin.y;
		peak.z = origin.z;

		vertex1.x = origin.x;
		vertex1.y = origin.y - width / 2;
		vertex1.z = origin.z - width / 2;

		vertex2.x = origin.x;
		vertex2.y = origin.y - width / 2; 
		vertex2.z = origin.z + width / 2;

		vertex3.x = origin.x;
		vertex3.y = origin.y + width / 2;
		vertex3.z = origin.z + width / 2;

		vertex4.x = origin.x;
		vertex4.y = origin.y + width / 2;
		vertex4.z = origin.z - width / 2;
	} else if (strcmp(orientation, "forward") == 0) {
		peak.x = origin.x;
		peak.y = origin.y + height;
		peak.z = origin.z;

		vertex1.x = origin.x - width / 2;
		vertex1.y = origin.y;
		vertex1.z = origin.z - width / 2;

		vertex2.x = origin.x - width / 2;
		vertex2.y = origin.y; 
		vertex2.z = origin.z + width / 2;

		vertex3.x = origin.x + width / 2;
		vertex3.y = origin.y;
		vertex3.z = origin.z + width / 2;

		vertex4.x = origin.x + width / 2;
		vertex4.y = origin.y;
		vertex4.z = origin.z - width / 2;
	} else if (strcmp(orientation, "backward") == 0) {
		peak.x = origin.x;
		peak.y = origin.y - height;
		peak.z = origin.z;

		vertex1.x = origin.x - width / 2;
		vertex1.y = origin.y;
		vertex1.z = origin.z - width / 2;

		vertex2.x = origin.x - width / 2;
		vertex2.y = origin.y; 
		vertex2.z = origin.z + width / 2;

		vertex3.x = origin.x + width / 2;
		vertex3.y = origin.y;
		vertex3.z = origin.z + width / 2;

		vertex4.x = origin.x + width / 2;
		vertex4.y = origin.y;
		vertex4.z = origin.z - width / 2;
	}

	// Create the four triangles for the side of the pyramid and add them to the scene	
	Triangle3D t1 = {vertex1, vertex2, peak};
	Triangle3D t2 = {vertex1, vertex4, peak};
	Triangle3D t3 = {vertex2, vertex3, peak};
	Triangle3D t4 = {vertex3, vertex4, peak};
    
	Scene3D_add_triangle(scene, t1);
	Scene3D_add_triangle(scene, t2);
	Scene3D_add_triangle(scene, t3);
	Scene3D_add_triangle(scene, t4);
	
	// Create the base
	Scene3D_add_quadrilateral(scene, vertex1, vertex2, vertex3, vertex4);
}


void Scene3D_add_cuboid(Scene3D* scene, Coordinate3D origin, double width, double height, double depth) {	
	Coordinate3D vertex1 = {origin.x - width/ 2, origin.y - height / 2, origin.z + depth / 2};
	Coordinate3D vertex2 = {origin.x - width/ 2, origin.y + height / 2, origin.z + depth / 2};
	Coordinate3D vertex3 = {origin.x + width/ 2, origin.y + height / 2, origin.z + depth / 2};
	Coordinate3D vertex4 = {origin.x + width/ 2, origin.y - height / 2, origin.z + depth / 2};

	Coordinate3D vertex5 = {origin.x + width/ 2, origin.y - height / 2, origin.z - depth / 2};
	Coordinate3D vertex6 = {origin.x + width/ 2, origin.y + height / 2, origin.z - depth / 2};
	Coordinate3D vertex7 = {origin.x - width/ 2, origin.y + height / 2, origin.z - depth / 2};
	Coordinate3D vertex8 = {origin.x - width/ 2, origin.y - height / 2, origin.z - depth / 2};

	Scene3D_add_quadrilateral(scene, vertex1, vertex2, vertex3, vertex4);
	Scene3D_add_quadrilateral(scene, vertex3, vertex4, vertex6, vertex5);
	Scene3D_add_quadrilateral(scene, vertex5, vertex6, vertex7, vertex8);
	Scene3D_add_quadrilateral(scene, vertex1, vertex2, vertex7, vertex8);
	Scene3D_add_quadrilateral(scene, vertex1, vertex4, vertex5, vertex8);
	Scene3D_add_quadrilateral(scene, vertex2, vertex3, vertex6, vertex7);
}


void Scene3D_add_sphere(Scene3D* scene, Coordinate3D origin, double radius, double increment) {
    double cosPhi, sinPhi, cosTheta, sinTheta;

    for (double phi = increment; phi <= 180; phi += increment) {
		// The math sin and cos functions take their arguments as radians so convert the degrees
        cosPhi = cos(phi * PI / 180);
        sinPhi = sin(phi * PI / 180);

        for (double theta = 0; theta < 360; theta += increment) {
			// The math sin and cos functions take their arguments as radians so convert the degrees
            cosTheta = cos(theta * PI / 180);
            sinTheta = sin(theta * PI / 180);

			// Each coordinate must be converted from shperical coordinates to cartesian coordinates
            Coordinate3D C1 = {radius * sinPhi * cosTheta, radius * sinPhi * sinTheta, radius * cosPhi};
            C1.x += origin.x;
            C1.y += origin.y;
            C1.z += origin.z;
            C1.x = round(C1.x * 10000) / 10000;
            C1.y = round(C1.y * 10000) / 10000;
            C1.z = round(C1.z * 10000) / 10000;

			// Some small values are rounded to 0 and give a -0.00000 coord so hard code them to 0
			if (fabs(C1.x) < 0.0001) C1.x = 0;
            if (fabs(C1.y) < 0.0001) C1.y = 0;
            if (fabs(C1.z) < 0.0001) C1.z = 0;

            Coordinate3D C2 = {radius * sinPhi * cos((theta + increment) * PI / 180), radius * sinPhi * sin((theta + increment) * PI / 180), radius * cosPhi};
            C2.x += origin.x;
            C2.y += origin.y;
            C2.z += origin.z;
            C2.x = round(C2.x * 10000) / 10000;
            C2.y = round(C2.y * 10000) / 10000;
            C2.z = round(C2.z * 10000) / 10000;
			
			// Some small values are rounded to 0 and give a -0.00000 coord so hard code them to 0
			if (fabs(C2.x) < 0.0001) C2.x = 0;
            if (fabs(C2.y) < 0.0001) C2.y = 0;
            if (fabs(C2.z) < 0.0001) C2.z = 0;
            
			Coordinate3D C3 = {radius * sin((phi - increment) * PI / 180) * cosTheta, radius * sin((phi - increment) * PI / 180) * sinTheta, radius * cos((phi - increment) * PI / 180)};
            C3.x += origin.x;
            C3.y += origin.y;
            C3.z += origin.z;
            C3.x = round(C3.x * 10000) / 10000;
            C3.y = round(C3.y * 10000) / 10000;
            C3.z = round(C3.z * 10000) / 10000;
			
			// Some small values are rounded to 0 and give a -0.00000 coord so hard code them to 0
			if (fabs(C3.x) < 0.0001) C3.x = 0;
            if (fabs(C3.y) < 0.0001) C3.y = 0;
            if (fabs(C3.z) < 0.0001) C3.z = 0;
            
			Coordinate3D C4 = {radius * sin((phi - increment) * PI / 180) * cos((theta + increment) * PI / 180), radius * sin((phi - increment) * PI / 180) * sin((theta + increment) * PI / 180), radius * cos((phi - increment) * PI / 180)};
            C4.x += origin.x;
            C4.y += origin.y;
            C4.z += origin.z;
            C4.x = round(C4.x * 10000) / 10000;
            C4.y = round(C4.y * 10000) / 10000;
            C4.z = round(C4.z * 10000) / 10000;
			
			// Some small values are rounded to 0 and give a -0.00000 coord so hard code them to 0
			if (fabs(C4.x) < 0.0001) C4.x = 0;
            if (fabs(C4.y) < 0.0001) C4.y = 0;
            if (fabs(C4.z) < 0.0001) C4.z = 0;
           
		   Scene3D_add_quadrilateral(scene, C1, C2, C3, C4);
        }
    }
}


void Scene3D_add_fractal(Scene3D* scene, Coordinate3D origin, double size, int levels) {
	// Add the largest center cube
	Scene3D_add_cuboid(scene, origin, size, size, size);
	
	if (levels > 1) {
		// each level of cubes should be half the size of the previous level
        double new_size = size / 2.0;
        Coordinate3D new_origin;

        // create 6 new cubes and place them in the center of each side of the previous cube
        for (int i = 0; i < 6; i++) {
            switch (i) {
                case 0: // front
                    new_origin = (Coordinate3D){origin.x, origin.y, origin.z - new_size};
                    break;
                case 1: // back
                    new_origin = (Coordinate3D){origin.x, origin.y, origin.z + new_size};
                    break;
                case 2: // top
                    new_origin = (Coordinate3D){origin.x, origin.y + new_size, origin.z};
                    break;
                case 3: // bottom
                    new_origin = (Coordinate3D){origin.x, origin.y - new_size, origin.z};
                    break;
                case 4: // right
                    new_origin = (Coordinate3D){origin.x + new_size, origin.y, origin.z};
                    break;
                case 5: // left
                    new_origin = (Coordinate3D){origin.x - new_size, origin.y, origin.z};
                    break;
            }
            Scene3D_add_fractal(scene, new_origin, new_size, levels - 1);
        }
    }	
}



void Scene3D_add_quadrilateral(Scene3D* scene, Coordinate3D a, Coordinate3D b, Coordinate3D c, Coordinate3D d) { 
	Triangle3D triangle_1 = (Triangle3D) {a, b, c}; 
	Triangle3D triangle_2 = (Triangle3D) {b, c, d}; 
	Triangle3D triangle_3 = (Triangle3D) {a, c, d}; 
	Triangle3D triangle_4 = (Triangle3D) {a, b, d}; 

	Scene3D_add_triangle(scene, triangle_1); 
	Scene3D_add_triangle(scene, triangle_2); 
	Scene3D_add_triangle(scene, triangle_3); 
	Scene3D_add_triangle(scene, triangle_4); 
}

/*
 * This function takes a scene and a Triangle3D to add to the scene as
 * parameters. It allocates space for a TriangleNode3D to hold the triangle
 * and prepends the node to the linked list holding all triangle nodes in the
 * scene
*/
void Scene3D_add_triangle(Scene3D* scene, Triangle3D newTriangle) {
	Triangle3DNode* node = malloc(sizeof(Triangle3DNode));
	node->triangle = newTriangle;

	node->next = scene->root;
			
	scene->root  = node;
	scene->count++;

}

/*
 * Adds a triangle at the end of a linked list
*/
void Scene3D_add_triangle_end(Scene3D* scene, Triangle3D newTriangle) {
	if (scene->root == NULL) {
		scene->root = malloc(sizeof(Triangle3DNode));
		
		scene->root->triangle = newTriangle;
		scene->root->next = NULL;

		scene->count = 1;
		return;
	} 
	
	Triangle3DNode* cur = scene->root;
	
	while (cur != NULL) {
		if (cur->next == NULL) {
			Triangle3DNode* node = malloc(sizeof(Triangle3DNode));
			node->triangle = newTriangle;
			node->next = NULL;
			
			cur->next = node;
			scene->count++;
			
			break;
		}

		cur = cur->next;
	}
}














