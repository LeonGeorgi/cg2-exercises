// This source code is property of the Computer Graphics and Visualization chair of the
// TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved
//
// The main file of the plugin. It defines a class that demonstrates how to register with
// the scene graph, drawing primitives, creating a GUI, using a config file and various
// other parts of the framework.

#include <math.h> 

// Framework core
#include "cgv/base/register.h"
#include "cgv/gui/provider.h"
#include "cgv/gui/trigger.h"
#include "cgv/render/drawable.h"
#include "cgv/render/shader_program.h"
#include "cgv/render/vertex_buffer.h"
#include "cgv/render/attribute_array_binding.h"
#include "cgv/math/ftransform.h"
#include "cgv/media/illum/surface_material.h"
#include "cgv/media/color.h"

// Framework standard plugins
#include "libs/cgv_gl/gl/gl.h"

// Local includes
#include "cubes_fractal.h"


// ************************************************************************************/
// Task 1.2a: Create a drawable that provides a (for now, empty) GUI and supports
//            reflection, so that its properties can be set via config file.
//
// Task 1.2b: Utilize the cubes_fractal class to render a fractal of hierarchically
//            transformed cubes. Expose its recursion depth and color properties to GUI
//            manipulation and reflection. Set reasonable values via the config
//            file.
//
// Task 1.2c: Implement an option (configurable via GUI and config file) to use a vertex
//            array object for rendering the cubes. The vertex array functionality 
//            should support (again, configurable via GUI and config file) both
//            interleaved (as in cgv_demo.cpp) and non-interleaved attributes.

#define MAX_DEPTH 7



