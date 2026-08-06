#version 330 core

// IN ----------------
in vec4 fragColor;
in vec2 fragCoord;

// OUT ----------------
out vec4 fragShaded;

// UNIFORM ------------
uniform vec2 resolution;
uniform float playTime;
uniform float mouvTime;
uniform float peak;
uniform float RMS;
uniform float lowsAmp;
uniform float midsAmp;
uniform float highsAmp;

#define attenuation 0.05

const int Steps = 1000;
const float Epsilon = 0.05; // Marching epsilon
const float T=0.5; // treshold "collide with objects when their potential field > T"

const float rA=1.0; // Minimum ray marching distance from origin
const float rB=50.0; // Maximum

float atanp(in vec2 p) { return atan(p.y,p.x); }
float cube_root(float x) { return sign(x) * pow(abs(x), 1./3.); }
float sq(float x) { return x*x; }

// Rotates around the Y axis
vec3 rotateY(vec3 p, float a) {mat3 rota = mat3(cos(a),0,-sin(a),0,1,0,sin(a),0,cos(a));return rota*p;}

// Rotates around the X axis
vec3 rotateX(vec3 p, float a) {mat3 rota = mat3(cos(a),0,-sin(a),0,1,0,sin(a),0,cos(a));return rota*p.yxz;}

// Rotates around the Z axis
vec3 rotateZ(vec3 p, float a) {mat3 rota = mat3(cos(a),0,-sin(a),0,1,0,sin(a),0,cos(a));return rota*p.xzy;}

// Smooth falloff function
// r : small radius
// R : Large radius
float falloff( float r, float R )
{
   float x = clamp(r/R,0.0,1.0);
   float y = (1.0-x*x);
   return y*y*y;
}


// Primitive functions

// Point skeleton
// p : point
// c : center of skeleton
// e : energy associated to skeleton
// R : large radius
float point(vec3 p, vec3 c, float e,float R)
{
   return e*falloff(length(p-c),R);
}

// Cube
float cube(vec3 p, vec3 pos, float e, float R)
{
	vec3 b1 = pos + R/2*vec3(1.0,1.0,1.0);
	vec3 b2 = pos - R/2*vec3(1.0,1.0,1.0);
	float v = 0.0;
	
	bool inBounds = all(greaterThan(p.xyz, b2.xyz)) && all(lessThan(p.xyz, b1.xyz));
	if (inBounds)
	{
		v = falloff(p.x-pos.x, R) + falloff(p.y-pos.y, R) + falloff(p.z-pos.z, R)
			+ falloff(pos.x-p.x, R) + falloff(pos.y-p.y, R) + falloff(pos.z-p.z, R);
	}
	else
	{
		v = point(p, pos, e, R);
	}
	return v;
}

// Blending
// a : field function of left sub-tree
// b : field function of right sub-tree
float Blend(float a,float b)
{
   return a+b;
}




float objects(vec3 p)
{

	vec3 pointpos = vec3(-2.5+sin(playTime)*5, 0.0, 0.0);
	vec3 cubepos = vec3(2.5, 0.0, 0.0);
	float v = Blend(point(p, pointpos, 1.0 ,2.0), cube(p, cubepos, 1.0, 2.0));
	
	return v - T;
}

// Calculate objects normal
// p : point
vec3 ObjectNormal(in vec3 p )
{
   float eps = 0.0001;
   vec3 n;
   float v = objects(p);
   n.x = objects( vec3(p.x+eps, p.y, p.z) ) - v;
   n.y = objects( vec3(p.x, p.y+eps, p.z) ) - v;
   n.z = objects( vec3(p.x, p.y, p.z+eps) ) - v;
   return normalize(n);
}

