// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved
//
#pragma once

#include "common.h"
#include "DataStore.h"

#include <cgv/gui/trigger.h>
#include <cgv/gui/provider.h>
#include <cgv/gui/event_handler.h>
#include <cgv/media/illum/surface_material.h>
#include <cgv/render/drawable.h>
#include <cgv/render/context.h>
#include <cgv_gl/gl/gl.h>

using namespace cgv::base;
using namespace cgv::signal;
using namespace cgv::gui;
using namespace cgv::math;
using namespace cgv::render;
using namespace cgv::utils;

class SkeletonViewer : public node, public drawable, public provider
{
private:
	DataStore* data;
	gui_group_ptr tree_view;
	gui_group_ptr bone_group;
	cgv::media::illum::surface_material material;


	// Maps gui elements in the tree view to a specific bone
	std::map<base_ptr, Bone*> gui_to_bone;

	// slot for the signal
	void timer_event(double t, double dt);
	void skeleton_changed(std::shared_ptr<Skeleton>);

	void generate_tree_view_nodes();
	void generate_tree_view_nodes(gui_group_ptr parent, Bone* bone);

	void tree_selection_changed(base_ptr p, bool select);
	void generate_bone_gui(Bone* bone);

	void load_skeleton();
	void write_pinocchio();
	void load_pinocchio();
	void load_animation();
	void start_choose_base();

	void draw_skeleton_subtree(Bone* node, const Mat4& global_to_parent_local, context& ctx, int level);

	void dof_changed(double new_value);
	void recursive_connect_signals(Bone* b);

	bool is_dof_rotation(std::string dof_title);
	Mat4 get_rotation_matrix(std::string dof_title, Vec3 dir);
	

	void start_animation();
	void stop_animation();

public:
	// The constructor of this class
	SkeletonViewer(DataStore*);	

	// Create the gui elements
	void create_gui();
	// Draw the scene
	void draw(context& ctx);

	Animation animation;
	bool do_animation = false;
	int animation_frame = 0;

	std::string get_parent_type() const;
};

