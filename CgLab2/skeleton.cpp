#include <iostream>
#include <glm/glm.hpp>
#include <SDL.h>
#include <vector>
#include "SDLauxiliary.h"
#include "TestModel.h"

using namespace std;
using glm::vec3;
using glm::mat3;

// ----------------------------------------------------------------------------
// GLOBAL VARIABLES

const int SCREEN_WIDTH = 500;
const int SCREEN_HEIGHT = 500;
SDL_Surface* screen;
int t;
vector<Triangle> triangles;
float focalLength = 10;
vec3 cameraPos( 0, 0, 0 );
vec3 start( 0, 0, 0 );



// ----------------------------------------------------------------------------
// FUNCTIONS


struct Intersection {
	
		vec3 position;
		float distance;
		int triangleIndex;
	};
	
void Update();
void Draw();
bool ClosestIntersection(
	vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection );
void CalcDirection(int x, int y, vec3& d);

int main( int argc, char* argv[] )
{
	screen = InitializeSDL( SCREEN_WIDTH, SCREEN_HEIGHT );
	LoadTestModel(triangles);
	cout << triangles.size();
	t = SDL_GetTicks();	// Set start value for timer.

	while( NoQuitMessageSDL() )
	{
		Update();
		Draw();
	}

	SDL_SaveBMP( screen, "screenshot.bmp" );
	return 0;
}

bool ClosestIntersection(vec3 start,
	vec3 dir,
	const vector<Triangle>& triangles,
	Intersection& closestIntersection ){
	
	bool exists = false;
	for(int i=0; i<triangles.size(); i++){
		
		using glm::vec3;
		using glm::mat3;
		vec3 v0 = triangles[i].v0;
		vec3 v1 = triangles[i].v1;
		vec3 v2 = triangles[i].v2;
		vec3 e1 = v1 - v0;
		vec3 e2 = v2 - v0;
		vec3 b = start -v0;
		mat3 A( -dir, e1, e2 );
		vec3 x = glm::inverse( A ) * b;
		
		if ( x.x > 0 ){	
			if (x.y > 0) {	
				if (x.z > 0) {
					if (x.y+x.z < 1){
						if (closestIntersection.distance < x.t){
							closestIntersection.position = x.x*dir;
							closestIntersection.distance = x.x;
							closestIntersection.triangleIndex = i;
						}
					}
				}
			}
			exists = true;
		}
		
	}
	return exists;
}

void Update()
{
	// Compute frame time:
	int t2 = SDL_GetTicks();
	float dt = float(t2-t);
	t = t2;
	// cout << "Render time: " << dt << " ms." << endl;
}

void CalcDirection(int x, int y, vec3& d){
	
	d.x = x-SCREEN_WIDTH/2;
	d.y = y-SCREEN_HEIGHT/2;
	d.z = focalLength;
	
}

void Draw()
{

/*
check for each pixel, shoot a ray and check for collisions.
*/
	if( SDL_MUSTLOCK(screen) )
		SDL_LockSurface(screen);

	for( int y=0; y<SCREEN_HEIGHT; ++y )
	{
		for( int x=0; x<SCREEN_WIDTH; ++x )
		{
			vec3 dir(0,0,0);
			Intersection closestIntersection;
			CalcDirection(x,y,dir);
			bool exists = ClosestIntersection(start, dir, triangles, closestIntersection);
			if (exists)
				PutPixelSDL( screen, x, y, triangles[closestIntersection.triangleIndex].color );
				
		}
	}

	if( SDL_MUSTLOCK(screen) )
		SDL_UnlockSurface(screen);

	SDL_UpdateRect( screen, 0, 0, 0, 0 );
}