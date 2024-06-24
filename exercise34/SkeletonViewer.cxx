// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved
//
#include "SkeletonViewer.h"

#include <cgv/utils/ostream_printf.h>
#include <cgv/gui/file_dialog.h>
#include <cgv/gui/dialog.h>
#include <cgv/gui/key_event.h>
#include <cgv/render/view.h>
#include <cgv/base/find_action.h>
#include <cgv/media/illum/surface_material.h>

#include "math_helper.h"

using namespace cgv::utils;

cgv::render::shader_program Mesh::prog;

cgv::rgb colors[] = {
	{1.0,0.0,0.0},
	{0.77,0.53,0.09},
	{0.38,0.37,0.46},
	{0.76,0.34,0,60},
	{0.67,0.83,0,38},
	{0,78,0.68,0.09},
	{0.67,0,57,0.30},
	{0.21,0.47,0.41},
	{0.80,0.56,0.14},
	{0.60,0,79,0.72}
};

// The constructor of this class
SkeletonViewer::SkeletonViewer(DataStore* data)
	: node("Skeleton Viewer"), data(data)
	/* Bonus task: initialize members relevant for animation */
{	
	connect(data->skeleton_changed, this, &SkeletonViewer::skeleton_changed);

	connect(get_animation_trigger().shoot, this, &SkeletonViewer::timer_event);

	// Prepare surface material
	material.set_brdf_type((cgv::media::illum::BrdfType)(cgv::media::illum::BT_LAMBERTIAN | cgv::media::illum::BT_PHONG));
	material.ref_specular_reflectance() = { .03125f, .03125f, .03125f };
	material.ref_roughness() = .03125f;
}

//draws a part of a skeleton, represented by the given root node
void SkeletonViewer::draw_skeleton_subtree(Bone* node, const Mat4& global_to_parent_local, context& ctx, int level)
{
	////
	// Task 3.2, 4.3: Visualize the skeleton
	
	if (node->get_name() != "root") {

		Vec4 origin = global_to_parent_local * Vec4(0, 0, 0, 1);
		//Vec4 end = global_to_parent_local * node->calculate_transform_prev_to_current_without_dofs() * Vec4(0, 0, 0, 1);
		Vec4 end = global_to_parent_local * node->calculate_transform_prev_to_current_with_dofs() * Vec4(0, 0, 0, 1);

		/* old linesegment code
		//ctx.set_color(colors[level]);
		glLineWidth(3);

		float vertices[] = {
				origin.x(),origin.y(),origin.z(), // left  
				end.x(),end.y(),end.z(), // right 
		};

		unsigned int VBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		glBindVertexArray(VAO); 
		glDrawArrays(GL_LINES, 0, 2);
		glLineWidth(1);
	
		// Draw the bone as an arrow	
		*/

		Vec3 startArrow = Vec3(origin.x(), origin.y(), origin.z());
		Vec3 endArrow = Vec3(end.x(), end.y(), end.z());

		material.set_diffuse_reflectance(colors[level]);
		ctx.set_material(material);
		ctx.tesselate_arrow(startArrow,endArrow,0.05);
	
		ctx.push_modelview_matrix();


		// draw disks
		Mat4 translation = Mat4();
		translation.identity();
		translation(0, 3) = startArrow.x();
		translation(1, 3) = startArrow.y();
		translation(2, 3) = startArrow.z();
		ctx.ref_surface_shader_program().disable(ctx);
		ctx.ref_default_shader_program().enable(ctx);

		for(int i = 0; i < node->dof_count(); i++)
		{
			if (is_dof_rotation(node->get_dof(i)->get_name())) {
				ctx.push_modelview_matrix();

				// rotate the disk to the dof's axis doesnt work yet TODO
				//ctx.mul_modelview_matrix(get_rotation_matrix(node->get_dof(i)->get_name(),(endArrow-startArrow)));
				ctx.mul_modelview_matrix(translation);
			

				ctx.set_color(cgv::rgba(0, 1, 0, 0.2));
				ctx.tesselate_unit_disk();

				ctx.pop_modelview_matrix();
			}
		}
		ctx.ref_default_shader_program().disable(ctx);
		ctx.ref_surface_shader_program().enable(ctx);
	
	}

	for (int i = 0; i < node->childCount(); i++) {
		//draw_skeleton_subtree(node->child_at(i), global_to_parent_local * node->calculate_transform_prev_to_current_without_dofs(), ctx, level + 1);
		draw_skeleton_subtree(node->child_at(i), global_to_parent_local * node->calculate_transform_prev_to_current_with_dofs(), ctx, level + 1);
	}
}

void SkeletonViewer::timer_event(double t, double dt)
{
	////
	// Bonus task: implement animation */

	if (do_animation) {
		animation.apply_frame(animation_frame);
		animation_frame++;

		if (animation_frame >= animation.frame_count()) {
			animation_frame = 0;
		}
	}
}

