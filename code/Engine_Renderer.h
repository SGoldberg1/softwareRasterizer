/* date = March 22nd 2022 1:36 pm */

#ifndef _ENGINE__RENDERER_H
#define _ENGINE__RENDERER_H

struct render_matrial
{
	f32 SpecularIntensisty;
	f32 SpecularShininess;
	render_bitmap Diffuse;
	render_bitmap Normal;
	render_bitmap Specular;
	render_bitmap Occlusion;
	v4 Color;
};

#endif //_ENGINE__RENDERER_H