// Trace ray using ray marching
// o : ray origin
// u : ray direction
// h : hit
// s : Number of steps
float Trace(vec3 o, vec3 u, out bool h,out int s)
{
   h = false;

   // Don't start at the origin
   // instead move a little bit forward
   float t=rA;

   for(int i=0; i<Steps; i++)
   {
      s=i;
      vec3 p = o+t*u;
      float v = objects(p);
      // Hit objects (1) 
      if (v > 0.0)
      {
         s=i;
         h = true;
         break;
      }
      // Move along ray
      t += Epsilon;  

      // Escape marched far away
      if (t>rB)
      {
         break;
      }
   }
   return t;
}

// Background color
vec3 background(vec3 rd)
{

    vec2 p = vec2(atan(rd.y/rd.z), atan(rd.x/rd.z))*10;
    float z = 1.;
    z *= sin(length(p + vec2(5.,0.))) * cos(8.*atanp(p + vec2(5.,0.)));
    z *= sin(length(p - vec2(5.,5.))) * cos(8.*atanp(p - vec2(5.,5.)));
    z *= sin(length(p + vec2(0.,5.))) * cos(8.*atanp(p + vec2(0.,5.)));
    if(-0.1 < z && z < 0. || 0.2 < z) return mix(vec3(0.2, 0.1, 0.005), vec3(0.3, 0.2, 0.3), rd.y*1.0+0.25);
    return mix(vec3(0.8, 0.8, 0.9), vec3(0.6, 0.9, 1.0), rd.y*1.0+0.25);


}

// Shading and lighting
// p : point,
// n : normal at point
vec3 Shade(vec3 p, vec3 n, vec3 rd)
{
   // point light
   const vec3 lightPos = vec3(5.0, 5.0, 5.0); // position of the light
   const vec3 lightColor = vec3(1.0, 1.0, 1.0);
   const vec3 ambientcolor = vec3(0.5,0.5,0.5);
   const vec3 glowColor = vec3(0.40, 0.80, 1.00);
   
   // light direction
   vec3 l = normalize(lightPos - p);

   // Not even Phong shading, use weighted cosine instead for smooth transitions
   float diff = 0.5*(1.0+dot(n, l));
   
   float fresnel = pow(1.0 - dot(rd, n), 3);
   
   // we mix ambient and diffuse components with an equal weight
   vec3 c =  mix(mix(ambientcolor, lightColor, 0), glowColor , fresnel);
   return c;
}

void main()
{
   vec2 pixel = fragCoord.xy;//(fragCoord.xy / resolution.xy)*2.0-1.0;

   // compute ray origin and direction
   float asp = resolution.x / resolution.y;
   vec3 rd = vec3(asp*pixel.x, pixel.y, -4.0); //POV direction
   vec3 ro = vec3(0.0, 0.0, 15.0); // POV origin
   
   float a=playTime*0.05;
   //rd.z = rd.z+2.0*mouse.y;
   rd = normalize(rd);
   ro = rotateX(rotateZ(rotateY(ro, a*1.1),a*0.9),a);
   rd = rotateX(rotateZ(rotateY(rd, a*1.1),a*0.9),a);


   // Trace ray
   bool hit;

   // Number of steps
   int s;

   float t = Trace(ro, rd, hit,s);
   vec3 pos=ro+t*rd;
   // Shade background
   vec3 rgb = background(rd);

   if (hit)
   {
      // Compute normal
      vec3 n = ObjectNormal(pos);

      // Shade objects with light
      rgb = Shade(pos, n, rd);
   }

   fragShaded=vec4(rgb, 1.0);
}
/*
void main()
{
	float n1 = lowsAmp;
	float n2 = midsAmp;
	float n3 = highsAmp;
	
	// From NSC to pixels
	vec2 uv = fragCoord.xy;
	uv.y *= resolution.y/resolution.x;
	
	if(mod(length(uv)*100,playTime) <= 2)
	{
		fragShaded = vec4(0,1,0,1);
	}
	else
	{
		fragShaded = vec4(1,1,1,1);
	}
}
*/