void SkeletonViewer::start_animation()
{
	////
	// Bonus task: implement animation
	do_animation = true;
}

void SkeletonViewer::stop_animation()
{
	////
	// Bonus task: implement animation
	do_animation = false;

}

void SkeletonViewer::skeleton_changed(std::shared_ptr<Skeleton> s) 
{
	// This function is called whenever the according signal of the
	// data store has been called.

	//Rebuild the tree-view
	generate_tree_view_nodes();

	//Fit view to skeleton
	std::vector<cgv::render::view*> view_ptrs;
	cgv::base::find_interface<cgv::render::view>(get_node(), view_ptrs);
	if (view_ptrs.empty()) {
		// If there is no view, we cannot update it
		cgv::gui::message("could not find a view to adjust!!");
	}
	else
	{
		Vec3 center = (s->getMin() + s->getMax()) * 0.5;
		view_ptrs[0]->set_focus(center.x(), center.y(), center.z());
		// Set the scene's size at the focus point
		view_ptrs[0]->set_y_extent_at_focus(s->getMax().y() - s->getMin().y());
	}	

	//connect signals	
	recursive_connect_signals(s->get_root());	

	post_redraw();
}

void SkeletonViewer::recursive_connect_signals(Bone* b)
{
	for (int i = 0; i < b->dof_count(); ++i)
		connect(b->get_dof(i)->changed_signal, this, &SkeletonViewer::dof_changed);
	for (int i = 0; i < b->childCount(); ++i)
		recursive_connect_signals(b->child_at(i));
}

bool SkeletonViewer::is_dof_rotation(std::string dof_title)
{
	if(dof_title == "Z-Rotation" || dof_title == "Y-Rotation" || dof_title == "X-Rotation")
		return true;
	else
		return false;
}

Mat4 SkeletonViewer::get_rotation_matrix(std::string dof_title, Vec3 dir)
{
	Mat4 ret = Mat4();

	Vec3 rot = Vec3();
	if (dof_title == "X-Rotation") {
		Vec3 rot = Vec3(1, 0, 0);
	}
	else if (dof_title == "Y-Rotation") {
		Vec3 rot = Vec3(0, 1, 0);
	}
	else if (dof_title == "Z-Rotation") {
		Vec3 rot = Vec3(0, 0, 1);
	}

	Vec3 xAxis =  cgv::math::cross(rot,dir);
	xAxis.normalize();


	Vec3 yAxis = cgv::math::cross(dir, xAxis);
	yAxis.normalize();


	ret(0, 0) = xAxis.x();
	ret(0, 1) = xAxis.y();
	ret(0, 2) = xAxis.z();
	ret(0, 3) = 0;

	ret(1, 0) = yAxis.x();
	ret(1, 1) = yAxis.y();
	ret(1, 2) = yAxis.z();
	ret(1, 3) = 0;

	ret(2, 0) = dir.x();
	ret(2, 1) = dir.y();
	ret(2, 2) = dir.z();
	ret(2, 3) = 0;

	ret(3, 0) = 0;
	ret(3, 1) = 0;
	ret(3, 2) = 0;
	ret(3, 3) = 1;

	return ret;

}

void SkeletonViewer::dof_changed(double)
{
	if (!data->dof_changed_by_ik)
		data->set_endeffector(nullptr);

	post_redraw();
}

void SkeletonViewer::generate_tree_view_nodes()
{
	tree_view->remove_all_children();
	gui_to_bone.clear();

	if (!data->get_skeleton() || !data->get_skeleton()->get_root())
		return;
	generate_tree_view_nodes(tree_view, data->get_skeleton()->get_root());
}

void SkeletonViewer::generate_tree_view_nodes(gui_group_ptr parent, Bone* bone)
{
	if (bone->childCount() == 0)
	{
		//If this is a leaf, use a button
		auto button = parent->add_button(bone->get_name(), "", "");
		gui_to_bone[button] = bone;
	}
	else
	{
		//If this is not a leaf, use a group
		auto g = parent->add_group(bone->get_name(), "", "", "");
		gui_to_bone[g] = bone;
		for (int i = 0; i < bone->childCount(); ++i)
			generate_tree_view_nodes(g, bone->child_at(i));
	}
}

void SkeletonViewer::start_choose_base()
{
	Bone* b = data->get_endeffector();
	data->set_endeffector(nullptr);
	data->set_base(b);
}

void SkeletonViewer::tree_selection_changed(base_ptr p, bool select)
{
	bone_group->remove_all_children();

	if (select)		
	{
		Bone* bone = gui_to_bone.at(p);
		generate_bone_gui(bone);
		data->set_endeffector(bone);
	}
	else
	{
		data->set_endeffector(nullptr);
	}
}

std::string SkeletonViewer::get_parent_type() const
{
	return "layout_group";
}

