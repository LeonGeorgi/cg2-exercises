﻿// This source code is property of the Computer Graphics and Visualization 
// chair of the TU Dresden. Do not distribute! 
// Copyright (C) CGV TU Dresden - All Rights Reserved
//
#include "Bone.h"

#include <cgv/math/transformations.h>
#include <cgv/math/inv.h>

#include "math_helper.h"

Bone::Bone()
	: parent(nullptr), length(0.0f), direction_in_world_space(0.0, 0.0, 0.0), translationTransforms(0)
{}

Bone::~Bone()
{
	for (auto bone : children)
		delete bone;
	children.clear();

	dofs.clear();

	for (auto dof : orientation)
		delete dof;
	orientation.clear();
}

void Bone::calculate_matrices()
{
	orientationTransformGlobalToLocal.identity();
	std::for_each(orientation.begin(), orientation.end(), [&](AtomicTransform* t) {
		orientationTransformGlobalToLocal = orientationTransformGlobalToLocal * t->calculate_matrix();
	});
	orientationTransformLocalToGlobal = cgv::math::inv(orientationTransformGlobalToLocal);

	////
	// Task 3.1: Implement matrix calculationparent
	//as system transform 
	//orientation: 
	
	if (parent == nullptr) {
		orientationTransformPrevJointToCurrent = Mat4();
		orientationTransformPrevJointToCurrent.identity();
	} else {
		orientationTransformPrevJointToCurrent = parent->orientationTransformGlobalToLocal * this->orientationTransformLocalToGlobal;
	}
	//transformation:
	translationTransformCurrentJointToNext = Mat4();
	translationTransformCurrentJointToNext.identity();
	Vec4 dirLocal = orientationTransformGlobalToLocal * Vec4(direction_in_world_space, 0);
	translationTransformCurrentJointToNext(0, 3) = dirLocal[0] * length;
	translationTransformCurrentJointToNext(1, 3) = dirLocal[1] * length;
	translationTransformCurrentJointToNext(2, 3) = dirLocal[2] * length;
	

	////
	// Task 4.6: Implement matrix calculation (skinning)

}

Mat4 Bone::calculate_transform_prev_to_current_with_dofs()
{
	////
	// Task 3.1: Implement matrix calculation

	Mat4 t;
	t.identity();
	//TODO impelement degree of freedom
	t = t * orientationTransformPrevJointToCurrent;
	for (auto dof : dofs)
	{
		t = t * dof->calculate_matrix();
	}
	t = t * translationTransformCurrentJointToNext;
	return t;
}

Mat4 Bone::calculate_transform_prev_to_current_without_dofs()
{
	////
	// Task 3.1: Implement matrix calculation

	Mat4 t;
	t = orientationTransformPrevJointToCurrent * translationTransformCurrentJointToNext;
	return t;
}

void Bone::add_dof(AtomicTransform* dof)
{
	dof->set_index_in_amc((int)dofs.size());	
	if (dynamic_cast<AtomicTranslationTransform*>(dof))
	{
		dofs.push_front(std::shared_ptr<AtomicTransform>(dof));
		++translationTransforms;
	}
	else
		dofs.insert(dofs.begin() + translationTransforms, std::shared_ptr<AtomicTransform>(dof));
}

void Bone::set_name(const std::string& name) { this->name = name; }
const std::string& Bone::get_name() const { return name; }

void Bone::set_direction_in_world_space(const Vec3& direction) { this->direction_in_world_space = direction; }
const Vec3& Bone::get_direction_in_world_space() const { return direction_in_world_space; }

void Bone::set_length(float l) { this->length = l; }
float Bone::get_length() const { return length; }

void Bone::add_axis_rotation(AtomicRotationTransform* transform) { orientation.push_front(transform); }
void Bone::add_child(Bone* child)
{
	child->set_parent(this);
	children.push_back(child);
}
Bone* Bone::child_at(int i) const { return children[i]; }
int Bone::childCount() const { return (int)children.size(); }

void Bone::set_parent(Bone* parent)
{
	this->parent = parent;
}
Bone* Bone::get_parent() const { return parent; }

int Bone::dof_count() const { return (int)dofs.size(); }
std::shared_ptr<AtomicTransform> Bone::get_dof(int dofIndex) const { return dofs[dofIndex]; }

const Mat4& Bone::get_binding_pose_matrix() const
{
	return transformLocalToGlobal;
}

const Mat4& Bone::get_translation_transform_current_joint_to_next() const { return translationTransformCurrentJointToNext; }
const Mat4& Bone::get_orientation_transform_prev_joint_to_current() const { return orientationTransformPrevJointToCurrent; }

Vec4 Bone::get_bone_local_root_position() const { return Vec4(0, 0, 0, 1); }
Vec4 Bone::get_bone_local_tip_position() const { return translationTransformCurrentJointToNext * Vec4(0, 0, 0, 1); }