class cubes_fractal_drawable
	: public cgv::base::base,
	public cgv::gui::provider,
	public cgv::render::drawable
{

protected:

	bool drawFractal = false;
	cubes_fractal cubesFractal;

	cgv::rgba fractalColor;
	float fractalColorR, fractalColorG, fractalColorB;
	unsigned maxDepth = 3;
	enum Vertex_Array_Type {
		BUILTIN,
		INTERLEAVED,
		NON_INTERLEAVED,
		ALL_GEOMETRY_INTERLEAVED } vertexArrayType = Vertex_Array_Type::ALL_GEOMETRY_INTERLEAVED;

	struct vertex {
		cgv::vec3 pos;
		cgv::vec3 normal;
	};

	struct vertexColor {
		cgv::vec3 pos;
		cgv::vec3 normal;
		cgv::vec4 color;
	};

	std::vector<vertex> vertices;
	std::vector<cgv::vec3> nonInterleavedVertexData;

	cgv::render::vertex_buffer vb;
	cgv::render::vertex_buffer vb_non_interleaved;

	cgv::render::attribute_array_binding vertex_array_interleaved;
	cgv::render::attribute_array_binding vertex_array_non_interleaved;

	
	// Task 1.3.1: 
	cgv::media::illum::surface_material material;
	std::vector<vertexColor> vertices_all_geometry;

	cgv::render::vertex_buffer vb_all_geometry;
	cgv::render::attribute_array_binding vertex_array_all_geometry;

	int cubesPerLevelCounter[MAX_DEPTH] = { 0 };


	void initUnitCubeGeometry()
	{
		vertices.resize(36);
		nonInterleavedVertexData.resize(36 * 2);

		//Front face
		vertices[0].pos = cgv::vec3(-1, -1, 1);
		vertices[1].pos = cgv::vec3(1, -1, 1);
		vertices[2].pos = cgv::vec3(-1, 1, 1);

		vertices[3].pos = cgv::vec3(1, 1, 1);
		vertices[4].pos = cgv::vec3(-1, 1, 1);
		vertices[5].pos = cgv::vec3(1, -1, 1);

		vertices[0].normal = cgv::vec3(0, 0, 1);
		vertices[1].normal = cgv::vec3(0, 0, 1);
		vertices[2].normal = cgv::vec3(0, 0, 1);

		vertices[3].normal = cgv::vec3(0, 0, 1);
		vertices[4].normal = cgv::vec3(0, 0, 1);
		vertices[5].normal = cgv::vec3(0, 0, 1);

		//Back face
		vertices[6].pos = cgv::vec3(-1, -1, -1);
		vertices[7].pos = cgv::vec3(-1, 1, -1);
		vertices[8].pos = cgv::vec3(1, -1, -1);

		vertices[9].pos = cgv::vec3(1, -1, -1);
		vertices[10].pos = cgv::vec3(-1, 1, -1);
		vertices[11].pos = cgv::vec3(1, 1, -1);

		vertices[6].normal = cgv::vec3(0, 0, -1);
		vertices[7].normal = cgv::vec3(0, 0, -1);
		vertices[8].normal = cgv::vec3(0, 0, -1);

		vertices[9].normal = cgv::vec3(0, 0, -1);
		vertices[10].normal = cgv::vec3(0, 0, -1);
		vertices[11].normal = cgv::vec3(0, 0, -1);

		//Left face
		vertices[12].pos = cgv::vec3(-1, -1, -1);
		vertices[13].pos = cgv::vec3(-1, -1, 1);
		vertices[14].pos = cgv::vec3(-1, 1, -1);

		vertices[15].pos = cgv::vec3(-1, 1, -1);
		vertices[16].pos = cgv::vec3(-1, -1, 1);
		vertices[17].pos = cgv::vec3(-1, 1, 1);

		vertices[12].normal = cgv::vec3(-1, 0, 0);
		vertices[13].normal = cgv::vec3(-1, 0, 0);
		vertices[14].normal = cgv::vec3(-1, 0, 0);

		vertices[15].normal = cgv::vec3(-1, 0, 0);
		vertices[16].normal = cgv::vec3(-1, 0, 0);
		vertices[17].normal = cgv::vec3(-1, 0, 0);

		//Right face
		vertices[18].pos = cgv::vec3(1, -1, -1);
		vertices[19].pos = cgv::vec3(1, 1, -1);
		vertices[20].pos = cgv::vec3(1, -1, 1);

		vertices[21].pos = cgv::vec3(1, -1, 1);
		vertices[22].pos = cgv::vec3(1, 1, -1);
		vertices[23].pos = cgv::vec3(1, 1, 1);

		vertices[18].normal = cgv::vec3(1, 0, 0);
		vertices[19].normal = cgv::vec3(1, 0, 0);
		vertices[20].normal = cgv::vec3(1, 0, 0);

		vertices[21].normal = cgv::vec3(1, 0, 0);
		vertices[22].normal = cgv::vec3(1, 0, 0);
		vertices[23].normal = cgv::vec3(1, 0, 0);

		//Top face
		vertices[24].pos = cgv::vec3(-1, 1, -1);
		vertices[25].pos = cgv::vec3(-1, 1, 1);
		vertices[26].pos = cgv::vec3(1, 1, -1);

		vertices[27].pos = cgv::vec3(1, 1, -1);
		vertices[28].pos = cgv::vec3(-1, 1, 1);
		vertices[29].pos = cgv::vec3(1, 1, 1);

		vertices[24].normal = cgv::vec3(0, 1, 0);
		vertices[25].normal = cgv::vec3(0, 1, 0);
		vertices[26].normal = cgv::vec3(0, 1, 0);

		vertices[27].normal = cgv::vec3(0, 1, 0);
		vertices[28].normal = cgv::vec3(0, 1, 0);
		vertices[29].normal = cgv::vec3(0, 1, 0);

		//Bottom face
		vertices[30].pos = cgv::vec3(-1, -1, -1);
		vertices[31].pos = cgv::vec3(1, -1, -1);
		vertices[32].pos = cgv::vec3(-1, -1, 1);

		vertices[33].pos = cgv::vec3(1, -1, 1);
		vertices[34].pos = cgv::vec3(-1, -1, 1);
		vertices[35].pos = cgv::vec3(1, -1, -1);

		vertices[30].normal = cgv::vec3(0, -1, 0);
		vertices[31].normal = cgv::vec3(0, -1, 0);
		vertices[32].normal = cgv::vec3(0, -1, 0);

		vertices[33].normal = cgv::vec3(0, -1, 0);
		vertices[34].normal = cgv::vec3(0, -1, 0);
		vertices[35].normal = cgv::vec3(0, -1, 0);

		for (int i = 0; i < vertices.size(); i++) {
			nonInterleavedVertexData[i] = vertices[i].pos;
			nonInterleavedVertexData[i + vertices.size()] = vertices[i].normal;
		}
	}

	
	int calculateNumOfVertices(int depth) {
		int numVertices = 0;
		for (int i = 0; i <= depth; i++)
		{
			if(i == 0)
				numVertices += 36;
			else if(i == 1)
				numVertices += 36 * 4;
			else 
				numVertices += 36 * 4 * pow(3, i - 1);
		}

		return numVertices;
	}

	void addFractalGeometryRecursive(int level,cgv::dmat4 matrix, const cgv::media::color<float>& color) {
		matrix = matrix * cgv::math::scale4<double>(0.5, 0.5, 0.5);

		cgv::media::color<float, cgv::media::HLS> color_next(color);
		color_next.H() = std::fmod(color_next.H() + 0.2f, 1.0f);
		color_next.S() = std::fmod(color_next.S() - 0.05f, 1.0f);
		
		int baseIndex = calculateNumOfVertices(level);
		unsigned num_children = level == 0 ? 4 : 3;
		if (level < MAX_DEPTH) {
			for (unsigned i = 0; i < num_children; i++)
			{
				// Save transformation before recursion
				auto matrixTmp = matrix * cgv::math::rotate4<double>(signed(i) * 90 - 90, 0, 0, 1)  * cgv::math::translate4<double>(2, 0, 0);

				for (int j = 0; j < 36; j++) {
					cgv::vec3 pos = matrixTmp.mul_pos(vertices_all_geometry[j].pos);
					cgv::vec3 normal = matrixTmp.mul_dir(vertices_all_geometry[j].normal);
					cgv::vec4 color = cgv::vec4(color_next.R(), color_next.G(), color_next.B(), 1.0f);
				
					vertices_all_geometry[baseIndex + 36 * cubesPerLevelCounter[level] + j] = {pos, normal,color};
				}
				cubesPerLevelCounter[level]++;
				addFractalGeometryRecursive(level + 1, matrixTmp, color_next);
			}

		}
	}
	void initAllGeometryToOneVB() {

		vertices_all_geometry.resize(calculateNumOfVertices(MAX_DEPTH));

		vertices_all_geometry[0].pos = cgv::vec3(-0.5, -0.5, 0.5);
		vertices_all_geometry[1].pos = cgv::vec3(0.5, -0.5, 0.5);
		vertices_all_geometry[2].pos = cgv::vec3(-0.5, 0.5, 0.5);

		vertices_all_geometry[3].pos = cgv::vec3(0.5, 0.5, 0.5);
		vertices_all_geometry[4].pos = cgv::vec3(-0.5, 0.5, 0.5);
		vertices_all_geometry[5].pos = cgv::vec3(0.5, -0.5, 0.5);

		vertices_all_geometry[0].normal = cgv::vec3(0, 0, 0.5);
		vertices_all_geometry[1].normal = cgv::vec3(0, 0, 0.5);
		vertices_all_geometry[2].normal = cgv::vec3(0, 0, 0.5);

		vertices_all_geometry[3].normal = cgv::vec3(0, 0, 0.5);
		vertices_all_geometry[4].normal = cgv::vec3(0, 0, 0.5);
		vertices_all_geometry[5].normal = cgv::vec3(0, 0, 0.5);

		//Back face
		vertices_all_geometry[6].pos = cgv::vec3(-0.5, -0.5, -0.5);
		vertices_all_geometry[7].pos = cgv::vec3(-0.5, 0.5, -0.5);
		vertices_all_geometry[8].pos = cgv::vec3(0.5, -0.5, -0.5);

		vertices_all_geometry[9].pos = cgv::vec3(0.5, -0.5, -0.5);
		vertices_all_geometry[10].pos = cgv::vec3(-0.5, 0.5, -0.5);
		vertices_all_geometry[11].pos = cgv::vec3(0.5, 0.5, -0.5);

		vertices_all_geometry[6].normal = cgv::vec3(0, 0, -0.5);
		vertices_all_geometry[7].normal = cgv::vec3(0, 0, -0.5);
		vertices_all_geometry[8].normal = cgv::vec3(0, 0, -0.5);

		vertices_all_geometry[9].normal = cgv::vec3(0, 0, -0.5);
		vertices_all_geometry[10].normal = cgv::vec3(0, 0, -0.5);
		vertices_all_geometry[11].normal = cgv::vec3(0, 0, -0.5);

		//Left face
		vertices_all_geometry[12].pos = cgv::vec3(-0.5, -0.5, -0.5);
		vertices_all_geometry[13].pos = cgv::vec3(-0.5, -0.5, 0.5);
		vertices_all_geometry[14].pos = cgv::vec3(-0.5, 0.5, -0.5);

		vertices_all_geometry[15].pos = cgv::vec3(-0.5, 0.5, -0.5);
		vertices_all_geometry[16].pos = cgv::vec3(-0.5, -0.5, 0.5);
		vertices_all_geometry[17].pos = cgv::vec3(-0.5, 0.5, 0.5);

		vertices_all_geometry[12].normal = cgv::vec3(-0.5, 0, 0);
		vertices_all_geometry[13].normal = cgv::vec3(-0.5, 0, 0);
		vertices_all_geometry[14].normal = cgv::vec3(-0.5, 0, 0);

		vertices_all_geometry[15].normal = cgv::vec3(-0.5, 0, 0);
		vertices_all_geometry[16].normal = cgv::vec3(-0.5, 0, 0);
		vertices_all_geometry[17].normal = cgv::vec3(-0.5, 0, 0);

		//Right face
		vertices_all_geometry[18].pos = cgv::vec3(0.5, -0.5, -0.5);
		vertices_all_geometry[19].pos = cgv::vec3(0.5, 0.5, -0.5);
		vertices_all_geometry[20].pos = cgv::vec3(0.5, -0.5, 0.5);

		vertices_all_geometry[21].pos = cgv::vec3(0.5, -0.5, 0.5);
		vertices_all_geometry[22].pos = cgv::vec3(0.5, 0.5, -0.5);
		vertices_all_geometry[23].pos = cgv::vec3(0.5, 0.5, 0.5);

		vertices_all_geometry[18].normal = cgv::vec3(0.5, 0, 0);
		vertices_all_geometry[19].normal = cgv::vec3(0.5, 0, 0);
		vertices_all_geometry[20].normal = cgv::vec3(0.5, 0, 0);

		vertices_all_geometry[21].normal = cgv::vec3(0.5, 0, 0);
		vertices_all_geometry[22].normal = cgv::vec3(0.5, 0, 0);
		vertices_all_geometry[23].normal = cgv::vec3(0.5, 0, 0);

		//Top face
		vertices_all_geometry[24].pos = cgv::vec3(-0.5, 0.5, -0.5);
		vertices_all_geometry[25].pos = cgv::vec3(-0.5, 0.5, 0.5);
		vertices_all_geometry[26].pos = cgv::vec3(0.5, 0.5, -0.5);

		vertices_all_geometry[27].pos = cgv::vec3(0.5, 0.5, -0.5);
		vertices_all_geometry[28].pos = cgv::vec3(-0.5, 0.5, 0.5);
		vertices_all_geometry[29].pos = cgv::vec3(0.5, 0.5, 0.5);

		vertices_all_geometry[24].normal = cgv::vec3(0, 0.5, 0);
		vertices_all_geometry[25].normal = cgv::vec3(0, 0.5, 0);
		vertices_all_geometry[26].normal = cgv::vec3(0, 0.5, 0);

		vertices_all_geometry[27].normal = cgv::vec3(0, 0.5, 0);
		vertices_all_geometry[28].normal = cgv::vec3(0, 0.5, 0);
		vertices_all_geometry[29].normal = cgv::vec3(0, 0.5, 0);

		//Bottom face
		vertices_all_geometry[30].pos = cgv::vec3(-0.5, -0.5, -0.5);
		vertices_all_geometry[31].pos = cgv::vec3(0.5, -0.5, -0.5);
		vertices_all_geometry[32].pos = cgv::vec3(-0.5, -0.5, 0.5);

		vertices_all_geometry[33].pos = cgv::vec3(0.5, -0.5, 0.5);
		vertices_all_geometry[34].pos = cgv::vec3(-0.5, -0.5, 0.5);
		vertices_all_geometry[35].pos = cgv::vec3(0.5, -0.5, -0.5);

		vertices_all_geometry[30].normal = cgv::vec3(0, -0.5, 0);
		vertices_all_geometry[31].normal = cgv::vec3(0, -0.5, 0);
		vertices_all_geometry[32].normal = cgv::vec3(0, -0.5, 0);

		vertices_all_geometry[33].normal = cgv::vec3(0, -0.5, 0);
		vertices_all_geometry[34].normal = cgv::vec3(0, -0.5, 0);
		vertices_all_geometry[35].normal = cgv::vec3(0, -0.5, 0);

		for (int i = 0; i < 36; i++) {
			vertices_all_geometry[i].color = cgv::vec4(fractalColorR,fractalColorG,fractalColorB,1.0);
		}
		
		addFractalGeometryRecursive(0, cgv::math::identity4<double>(), fractalColor);
	}


	void drawFractalAllGeometry(cgv::render::context& ctx, const cgv::media::color<float>& color, unsigned max_depth)
	{

		// Render the cube
		material.set_diffuse_reflectance(color);
		ctx.set_material(material);

		vertex_array_all_geometry.enable(ctx);
		glDrawArrays(GL_TRIANGLES, 0, calculateNumOfVertices(max_depth));
		vertex_array_all_geometry.disable(ctx);
	}

public:
	cubes_fractal_drawable()
	{
		cubesFractal = cubes_fractal();
		fractalColorR = fractalColorG = fractalColorB = 0.5f;
		fractalColor = cgv::media::color<float>(fractalColorR, fractalColorG, fractalColorB);

		//init material 
		material.set_brdf_type(
			(cgv::media::illum::BrdfType)(cgv::media::illum::BT_LAMBERTIAN | cgv::media::illum::BT_PHONG)
		);
		material.ref_specular_reflectance() = { .0625f, .0625f, .0625f };
		material.ref_roughness() = .03125f;
	}

	std::string get_type_name() const {
		return "cubes_fractal_drawable";
	}

	bool self_reflect(cgv::reflect::reflection_handler& rh) {
		
		unsigned* vertexArrayType_uint = (unsigned*)&vertexArrayType;

		return 
			rh.reflect_member("drawFractal", drawFractal) &&
			rh.reflect_member("vertexArrayType", *vertexArrayType_uint) &&
			rh.reflect_member("maxDepth", maxDepth) &&
			rh.reflect_member("fractalColorR", fractalColorR) &&
			rh.reflect_member("fractalColorG", fractalColorG) &&
			rh.reflect_member("fractalColorB", fractalColorB);
	}

	void on_set(void* member_ptr) {

		if (member_ptr == &fractalColorR || 
			member_ptr == &fractalColorG || 
			member_ptr == &fractalColorB) 
		{
			fractalColor.R() = fractalColorR;
			fractalColor.G() = fractalColorG;
			fractalColor.B() = fractalColorB;
			update_member(&fractalColor);
		}

		if (member_ptr == &fractalColor) {
			fractalColorR = fractalColor.R();
			fractalColorG = fractalColor.G();
			fractalColorB = fractalColor.B();
		}

		if(member_ptr == &vertexArrayType)
		{
			if (vertexArrayType == Vertex_Array_Type::BUILTIN)
				cubesFractal.use_vertex_array(nullptr, 0, GL_TRIANGLES);
			else if (vertexArrayType == Vertex_Array_Type::INTERLEAVED)
				cubesFractal.use_vertex_array(&vertex_array_interleaved, vertices.size(), GL_TRIANGLES);
			else if (vertexArrayType == Vertex_Array_Type::NON_INTERLEAVED)
				cubesFractal.use_vertex_array(&vertex_array_non_interleaved, vertices.size(), GL_TRIANGLES);
		}

		update_member(member_ptr);

		// Also trigger a redraw in case the drawable node is active
		if (this->is_visible())
			post_redraw();
	}

	void create_gui() {
		add_decorator("Cubes Fractal", "heading", "level=1");

		add_member_control(this, "Draw Fractal", drawFractal);
		
		add_member_control(this, "Max Depth", maxDepth, "value_slider", "min=0;max="+std::to_string(MAX_DEPTH)+";ticks=true");
		
		add_member_control(this, "Fractal Color", fractalColor);

		add_member_control(this, "Vertex Array Type", vertexArrayType, "dropdown", "enums='Built-In,Interleaved,Non-Interleaved,All Geometry interleaved'");
	}

	bool init(cgv::render::context& ctx) {
		bool success = true;
		
		initUnitCubeGeometry();

		cgv::render::type_descriptor
			normType =
			cgv::render::element_descriptor_traits<cgv::vec3>
			::get_type_descriptor(vertices[0].normal),
			posType =
			cgv::render::element_descriptor_traits<cgv::vec3>
			::get_type_descriptor(vertices[0].pos);


		cgv::render::shader_program& default_shader
			= ctx.ref_surface_shader_program(true /* true for texture support */);

		
		// - create buffer objects interleaved
		success = vb.create(ctx, &(vertices[0]), vertices.size()) && success;

		success = vertex_array_interleaved.create(ctx) && success;

		success = vertex_array_interleaved.set_attribute_array(
			ctx, default_shader.get_position_index(), posType, vb,
			0, // position is at start of the struct <-> offset = 0
			vertices.size(), // number of position elements in the array
			sizeof(vertex) // stride from one element to next
		) && success;

		success = vertex_array_interleaved.set_attribute_array(
			ctx, default_shader.get_normal_index(), normType, vb,
			sizeof(cgv::vec3), // norm  follow after position
			vertices.size(), // number of norms elements in the array
			sizeof(vertex) // stride from one element to next
		) && success;


		// - create buffer objects non interleaved
		
		success = vb_non_interleaved.create(ctx, &(nonInterleavedVertexData[0]), nonInterleavedVertexData.size()) && success;
		success = vertex_array_non_interleaved.create(ctx) && success;

		success = vertex_array_non_interleaved.set_attribute_array(
			ctx, default_shader.get_position_index(), posType, vb_non_interleaved,
			0,
			vertices.size(),
			0
		) && success;

		success = vertex_array_non_interleaved.set_attribute_array(
			ctx, default_shader.get_normal_index(), normType, vb_non_interleaved,
			sizeof(cgv::vec3)* vertices.size(),
			vertices.size(),
			0
		) && success;

		// Task 1.3.1:
		// - create buffer objects for all geometry interleaved
		initAllGeometryToOneVB();

		cgv::render::type_descriptor colType = 
			cgv::render::element_descriptor_traits<cgv::vec4>
			::get_type_descriptor(vertices_all_geometry[0].color);

		success = vb_all_geometry.create(ctx, &(vertices_all_geometry[0]), vertices_all_geometry.size()) && success;

		success = vertex_array_all_geometry.create(ctx) && success;

		success = vertex_array_all_geometry.set_attribute_array(
			ctx, default_shader.get_position_index(), posType, vb_all_geometry,
			0,
			vertices_all_geometry.size(),
			sizeof(vertexColor)
		) && success;

		success = vertex_array_all_geometry.set_attribute_array(
			ctx, default_shader.get_normal_index(), normType, vb_all_geometry,
			sizeof(cgv::vec3),
			vertices_all_geometry.size(),
			sizeof(vertexColor)
		) && success;

		success = vertex_array_all_geometry.set_attribute_array(
			ctx, default_shader.get_color_index(), colType, vb_all_geometry,
			sizeof(cgv::vec3)*2,
			vertices_all_geometry.size(),
			sizeof(vertexColor)
		) && success;


		if (vertexArrayType == Vertex_Array_Type::BUILTIN)
			cubesFractal.use_vertex_array(nullptr, 0, GL_TRIANGLES);
		else if (vertexArrayType == Vertex_Array_Type::INTERLEAVED)
			cubesFractal.use_vertex_array(&vertex_array_interleaved, vertices.size(), GL_TRIANGLES);
		else if (vertexArrayType == Vertex_Array_Type::NON_INTERLEAVED)
			cubesFractal.use_vertex_array(&vertex_array_non_interleaved, vertices.size(), GL_TRIANGLES);	

		return success;
	}

	void init_frame(cgv::render::context& ctx) {

	}

	void draw(cgv::render::context& ctx) {

		if (!drawFractal)
			return;

		ctx.ref_surface_shader_program().enable(ctx);

		if (vertexArrayType == Vertex_Array_Type::ALL_GEOMETRY_INTERLEAVED)
			drawFractalAllGeometry(ctx, fractalColor, maxDepth);
		else
			cubesFractal.draw_recursive(ctx, fractalColor, maxDepth);

		ctx.ref_surface_shader_program().disable(ctx);
	}
};

// Create an instance of the demo class at plugin load and register it with the framework

// [END] Tasks 1.2a, 1.2b and 1.2c
// ************************************************************************************/


// ************************************************************************************/
// Task 1.2a: register an instance of your drawable.
cgv::base::object_registration<cubes_fractal_drawable> cgv_demo_registration("");
// < your code here >