void SkeletonViewer::load_skeleton()
{
	std::string filename = cgv::gui::file_open_dialog("Open", "Skeleton Files (*.asf):*.asf");
	if (!filename.empty())
	{
		auto s = std::make_shared<Skeleton>();
		if (s->fromASFFile(filename))
		{
			data->set_skeleton(s);
			data->set_endeffector(nullptr);
			data->set_base(s->get_root());
		}
		else
		{
			cgv::gui::message("Could not load specified ASF file.");
		}
	}
}

void SkeletonViewer::write_pinocchio()
{
	if (!data->get_skeleton())
	{
		cgv::gui::message("An ASF skeleton has to be loaded first.");
		return;
	}

	std::string filename = cgv::gui::file_save_dialog("Save", "Pinocchio Skeleton (*.txt):*.txt");
	if (!filename.empty())
	{
		data->get_skeleton()->write_pinocchio_file(filename);
	}
}

void SkeletonViewer::load_pinocchio()
{
	if (!data->get_skeleton())
	{
		cgv::gui::message("An ASF skeleton has to be loaded first.");
		return;
	}

	std::string filename = cgv::gui::file_open_dialog("Open", "Pinocchio skeleton (*.out):*.out");
	if (!filename.empty())
	{
		data->get_skeleton()->read_pinocchio_file(filename);		
		skeleton_changed(data->get_skeleton());
	}
}

void SkeletonViewer::load_animation()
{
	if (!data->get_skeleton())
	{
		cgv::gui::message("An ASF skeleton has to be loaded first.");
		return;
	}

	std::string filename = cgv::gui::file_open_dialog("Open", "Animation File (*.amc):*.amc");
	if (!filename.empty())
	{
		animation.read_amc_file(filename, data->get_skeleton().get());
		/*Bonus task: load animation from selected file */
	}
}

// Create the gui elements
void SkeletonViewer::create_gui()
{	
	//Bone tree view
	parent_group->multi_set("layout='table';rows=3;spacings='normal';");	

	tree_view = add_group("", "tree_group", "h=300;column_heading_0='Bones';column_width_0=-1", "f");	
	bone_group = add_group("", "align_group", "h=150", "f");
	
	auto dock_group = add_group("", "dockable_group", "", "fF");

	connect(tree_view->on_selection_change, this, &SkeletonViewer::tree_selection_changed);

	generate_tree_view_nodes();

	//Other GUI elements
	auto gui_group = dock_group->add_group("", "align_group", "", "f");

	connect_copy(gui_group->add_button("Load ASF skeleton", "", "\n")->click,
		rebind(this, &SkeletonViewer::load_skeleton));	

	connect_copy(gui_group->add_button("Load Animation", "", "\n")->click,
		rebind(this, &SkeletonViewer::load_animation));

	connect_copy(gui_group->add_button("Start Animation", "", "\n")->click,
		rebind(this, &SkeletonViewer::start_animation));

	connect_copy(gui_group->add_button("Stop Animation", "", "\n")->click,
		rebind(this, &SkeletonViewer::stop_animation));

	connect_copy(gui_group->add_button("Choose IK Base", "", "\n")->click,
		rebind(this, &SkeletonViewer::start_choose_base));
		
	connect_copy(gui_group->add_button("Write Pinocchio skeleton", "", "\n")->click,
		rebind(this, &SkeletonViewer::write_pinocchio));

	connect_copy(gui_group->add_button("Load Pinocchio skeleton", "", "\n")->click,
		rebind(this, &SkeletonViewer::load_pinocchio));
}

void SkeletonViewer::generate_bone_gui(Bone* bone)
{
	// Add bone-specific gui elements to bone_group.
	// Use the layout "\n" to specify vertical alignment

	bone_group->add_view("Selected Bone", bone->get_name());

	for (int i = 0; i < bone->dof_count(); ++i)
	{
		auto dof = bone->get_dof(i);
		auto slider = bone_group->add_control<double>(dof->get_name(), dof.get(), "value_slider");
		slider->set("min", dof->get_lower_limit());
		slider->set("max", dof->get_upper_limit());
	}
}

void SkeletonViewer::draw(context& ctx)
{
	////
	// Task 3.3: If you require it, extend this method with additional logic

	//turn backface culling off so disk is visible from both sides
	ctx.set_cull_state(CM_OFF);

	//turn on blending for transparency
	/*
	ctx.set_blend_state({
		true,
		BF_SRC_ALPHA, BF_ONE_MINUS_SRC_ALPHA,
		BF_SRC_ALPHA, BF_ONE_MINUS_SRC_ALPHA
	});
	*/

	// TODO figure the correct blending function out
	glEnable(GL_BLEND); //Enable blending.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Shortcut to the built-in default shader with lighting and texture support
	ctx.ref_surface_shader_program().enable(ctx);
	if (data->get_skeleton() != nullptr)
		draw_skeleton_subtree(data->get_skeleton()->get_root(), data->get_skeleton()->get_origin(), ctx, 0);
	ctx.ref_surface_shader_program().disable(ctx);

}
