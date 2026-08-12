#version 330 core

// IN ----------------
in vec4 fragColor;
in vec2 fragCoord;

// OUT ----------------
out vec4 fragShaded;

// UNIFORM ------------
uniform vec2 resolution;
uniform float playTime;
uniform float lowPos;
uniform float midPos;
uniform float highPos;
uniform float peak;
uniform float RMS;
uniform float lowsAmp;
uniform float midsAmp;
uniform float highsAmp;
uniform int state;

#define attenuation 0.05

const int Steps = 1000;
const float Epsilon = 0.05; // Marching epsilon
const float T=0.5; // treshold : "collide with objects when their potential field > T"

const float rA=1.0; // Minimum ray marching distance from origin
const float rB=50.0; // Maximum


const vec3 lightPos = vec3(5.0, 5.0, 5.0);

//									when state==1 							when state€(2,3)
vec3 lightColor = 			mix(vec3(0.1020, 0.0980, 0.3255), vec3(0.6353, 0.0675, 0.9255), float(state==1));
vec3 ambientcolor = 		mix(vec3(0.3686, 0.1412, 0.3059), vec3(0.0118, 0.0235, 0.2157), float(state==1));
vec3 glowColor = 			mix(vec3(1.0000, 0.5216, 0.7333), vec3(0.5686, 0.0392, 0.4039), float(state==1));
vec3 bgColorLight = 		mix(vec3(0.0078, 0.1020, 0.3294), vec3(0.5490, 0.0000, 0.0000), float(state==1));
vec3 bgColorDark = 		mix(vec3(0.0314, 0.0235, 0.0863), vec3(0.7569, 0.2000, 0.5137), float(state==1));


float atanp(in vec2 p) { return atan(p.y,p.x); }
float cube_root(float x) { return sign(x) * pow(abs(x), 1./3.); }
float cube(float x) { return x*x*x; }

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
float cube(vec3 p, vec3 pos, float e, float R, vec3 orientation)
{
	vec3 or_b = normalize(cross(orientation, pos));
	vec3 or_c = normalize(cross(orientation, or_b));
	float dist_or_a = abs(dot(p, orientation)-dot(pos, orientation));
	float dist_or_b = abs(dot(p, or_b)-dot(pos, or_b));
	float dist_or_c = abs(dot(p, or_c)-dot(pos, or_c));
	return min(min(falloff(dist_or_a, R),falloff(dist_or_b, R)),falloff(dist_or_c, R));
}
 
 
 
float objects(vec3 p)
{	
	// object variables
	float R = 2.0;
	
	float pT = playTime/5.;
	vec3 pointpos1 = vec3(sin(pT)*5*sin(pT/10)*pow(cos(pT), 3),
							cos(pT)*5,
							pow(sin(pT), 3)*10)*sin(pT/10);
	float pointsize = R; //- exp(-6*mod(playTime, 10));
	
	vec3 cubepos1 = vec3(2.5, 0.0, 0.0);
	vec3 cubepos2 = vec3(R+sin(pT/2),R+sin(pT/5),R+sin(pT/10)*4);
	
	vec3 cubdir1 = normalize(rotateX(vec3(1,1,1), lowPos));
	vec3 cubdir2 = normalize(rotateY(vec3(1,0,1), lowPos));
	
	// Evaluate potential field
	float v = point(p, pointpos1, 1.0 , pointsize) + 
		cube(p, cubepos1, 1.0, R, cubdir1) + 
		cube(p, cubepos2, 1.0, R, cubdir2);
	
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

    vec2 p = vec2(atan(rd.y/rd.z), atan(rd.x/rd.z))*(cos(playTime/50)*19+1);
    float z = 1.;
    z *= sin(length(p + vec2(sin(playTime/15)*5.,0.))) * cos(8.*atanp(p + vec2(5.,0.)));
    z *= sin(length(p - vec2(5.,5.))) * cos(8.*atanp(p - vec2(5.,sin(playTime/15)*5.)));
    z *= sin(length(p + vec2(0.,sin(playTime/15)*5.))) * cos(8.*atanp(p + vec2(0.,5.)));
    if(-0.1 < z && z < 0. || 0.2 < z) return mix(bgColorDark, vec3(0,0,0), rd.y+0.25);
    return mix(bgColorLight, glowColor, 0.20);


}

// Shading and lighting
// p : point,
// n : normal at point
vec3 Shade(vec3 p, vec3 n, vec3 rd)
{
   
   // light direction
   vec3 l = normalize(lightPos - p);

   // Not even Phong shading, use weighted cosine instead for smooth transitions
   float diff = cos(1-dot(n, l));
   
   float fresnel = cube(1.0 - dot(rd, n));
   
   // we mix ambient and diffuse components with an equal weight
   vec3 c =  mix(mix(ambientcolor, lightColor, diff), glowColor , fresnel);
   return c;
}

vec4 topShading(vec3 p, vec3 rgb)
{
	vec3 color;
	float lum = .4;
	if (state==0)
	{
		color = vec3(clamp(
										pow((rgb+lum), vec3(6)),
								0.0, 1.0));
	}
	else
	{
		color = vec3(clamp(
										vec3(rgb.x+sin(highPos)/10.0,
										rgb.y+cos(highPos)/15.0,
										rgb.z+cube(sin(highPos))/5.0), 
								0.0, 1.0));
	}
	
	if (length(color)>lum) color += glowColor*cube(RMS)/10.;
	return vec4(color, 1.);
}


void main()
{
   vec2 uv = fragCoord.xy;

   // compute ray origin and direction
   float asp = resolution.x / resolution.y;
   vec3 rd = vec3(asp*uv.x, uv.y, -4.0); //POV direction
   vec3 ro = vec3(0.0, 0.0, 15.0); // POV origin
   
   float a=midPos*0.05;
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
	
   fragShaded = topShading(pos, rgb);
}
