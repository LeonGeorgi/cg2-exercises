#include <limits>
#include <cgv/math/fvec.h>
#include "implicit_group.h"

// ======================================================================================
//  Task 1.1b: GENERAL HINTS
//
//  The common super class of all CSG nodes is implicit_group. This class defines a
//  method ::get_implicit_child(unsigned int), which retrieves a pointer - already
//  casted to implicit_base<T>* - to the indicated child node, on which you can then call
//  ::evaluate() and ::evaluate_gradient().
//  Use the method ::get_nr_children() of the super super class cgv::base::group to query
//  the number of children registered with your operator.
//
// ======================================================================================

template <typename T>
class union_node : public implicit_group<T>
{
public:
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	union_node() { implicit_base<T>::gui_color = 0xffff00; }
	std::string get_type_name() const { return "union_node"; }

	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const
	{
		T smallest_child = std::numeric_limits<T>::max();
		for (unsigned int i = 0; i < get_nr_children(); ++i)
		{
			T child = get_implicit_child(i)->evaluate(p);
			if (child < smallest_child)
			{
				smallest_child = child;
				selected_i = i;
			}
		}
		return smallest_child;
	}

	T evaluate(const pnt_type& p) const
	{
		unsigned int selected_i;
		return eval_and_get_index(p, selected_i);
	}

	vec_type evaluate_gradient(const pnt_type& p) const
	{
		unsigned int selected_i;
		eval_and_get_index(p, selected_i);
		return get_implicit_child(selected_i)->evaluate_gradient(p);
	}
};

template <typename T>
class intersection_node : public implicit_group<T>
{
public:
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	intersection_node() { implicit_base<T>::gui_color = 0xffff00; }
	std::string get_type_name() const { return "intersection_node"; }

	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const
	{
		T largest_child = std::numeric_limits<T>::lowest();
		for (unsigned int i = 0; i < get_nr_children(); ++i)
		{
			T child = get_implicit_child(i)->evaluate(p);
			if (child > largest_child)
			{
				largest_child = child;
				selected_i = i;
			}
		}
		return largest_child;
	}

	T evaluate(const pnt_type& p) const
	{
		unsigned int selected_i;
		return eval_and_get_index(p, selected_i);
	}

	vec_type evaluate_gradient(const pnt_type& p) const
	{
		unsigned int selected_i;
		eval_and_get_index(p, selected_i);
		return get_implicit_child(selected_i)->evaluate_gradient(p);
	}
};

template <typename T>
class difference_node : public implicit_group<T>
{
public:
	typedef typename implicit_base<T>::vec_type vec_type;
	typedef typename implicit_base<T>::pnt_type pnt_type;

	difference_node() { implicit_base<T>::gui_color = 0xffff00; }
	std::string get_type_name() const { return "difference_node"; }

	T eval_and_get_index(const pnt_type& p, unsigned int& selected_i) const
	{
		T child_1 = get_implicit_child(0)->evaluate(p);
		T child_2 = get_implicit_child(1)->evaluate(p);

		if (child_1 > -child_2)
		{
			selected_i = 0;
			return child_1;
		}
		else
		{
			selected_i = 1;
			return -child_2;
		}
	}

	T evaluate(const pnt_type& p) const
	{
		unsigned int selected_i;
		return eval_and_get_index(p, selected_i);
	}

	vec_type evaluate_gradient(const pnt_type& p) const
	{
		unsigned int selected_i;
		eval_and_get_index(p, selected_i);
		if (selected_i == 0)
		{
			return get_implicit_child(0)->evaluate_gradient(p);
		}
		else
		{
			return -get_implicit_child(1)->evaluate_gradient(p);
		}
	}
};

scene_factory_registration<union_node<double> > sfr_union("union;+");
scene_factory_registration<intersection_node<double> > sfr_intersect("intersection;*");
scene_factory_registration<difference_node<double> > sfr_difference("difference;-");
